#include "dm8127scan_service.h"

dm8127scan_service::dm8127scan_service(QObject *parent) : QObject(parent)
{
    BroadcastSocket = new QUdpSocket(this);
    //    udpsocket->bind(port,QUdpSocket::ReuseAddressHint);
    connect(BroadcastSocket,SIGNAL(readyRead()),this,SLOT(dataRecv()));
}

dm8127scan_service::~dm8127scan_service()
{

}

void dm8127scan_service::scan()
{
    emit sendToLog("scan...");
    qDebug()<<"scan...";
    NetCamScanData data;
    memset(&data,0,sizeof(data));
    data.magic[0]=SCAN_MAGIC1;
    data.magic[1]=SCAN_MAGIC2;
    strcpy(data.identical,IDENTICAL_STRING);
    data.CamId=0;
    char* pdata= (char*)&data;
    int num = sizeof(NetCamScanData)-sizeof(data.crc);
    int i;
    for(i=0;i<num;i++)
        data.crc+=*(pdata+i);
//    qDebug()<<data.crc;
    BroadcastSocket->writeDatagram(pdata,sizeof(NetCamScanData),QHostAddress::Broadcast,SCAN_SERVER_PORT);
//    qDebug()<<ret<<sizeof(NetCamScanData);

}

bool dm8127scan_service::check(const NetCamScanData *data)
{
    if((data->magic[0]!=SCAN_MAGIC1)||(data->magic[1]!=SCAN_MAGIC2))
        return false;
    int i;
    int num = sizeof(NetCamScanData)-sizeof(data->crc);
    int crc=0;
    char* pdata = (char*)data;
    for(i=0;i<num;i++)
        crc+=*(pdata+i);
    if(crc!=data->crc)
        return false;
    return true;
}

void dm8127scan_service::dataRecv()
{
    while(BroadcastSocket->hasPendingDatagrams())
    {
        if(BroadcastSocket->pendingDatagramSize()!=sizeof(NetCamScanData))
            continue;
        NetCamScanData data;
        char*pdata = (char*)&data;
        QHostAddress remoteIP;
        BroadcastSocket->readDatagram(pdata,sizeof(NetCamScanData),&remoteIP);

        if(!check(&data))
            continue;
        emit findDevice(data.CamId,QHostAddress(remoteIP.toIPv4Address()).toString());
    }
}
