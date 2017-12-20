#include "h264video.h"

H264Video::H264Video(video_bufQueue *bufQueue, QObject *parent):QObject(parent)
{
    if(bufQueue!=0)
        m_hBufQueue = bufQueue;
}

H264Video::~H264Video()
{
    H264VideoFree();
    if(StopTimer!=NULL)
    {
        StopTimer->deleteLater();
        StopTimer=NULL;
    }
}

void H264Video::H264VideoInit()
{
    videoStreamIndex=-1;
    av_register_all();//注册库中所有可用的文件格式和解码器
    avformat_network_init();//初始化网络流格式,使用RTSP网络流时必须先执行
    pAVFormatContext = avformat_alloc_context();//申请一个AVFormatContext结构的内存,并进行简单初始化
    pAVFrame=av_frame_alloc();
    pAVFrameGrey=av_frame_alloc();
    pAVPacket = av_packet_alloc();
    if(StopTimer!=NULL)
    {
        delete StopTimer;
        StopTimer=NULL;
    }
    StopTimer = new QTimer();
    StopTimer->setInterval(15000);
    StopTimer->setSingleShot(true);
    connect(StopTimer,SIGNAL(timeout()),this,SLOT(H264VideoTimeout()));
    
}

bool H264Video::H264VideoOpenStream()
{
    emit clearImage();
    //打开视频流
    qDebug()<<url;
    //set udp buffer size
    //AVDictionary* options = NULL;
    //av_dict_set(&options, "buffer_size", "1024000", 0);

    if(avformat_open_input(&pAVFormatContext, url.toStdString().c_str(),NULL,NULL)<0)
    {
        emit sendToLog("fail to open stream");
        qDebug()<<"fail to open stream";
        return false;
    }
    
    //获取视频流信息
    if(avformat_find_stream_info(pAVFormatContext,NULL)<0)
    {
        emit sendToLog("fail to get stream info");
        qDebug()<<"fail to get stream info";
        return false;
    }
    
    //获取视频流索引

    videoStreamIndex = -1;
    videoStreamIndex = av_find_best_stream(pAVFormatContext,AVMEDIA_TYPE_VIDEO,-1,-1,&pAVCodec,0);
    
    if (videoStreamIndex==-1){
        emit sendToLog("fail to get stream index");
        qDebug()<<"fail to get stream index";
        return false;
    }
    pAVCodecContext = avcodec_alloc_context3(NULL);

    if(pAVCodecContext==NULL)
    {
        emit sendToLog("Could not allocate AVCodecContext");
        qDebug()<<"Could not allocate AVCodecContext";
        return false;
    }
    avcodec_parameters_to_context(pAVCodecContext,pAVFormatContext->streams[videoStreamIndex]->codecpar);
    
    //获取视频流解码器
    if(pAVCodec==NULL)
        pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
    if(pAVCodec == NULL)
    {
        emit sendToLog("Codec not found");
        qDebug()<<"Codec not found";
        return false;
    }

    //fill grey scale out put frame
    out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_GRAY8, pAVCodecContext->width, pAVCodecContext->height, 1));
    av_image_fill_arrays(pAVFrameGrey->data, pAVFrameGrey->linesize, out_buffer, AV_PIX_FMT_GRAY8, pAVCodecContext->width, pAVCodecContext->height, 1);

    pSwsContext = sws_getContext(pAVCodecContext->width,pAVCodecContext->height,pAVCodecContext->pix_fmt,pAVCodecContext->width,pAVCodecContext->height,AV_PIX_FMT_GRAY8,SWS_BICUBIC,0,0,0);

    //打开对应解码器
    if(avcodec_open2(pAVCodecContext,pAVCodec,NULL)<0)
    {
        emit sendToLog("open decoder failed");
        qDebug()<<"open decoder failed";
        return false;
    }
    
    emit sendToLog("ready to play");
    qDebug()<<"ready to play";
    return true;
}

void H264Video::H264VideoPlay(int frameindex)
{
    //一帧一帧读取视频
    while (VideoStatus){
        StopTimer->stop();
        if (av_read_frame(pAVFormatContext, pAVPacket) >= 0){
            if(pAVPacket->stream_index==videoStreamIndex){
                int ret = avcodec_send_packet(pAVCodecContext,pAVPacket);
                if(ret!=0)
                    continue;
                while(avcodec_receive_frame(pAVCodecContext,pAVFrame)==0)
                {
                    mutex.lock();
                    sws_scale(pSwsContext,(const uint8_t* const *)pAVFrame->data,pAVFrame->linesize,0,pAVCodecContext->height,pAVFrameGrey->data,pAVFrameGrey->linesize);
                    QImage image(pAVFrameGrey->data[0],pAVCodecContext->width,pAVCodecContext->height,QImage::Format_Grayscale8);
                    //videoinfo.count++;
                    if(m_hBufQueue)
                    {
                        auto ret = m_hBufQueue->bufInput(image.bits());
                        if(ret)
                            emit getImage(frameindex);
                        else
                        {
                            qDebug()<<"no enough empty buf in the list";
                            QThread::msleep(1);
                        }
                    }
                    else
                    {
                    if(frameindex==VIDEO_SHOW_CAMERA)
                        emit getImage_Camera(image);
                    if(frameindex==VIDEO_SHOW_RUN)
                        emit getImage_Run(image);
                    }
                    mutex.unlock();

                }
            }
            //         videoinfo.oncetime=videoinfo.videotimer.elapsed();
        }
        av_packet_unref(pAVPacket);//释放资源,否则内存会一直上升
    }
    
    
}

void H264Video::H264VideoFree()
{
    avcodec_close(pAVCodecContext);
    avformat_close_input(&pAVFormatContext);
    if(pAVCodecContext!=NULL)
    {
        avcodec_free_context(&pAVCodecContext);
        pAVCodecContext=NULL;
    }
    if(pAVPacket!=NULL)
    {
        av_packet_free(&pAVPacket);
        pAVPacket=NULL;
    }
    if(pAVFrame!=NULL)
    {
        av_frame_free(&pAVFrame);
        pAVFrame=NULL;
    }
    if(pAVFrameGrey!=NULL)
    {
        av_frame_free(&pAVFrameGrey);
        pAVFrameGrey=NULL;
    }
    if(out_buffer!=NULL)
    {
        av_free(out_buffer);
        out_buffer=NULL;
    }
    if(pSwsContext!=NULL)
    {
        sws_freeContext(pSwsContext);
        pSwsContext=NULL;
    }
    InitFlag=false;
    emit closeVideo();
}


void H264Video::H264VideoOpen(int cmd)
{
    emit sendToLog("video control cmd: "+QString::number(cmd,10));
    qDebug()<<"video control cmd: "+QString::number(cmd,10);
    if(cmd==VIDEO_TERMINATE)//logout procedure
    {
        emit sendToLog("close stream.");
        qDebug()<<"close stream.";
        if(StopTimer!=NULL)
        {
            StopTimer->stop();
        }
        H264VideoFree();
        //emit clearImage();
        return;
    }

    if(InitFlag==false)
    {
        emit sendToLog("Open stream first.");
        qDebug()<<"Open stream first.";
        H264VideoInit();
        H264VideoOpenStream();
        InitFlag=true;
        VideoStatus=VIDEO_RUN;
        preSatus=cmd;
        //InitVideoInfo();

    }
    else if(preSatus!=cmd)
    {
        emit clearImage();
        VideoStatus=VIDEO_RUN;
    }
    if(VideoStatus==VIDEO_STOP)
    {
        emit sendToLog("status: stop, start Video Timer...");
        qDebug()<<"status: stop, start Video Timer...";
        StopTimer->start();
    }
    //videoinfo.videotimer.restart();
    //videoinfo.oncetime=0;
    H264VideoPlay(cmd);
    //videoinfo.pretime+=videoinfo.videotimer.elapsed();
    preSatus=cmd;

}

void H264Video::H264VideoTimeout()
{
    emit sendToLog("Video Timer timeout, close stream.");
    qDebug()<<"Video Timer timeout, close stream.";
    H264VideoFree();
    emit clearImage();

}


