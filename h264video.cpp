#include "h264video.h"

H264Video::H264Video(QObject *parent):QObject(parent)
{
    
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
    //打开视频流
    qDebug()<<url;
    int result=avformat_open_input(&pAVFormatContext, url.toStdString().c_str(),NULL,NULL);
    if (result<0){
        emit sendToLog("fail to open stream");
        qDebug()<<"fail to open stream";
        return false;
    }
    
    //获取视频流信息
    result=avformat_find_stream_info(pAVFormatContext,NULL);
    if (result<0){
        emit sendToLog("fail to get stream info");
        qDebug()<<"fail to get stream info";
        return false;
    }
    
    //获取视频流索引
    videoStreamIndex = -1;
    for (uint i = 0; i < pAVFormatContext->nb_streams; i++) {
        if (pAVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }
    
    if (videoStreamIndex==-1){
        emit sendToLog("fail to get stream index");
        qDebug()<<"fail to get stream index";
        return false;
    }
    
    //获取视频流的分辨率大小
    pAVCodecContext = pAVFormatContext->streams[videoStreamIndex]->codec;
    videoWidth=pAVCodecContext->width;
    videoHeight=pAVCodecContext->height;
    
    avpicture_alloc(&pAVPicture,AV_PIX_FMT_RGB24,videoWidth,videoHeight);
    
    //获取视频流解码器
    pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
    pSwsContext = sws_getContext(videoWidth,videoHeight,AV_PIX_FMT_YUV420P,videoWidth,videoHeight,AV_PIX_FMT_RGB24,SWS_BICUBIC,0,0,0);
    
    //打开对应解码器
    result=avcodec_open2(pAVCodecContext,pAVCodec,NULL);
    if (result<0){
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
    int frameFinished=0;
    while (VideoStatus){
        StopTimer->stop();
        if (av_read_frame(pAVFormatContext, &pAVPacket) >= 0){
            if(pAVPacket.stream_index==videoStreamIndex){
                avcodec_decode_video2(pAVCodecContext, pAVFrame, &frameFinished, &pAVPacket);
                if (frameFinished){

                    mutex.lock();
                    sws_scale(pSwsContext,(const uint8_t* const *)pAVFrame->data,pAVFrame->linesize,0,videoHeight,pAVPicture.data,pAVPicture.linesize);
                    //发送获取一帧图像信号
                    QImage image(pAVPicture.data[0],videoWidth,videoHeight,QImage::Format_RGB888);
                    videoinfo.count++;
//                    emit getVideoInfo(videoinfo.count,videoinfo.videotimer.elapsed()+videoinfo.pretime,videoinfo.videotimer.elapsed()-videoinfo.oncetime);

                    if(frameindex==VIDEO_SHOW_CAMERA)
                        emit getImage_Camera(image);
                    if(frameindex==VIDEO_SHOW_RUN)
                        emit getImage_Run(image);
                    mutex.unlock();

                }
            }
//         videoinfo.oncetime=videoinfo.videotimer.elapsed();
        }
        av_packet_unref(&pAVPacket);//释放资源,否则内存会一直上升
    }
    
    
}

void H264Video::H264VideoFree()
{
//    avcodec_close(pAVFormatContext);
    avformat_close_input(&pAVFormatContext);
    if(pAVFormatContext!=NULL)
    {
        avformat_free_context(pAVFormatContext);
        pAVFormatContext=NULL;
    }
    if(pAVFrame!=NULL)
    {
        av_frame_free(&pAVFrame);
        pAVFrame=NULL;
    }
    //    avpicture_free(&pAVPicture);
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
        emit clearImage();
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
        InitVideoInfo();

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
    videoinfo.videotimer.restart();
    videoinfo.oncetime=0;
    H264VideoPlay(cmd);
    videoinfo.pretime+=videoinfo.videotimer.elapsed();
    preSatus=cmd;

}

void H264Video::H264VideoTimeout()
{
    emit sendToLog("Video Timer timeout, close stream.");
    qDebug()<<"Video Timer timeout, close stream.";
    H264VideoFree();

}

void H264Video::InitVideoInfo()
{
    videoinfo.count=0;
    videoinfo.videotimer.start();
    videoinfo.pretime=0;
    videoinfo.oncetime=0;
}


