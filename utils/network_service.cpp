#include "network_service.h"

/**
 * @brief network_service::network_service
 * @param parent
 */
network_service::network_service(QObject *parent) : QObject(parent)
{

}

/**
 * @brief network_service::SendMsg
 * @param socket
 * @param msg
 * @param len
 * @param ackMsg
 * @param acklen
 * @return
 */
int network_service::SendMsg(QTcpSocket *socket, netMsg *msg, int len, netMsg *ackMsg, int *acklen)
{
    int ret = MSG_SOK;
    int size;
    netMsg* tmpMsg;
    QByteArray ackData;

    size = socket->write((const char*)msg,len);
    if(size != len)
    {
        ret = MSG_NETERROR;
        return ret;
    }

    if(socket->waitForReadyRead(MSG_TIMEOUT))
    {
        while(socket->bytesAvailable())
            ackData = socket->readAll();
        if(ackData.length()!=*acklen)
        {
            ret = MSG_SFAIL;
            return ret;
        }

        *acklen = ackData.length();
        tmpMsg = (netMsg*)(ackData.data());

        if(tmpMsg->ackCmd==ACKCMD_SOK&&tmpMsg->cmd==msg->cmd)
            memcpy(ackMsg,tmpMsg,*acklen);
        else
            ret = MSG_SFAIL;
    }
    else
    {
        emit sendToLog("tcp socket receive time out!!!");
        ret = MSG_OVERTIME;
    }

    return ret;
}

/**
 * @brief network_service::getLocalIP
 * @return
 */
QString network_service::getLocalIP()
{
    QString ip = "";
    QString localHost = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHost);
    foreach (QHostAddress address, info.addresses()) {
        if(address.protocol()==QAbstractSocket::IPv4Protocol)
            ip=address.toString();
    }
    return ip;
}
