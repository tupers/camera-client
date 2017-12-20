#ifndef VIDEO_CORE_H
#define VIDEO_CORE_H

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
}

#include <QObject>
#include "utils/video_bufqueue.h"

#define VIDEO_RUN   true
#define VIDEO_STOP  false

class video_core : public QObject
{
    Q_OBJECT
public:
    explicit video_core(video_bufQueue* bufQueue=NULL,QObject *parent = nullptr):m_hBufQueue(bufQueue){videoInit();}
private:
    void videoInit();
    bool getVideoInfo(QString url);
    void videoRun();

    video_bufQueue* m_hBufQueue=NULL;

    AVFormatContext* m_pFormatContext=NULL;
    AVCodecContext* m_pCodecContext=NULL;
    SwsContext* m_pSwsContext=NULL;
    AVPacket* m_pPacket = NULL;
    AVCodec* m_pCodec = NULL;
    AVFrame* m_pFrame=NULL;
    AVFrame* m_pFrameGrey=NULL;
    uint8_t* m_pFrameBuffer=NULL;
    int m_nVideoStram=-1;
    QString m_strUrl = "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";

    bool m_bState=VIDEO_STOP;

signals:
    void sendToLog(QString);
public slots:
    void videoOpen(QString url);

};

#endif // VIDEO_CORE_H
