#ifndef VIDEO_BUFQUEUE_H
#define VIDEO_BUFQUEUE_H

#include <QObject>
#include <QQueue>
#include <QVector>
#include <QMutex>

class video_bufQueue : public QObject
{
    Q_OBJECT
public:
    explicit video_bufQueue(QObject *parent = nullptr);
    video_bufQueue(int buf_size,int buf_num,QObject *parent = nullptr);
    ~video_bufQueue(){freeBuf();}
    void init(int buf_size,int buf_num);
    int bufInput(const uchar*data);
    int bufOutput(uchar*data);
    int getBufSize(){return m_nBufSize;}
    int getBufNum(){return m_nBufNum;}
private:
    void freeBuf();
    void allocBuf(int buf_size,int buf_num);

    typedef struct{
        uchar* data;
    }_queueBuffer;

    QQueue<_queueBuffer> m_queUsed;
    QQueue<_queueBuffer> m_queEmpty;
    QVector<_queueBuffer> m_vecBufPool;
    int m_nBufSize;
    int m_nBufNum;
    QMutex m_Mutex;


signals:

public slots:
};

#endif // VIDEO_BUFQUEUE_H
