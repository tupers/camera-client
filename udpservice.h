#ifndef UDPSERVICE_H
#define UDPSERVICE_H

#include <QObject>
#include <QtNetwork>
#include <ezstream.h>

#define DATAARRAY 0;
#define DEBUGMSG  1;


class udpservice : public QObject
{
    Q_OBJECT
public:
    ~udpservice();
    explicit udpservice(int port = 8000,int datasize = 0,QObject *parent = 0);
    void initRecvBuf(int size);

private:
    QUdpSocket *udpsocket;
    QHostAddress hostaddr;
    quint16 hostport;
    unsigned char*databuf=NULL;
    unsigned char*pdatabuf=NULL;
    int datasize=0;
    int recvsize=0;
    int dataheadflag=0;
    int datawidth=0;
    int dataheight=0;
    int datapitch=0;
    int mode=0;
signals: 
    void sendToLog(QString);
    void recvDataInfo(uchar*data,int width,int height,int pitch);
    void recvDataInfo(QByteArray data);
public slots:
    void dataRecv();
};

#endif // UDPSERVICE_H
