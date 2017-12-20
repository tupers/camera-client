#include "video_core.h"

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
    while(m_bState)
    {
        if(av_read_frame(m_pFormatContext,m_pPacket)>=0)
        {

        }
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
