#include "udpservice.h"

udpservice::~udpservice()
{
    if(databuf!=NULL)
        delete databuf;
}

udpservice::udpservice(QHostAddress ip, int datasize, QObject *parent) : QObject(parent)
{
    udpsocket = new QUdpSocket(this);
    udpsocket->bind(ip,0);
    connect(udpsocket,SIGNAL(readyRead()),this,SLOT(dataRecv()));
    if(datasize==0)
        mode=1;
    else
    {
        mode=0;
        initRecvBuf(datasize);
    }
}

void udpservice::initRecvBuf(int size)
{
    databuf = new uchar[size];
    if(databuf==NULL)
    {
        emit sendToLog("malloc databuf failed");
        qDebug()<<"malloc databuf failed.";
    }
    memset(databuf,0,size);
}

void udpservice::dataRecv()
{
//    qDebug()<<"udprcv thread id: "<<QThread::currentThreadId();
    if(mode==0)
    {
    while(udpsocket->hasPendingDatagrams())
    {
        if((udpsocket->pendingDatagramSize()==sizeof(EzFrameInfo))&&(dataheadflag==0))
        {
            dataheadflag=1;
            QByteArray datagram;
            EzFrameInfo datainfo;
            datagram.resize(udpsocket->pendingDatagramSize());
            udpsocket->readDatagram(datagram.data(),datagram.size(),&hostaddr,&hostport);
            memcpy(&datainfo,datagram.data(),sizeof(EzFrameInfo));
            pdatabuf=databuf;
            recvsize=0;
            datasize=datainfo.height*datainfo.pitch;
            dataheight=datainfo.height;
            datawidth=datainfo.width;
            datapitch=datainfo.pitch;
            int check=datagram.size();
            udpsocket->writeDatagram((char*)&check,sizeof(int),hostaddr,hostport);
        }
        else
        {
            QByteArray datagram;
            datagram.resize(udpsocket->pendingDatagramSize());
            udpsocket->readDatagram(datagram.data(),datagram.size(),&hostaddr,&hostport);
            memcpy(pdatabuf,datagram.data(),datagram.size());
            recvsize+=datagram.size();
            pdatabuf+=datagram.size();
            int check=datagram.size();
            udpsocket->writeDatagram((char*)&check,sizeof(int),hostaddr,hostport);
        }
    }
    if(recvsize>=datasize)
    {
        dataheadflag=0;
        emit sendToLog("receive finished.");
        qDebug()<<"receive finished.";
        emit recvDataInfo(databuf,datawidth,dataheight,datapitch);
    }
    }
    else
    {
        while(udpsocket->hasPendingDatagrams())
        {
            QByteArray datagram;
            datagram.resize(udpsocket->pendingDatagramSize());
            udpsocket->readDatagram(datagram.data(),datagram.size(),&hostaddr,&hostport);
            emit recvDataInfo(datagram);
        }
    }
}
