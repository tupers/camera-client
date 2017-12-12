#include "video_bufqueue.h"

video_bufQueue::video_bufQueue(QObject *parent) : QObject(parent)
{
    m_queEmpty.clear();
    m_queUsed.clear();
    m_vecBufPool.clear();
}

video_bufQueue::video_bufQueue(int buf_size, int buf_num, QObject *parent)
{
    m_queEmpty.clear();
    m_queUsed.clear();
    m_vecBufPool.clear();
    init(buf_size,buf_num);
}

void video_bufQueue::allocBuf(int buf_size, int buf_num)
{
    for(int i=0;i<buf_num;i++)
    {
        _queueBuffer buf;
        buf.data = new uchar[buf_size];
        m_vecBufPool.append(buf);
    }
}

void video_bufQueue::init(int buf_size,int buf_num)
{
    freeBuf();
    allocBuf(buf_size,buf_num);
    for(int i=0;i<buf_num;i++)
        m_queEmpty.enqueue(m_vecBufPool[i]);
    m_nBufNum = buf_num;
    m_nBufSize = buf_size;
}

int video_bufQueue::bufInput(const uchar *data)
{
    m_Mutex.lock();
    if(m_queEmpty.isEmpty())
    {
        m_Mutex.unlock();
        return 0;
    }
    _queueBuffer buf = m_queEmpty.dequeue();
    memcpy(buf.data,data,m_nBufSize);

    m_queUsed.enqueue(buf);
    m_Mutex.unlock();
    return m_queUsed.count();
}

int video_bufQueue::bufOutput(uchar *data)
{
    m_Mutex.lock();
    if(m_queUsed.isEmpty())
    {
        m_Mutex.unlock();
        return 0;
    }
    _queueBuffer buf = m_queUsed.dequeue();
    memcpy(data,buf.data,m_nBufSize);

    m_queEmpty.enqueue(buf);
    m_Mutex.unlock();
    return m_queEmpty.count();
}

void video_bufQueue::freeBuf()
{
    m_queEmpty.clear();
    m_queUsed.clear();
    for(_queueBuffer buf:m_vecBufPool)
        delete buf.data;
    m_vecBufPool.clear();
}
