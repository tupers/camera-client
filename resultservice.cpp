#include "resultservice.h"

resultService::resultService(QObject *parent) : QObject(parent)
{
    resultSocket = new QTcpSocket(this);
    resultSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    connect(resultSocket,SIGNAL(connected()),this,SLOT(connectSuccessfully()));
    connect(resultSocket,SIGNAL(readyRead()),this,SLOT(readData()));
    connect(resultSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(reportError(QAbstractSocket::SocketError)));
    connect(resultSocket,SIGNAL(disconnected()),this,SLOT(disconnectfromServer()));

    retryTimer = new QTimer(this);
    retryTimer->setInterval(5000);
    retryTimer->setSingleShot(false);
    connect(retryTimer,SIGNAL(timeout()),this,SLOT(retryConnection()));

}

void resultService::connectSocket(QString ip, int port)
{
    retryTimer->stop();
    resultSocket->abort();
    isRun = true;
    hostIP = ip;
    hostPort = port;
    resultSocket->connectToHost(hostIP,hostPort);
}

void resultService::readData()
{
    QByteArray ba = resultSocket->readAll();
    emit resultData(ba);
}

void resultService::connectSuccessfully()
{
    retryTimer->stop();
    emit sendToLog(QString("%1 at %2 connected successfully.").arg(hostIP).arg(hostPort));
    qDebug()<<hostIP<<" at "<<hostPort<<" connected successfully.";
    while(resultSocket->socketOption(QAbstractSocket::KeepAliveOption).toInt()!=1)
    {
        resultSocket->setSocketOption(QAbstractSocket::KeepAliveOption,1);
    }
}

void resultService::disconnectfromServer()
{
    emit sendToLog(QString("%1 at %2 connection disconnected.").arg(hostIP).arg(hostPort));
    qDebug()<<hostIP<<" at "<<hostPort<<" connection disconnected.";
    if(isRun==true)
    {
        emit sendToLog("try connect again.");
        qDebug()<<"try connect again.";
        retryTimer->start();
    }
}

void resultService::retryConnection()
{
    connectSocket(hostIP,hostPort);
}
