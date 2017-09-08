#include "ftpdata.h"

ftpData::ftpData(QTcpSocket *socket, QByteArray *data, QObject *parent) : QObject(parent)
{
    dataSocket=socket;
    sendData=data;
    connect(dataSocket,SIGNAL(readyRead()),this,SLOT(readData()));
    connect(dataSocket,SIGNAL(disconnected()),this,SLOT(diconnectservice()));
    connect(dataSocket,&QTcpSocket::bytesWritten,this,[=](qint64 num){
        if(m_nPutSize>0)
        {
            m_nPutSize-=num;
            emit putProgress(m_nPutSize);
        }
    });

    rcvData.resize(0);
}

void ftpData::readData()
{
    QByteArray data;
    EzFtp_Ack ack;
    ack.status=FTP_ACK_SOK;
    while(dataSocket->bytesAvailable())
        data=dataSocket->readAll();
    ack.len=data.length();
    if(data.length()==sizeof(EzFtp_Header))
    {
        EzFtp_Header temp;
        memcpy(&temp,data.data(),data.length());

        if(temp.magic1==MAGIC_HEADER1&&temp.magic2==MAGIC_HEADER2)
        {
            cmd=temp.cmd;
            if(cmd==FTP_MSG_PUT)
            {
                if(temp.status==0)
                {
                    //qDebug()<<sendData->length();
                    dataSocket->write(sendData->data(),sendData->length());
                    m_nPutSize = sendData->length();
                    qDebug()<<"data write";
                }
                else
                {
                    qDebug()<<"put file error";
                    return;
                }
            }
            else
            {
            if(temp.pad==0)
                size=temp.len;
            else
                size=temp.len-1;
            dataSocket->write((const char*)&ack,sizeof(EzFtp_Ack));
            }
        }
        else
        {
            qDebug()<<"check data failed.";
            ack.status=FTP_ACK_SFAIL;
            dataSocket->write((const char*)&ack,sizeof(EzFtp_Ack));
        }

    }
    else
    {
        if(size==0)
        {
            qDebug()<<"unknown data";
            ack.status=FTP_ACK_SFAIL;
            dataSocket->write((const char*)&ack,sizeof(EzFtp_Ack));
            return;
        }

        rcvData.append(data.data(),data.length());
        currentsize+=data.length();

        dataSocket->write((const char*)&ack,sizeof(EzFtp_Ack));
        if(currentsize>=size)
        {
            qDebug()<<"data finished";
            emit dataFinished(cmd,rcvData);
        }
    }
}

void ftpData::diconnectservice()
{
    qDebug()<<"remote socket disconnected.";
    if(sendData!=NULL)
    {
        delete sendData;
        sendData=NULL;
    }
    dataSocket->abort();
    dataSocket->deleteLater();
    this->deleteLater();
}
