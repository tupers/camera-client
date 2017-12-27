#include "video_core.h"

video_core::video_core(int num,video_bufQueue** bufQueue, QObject *parent)
{
    m_vecBufQueue.clear();
    for(int i=0;i<num;i++)
        m_vecBufQueue.push_back(bufQueue[i]);
    m_tStopTimer.setInterval(5000);
    m_tStopTimer.setSingleShot(true);
    connect(this,&video_core::readyToClose,this,&video_core::videoClose);
    connect(&m_tStopTimer,&QTimer::timeout,this,[=](){m_bState=VideoStatus_TERMINATE;});
}

void video_core::videoInit()
{
    //ffmpeg init
    av_register_all();
    avformat_network_init();
    m_pFormatContext = avformat_alloc_context();
    m_pFrame = av_frame_alloc();
    m_pFrameGrey = av_frame_alloc();
    m_pPacket = av_packet_alloc();
    m_pCodecContext = avcodec_alloc_context3(NULL);

}

bool video_core::getVideoInfo(QString url)
{
    //AVDictionary* options = NULL;
    //av_dict_set(&options, "buffer_size", "1024000", 0);
    //if(avformat_open_input(&m_pFormatContext, url.toStdString().c_str(),NULL,&options)<0)

    //open stream
    if(avformat_open_input(&m_pFormatContext, url.toStdString().c_str(),NULL,NULL)<0)
    {
        emit sendToLog("fail to open stream");
        return false;
    }
    //get stream info
    if(avformat_find_stream_info(m_pFormatContext,NULL)<0)
    {
        emit sendToLog("fail to get stream info");
        return false;
    }
    //find video index
    m_nVideoStram = av_find_best_stream(m_pFormatContext,AVMEDIA_TYPE_VIDEO,-1,-1,&m_pCodec,0);
    if(m_nVideoStram==-1)
    {
        emit sendToLog("fail to get stream index");
        return false;
    }
    //find codec
    avcodec_parameters_to_context(m_pCodecContext,m_pFormatContext->streams[m_nVideoStram]->codecpar);
    if(m_pCodec==NULL)
        m_pCodec = avcodec_find_decoder(m_pCodecContext->codec_id);
    if(m_pCodec == NULL)
    {
        emit sendToLog("Codec not found");
        return false;
    }

    //fill grey scale out put frame
    m_pFrameBuffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_GRAY8, m_pCodecContext->width, m_pCodecContext->height, 1));
    av_image_fill_arrays(m_pFrameGrey->data, m_pFrameGrey->linesize, m_pFrameBuffer, AV_PIX_FMT_GRAY8, m_pCodecContext->width, m_pCodecContext->height, 1);

    //fill sws context
    m_pSwsContext = sws_getContext(m_pCodecContext->width,m_pCodecContext->height,m_pCodecContext->pix_fmt,m_pCodecContext->width,m_pCodecContext->height,AV_PIX_FMT_GRAY8,SWS_BICUBIC,0,0,0);

    return true;
}

void video_core::videoRun()
{
    while(m_bState==VideoStatus_OPEN)
    {
        if(m_bContrl==VideoCrtl_PLAY)
        {
            if(av_read_frame(m_pFormatContext,m_pPacket)>=0)
            {
                int ret = avcodec_send_packet(m_pCodecContext,m_pPacket);
                if(ret!=0)
                    continue;
                while(avcodec_receive_frame(m_pCodecContext,m_pFrame)==0)
                {
                    sws_scale(m_pSwsContext,(const uint8_t* const *)m_pFrame->data,m_pFrame->linesize,0,m_pCodecContext->height,m_pFrameGrey->data,m_pFrameGrey->linesize);
                    //QImage image(m_pFrameGrey->data[0],m_pCodecContext->width,m_pCodecContext->height,QImage::Format_Grayscale8);
                    if(m_vecBufQueue.size())
                    {
                        auto ret = m_vecBufQueue[0]->bufInput(m_pFrameGrey->data[0]);
                        if(ret)
                            emit getImage();
                        else
                        {
                            emit sendToLog("no enough empty buf in the list");
                            QThread::msleep(1);
                        }
                    }
                }
            }
        }
    }
    emit readyToClose();
}

void video_core::videoControl(video_core::VideoCtrlCmd cmd)
{
    if(cmd==VideoCrtl_PLAY)
        m_tStopTimer.stop();
    else if(cmd==VideoCrtl_STOP)
    {
        m_tStopTimer.start();
        emit sendToLog("Start video stop timer...");
    }
}

void video_core::videoOpen(QString url)
{
    if(!getVideoInfo(url))
        return;

    //open codec
    if(avcodec_open2(m_pCodecContext,m_pCodec,NULL)<0)
    {
        emit sendToLog("open decoder failed");
        return;
    }

    emit sendToLog("ready to play");
}

void video_core::videoClose()
{
    avcodec_close(m_pCodecContext);
    avformat_close_input(&m_pFormatContext);
    if(m_pCodecContext!=NULL)
    {
        avcodec_free_context(&m_pCodecContext);
        m_pCodecContext=NULL;
    }
    if(m_pPacket!=NULL)
    {
        av_packet_free(&m_pPacket);
        m_pPacket=NULL;
    }
    if(m_pFrame!=NULL)
    {
        av_frame_free(&m_pFrame);
        m_pFrame=NULL;
    }
    if(m_pFrameGrey!=NULL)
    {
        av_frame_free(&m_pFrameGrey);
        m_pFrameGrey=NULL;
    }
    if(m_pFrameBuffer!=NULL)
    {
        av_free(m_pFrameBuffer);
        m_pFrameBuffer=NULL;
    }
    if(m_pSwsContext!=NULL)
    {
        sws_freeContext(m_pSwsContext);
        m_pSwsContext=NULL;
    }
}
