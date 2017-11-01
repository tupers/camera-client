#ifndef H264VIDEO_H
#define H264VIDEO_H
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QImage>
#include <QTimer>
#include <QTime>

#ifndef INT64_C
#define INT64_C
#define UINT64_C
#endif
/*Video Status*/
#define VIDEO_RUN   true
#define VIDEO_STOP  false
/**************/

/*Video Control Command*/
#define VIDEO_TERMINATE     -1
//#define VIDEO_          0
#define VIDEO_SHOW_CAMERA   1
#define VIDEO_SHOW_RUN      2
/***********************/

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
}

typedef struct VideoInfoStruct
{
    QTime videotimer;
    int pretime;
    int oncetime;
    int count;
}VideoInfoStr;

class H264Video:public QObject
{
    Q_OBJECT
public:
    explicit H264Video(QObject*parent=0);
    ~H264Video();
    void H264VideoInit();
    bool H264VideoOpenStream();
    void H264VideoPlay(int);
    void setH264VideoUrl(QString u){url=u;}
    void H264VideoFree();
    void changeVideoStatus(){VideoStatus=!VideoStatus;}
    bool isVideoInit(){return InitFlag;}
    bool getVideoStatus(){return VideoStatus;}
public slots:
    void H264VideoOpen(int);
    void H264VideoTimeout();
private:
    bool InitFlag=false;
    int videoStreamIndex;
    AVFormatContext *pAVFormatContext=NULL;
    AVFrame *pAVFrame=NULL;
    AVFrame *pAVFrameYUV=NULL;
    uint8_t *out_buffer=NULL;
    QString url="rtsp://218.204.223.237:554/live/1/66251FC11353191F/e7ooqwcfbqjoo80j.sdp";
    QMutex mutex;
    AVCodecContext *pAVCodecContext=NULL;
    SwsContext * pSwsContext=NULL;
    AVPacket* pAVPacket=NULL;
    AVCodec *pAVCodec=NULL;
    bool VideoStatus=VIDEO_STOP;
    QTimer *StopTimer=NULL;
    int preSatus=0;
    VideoInfoStr videoinfo;
    void InitVideoInfo();
signals:
    void getImage_Camera(QImage);
    void getImage_Run(QImage);
    void getVideoInfo(int,int,int);
    void clearImage();
    void sendToLog(QString);
    void closeVideo();

};

#endif // H264VIDEO_H
