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
    data.cmd = SCAN_CMD_GET;
    //setup random check num
    srand((unsigned)time(NULL));
    m_nRandom = rand();
    data.random = m_nRandom;
    char* pdata= (char*)&data;
    BroadcastSocket->writeDatagram(pdata,sizeof(NetCamScanData),QHostAddress::Broadcast,SCAN_SERVER_PORT);
}

bool dm8127scan_service::check(const NetCamScanData *data)
{
    if((data->magic[0]!=SCAN_MAGIC1)||(data->magic[1]!=SCAN_MAGIC2)||data->cmd!=SCAN_CMD_PUT||m_nRandom!=data->random)
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
        emit findDevice(1,QHostAddress(remoteIP.toIPv4Address()).toString());
    }
}
