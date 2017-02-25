#include "dm8127ftp_service.h"

DM8127Ftp_Service::DM8127Ftp_Service(QObject *parent) : QObject(parent)
{
    cmdSocket=new QTcpSocket;
    dataServer=new QTcpServer;
    connect(cmdSocket,SIGNAL(connected()),this,SLOT(loginService()));
    connect(dataServer,SIGNAL(newConnection()),this,SLOT(newConnectionService()));
    dataPort=8011;

}

DM8127Ftp_Service::~DM8127Ftp_Service()
{
    delete cmdSocket;
    delete dataServer;
}

int DM8127Ftp_Service::SendServerMsg(QTcpSocket *ClientSocket, EzFtp_NetMsg *msg, int len, EzFtp_NetMsg *ackMsg, int32_t *acklen)
{
    int32_t ret = MSG_SOK;
    int32_t size;
    EzFtp_NetMsg *tmpMsg;
    QByteArray ackData;

    size = ClientSocket->write((const char *)msg,len);
    if(size != len)
    {
        ret = MSG_NETERROR;

        return ret;
    }

    if(ClientSocket->waitForReadyRead())
    {
        while(ClientSocket->bytesAvailable())
            ackData = ClientSocket->readAll();
        if(ackData.length()>*acklen)
        {

            ret = MSG_BUFOVERFLOW;

            return ret;
        }

        *acklen = ackData.length();
        tmpMsg = (EzFtp_NetMsg *)(ackData.data());

        if((tmpMsg->ackCmd == ACKCMD_SOK)&&(tmpMsg->cmd==msg->cmd))
        {
            memcpy(ackMsg,tmpMsg,*acklen);
        }
        else
        {
            ret = MSG_SFAIL;
        }
    }
    else
    {
        emit sendToLog("tcp socket receive time out!!!");
        qDebug()<<"tcp socket receive time out!!!";
        ret = MSG_OVERTIME;
    }

    return ret;
}

QString DM8127Ftp_Service::getLocalIP(QString name)
{
    QList<QNetworkInterface>list=QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface interfaces,list)
    {
        if(interfaces.humanReadableName()==name)
        {
//        qDebug()<<interfaces.humanReadableName();
        QList<QNetworkAddressEntry>entryList = interfaces.addressEntries();
        foreach(QNetworkAddressEntry entry,entryList)
        {
            if(entry.ip().protocol()==QAbstractSocket::IPv4Protocol)
            return entry.ip().toString();
        }
        }
    }
    return NULL;
}

void DM8127Ftp_Service::connectToHost(QString ip, int port, QString username, QString password)
{
    qDebug()<<cmdSocket->state();
    cmdSocket->abort();
    cmdSocket->connectToHost(ip,port);
    userName=username;
    passWord=password;
    remoteip=ip;
    remoteport=port;
}

void DM8127Ftp_Service::loginService()
{
    qDebug()<<cmdSocket->state();
    if(loginAccountCheck(userName,passWord))
    {
        serverState=Server_Loggedin;
        qDebug()<<"connected. open data server";
        if(!dataServer->listen(QHostAddress::Any,dataPort))
            qDebug()<<"listen failed";
        else
            list();
    }
    else
    {
        serverState=Server_Disconnected;
        qDebug()<<"login error.";
        cmdSocket->abort();
    }
}

bool DM8127Ftp_Service::loginAccountCheck(QString username, QString password)
{
    EzFtp_NetMsg *msg;
    EzFtp_NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    emit sendToLog("Checking account...");
    qDebug()<<"Checking account...";

    /** send  Log information*/
    QByteArray ba;
    if((username.length()>MAX_USERNAME)||(username.length()<MIN_USERNAME))
    {
        emit sendToLog("user name is invalid!!!");
        qDebug()<<"user name is invalid!!!";
        return false;
    }

    if((password.length()>MAX_PASSWD)||(password.length()<MIN_PASSWD))
    {
        emit sendToLog("password is invalid!!!");
        qDebug()<<"password is invalid!!!";
        return false;
    }

    len = sizeof(EzFtp_NetMsg) + sizeof(EZFtp_Account);
    msg = (EzFtp_NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmd = FTP_MSG_LOGIN;
    msg->cmdValueLen = sizeof(EZFtp_Account);
    EZFtp_Account *acount = (EZFtp_Account *)(msg->cmdValue);

    ba = username.toLatin1();
    memcpy(acount->UserName,ba.data(),ba.length());
    ba = password.toLatin1();
    memcpy(acount->Passwd,ba.data(),ba.length());

    acklen = sizeof(EzFtp_NetMsg);
    ackMsg = (EzFtp_NetMsg *)malloc(acklen);
    ret = SendServerMsg(cmdSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog("UserName or Passwd incorrect");
        qDebug()<<"UserName or Passwd incorrect";

        free(msg);
        free(ackMsg);
        return false;
    }

    //    acount = (EZFtp_Account *)(ackMsg->cmdValue);
    //    *authority = acount->authority;

    free(msg);
    free(ackMsg);

    return true;
}

void DM8127Ftp_Service::list(QString path)
{
    currentDir = path;
    EzFtp_NetMsg *msg;
    EzFtp_NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;
    int cmd = FTP_MSG_LIST;
    emit sendToLog(QString("Get list..."));
    qDebug()<<"Get list ...";
    EzFtp_File tempfile;
    tempfile.port=dataPort;

    QString addr = getLocalIP("本地连接");
    strcpy((char*)tempfile.ip,addr.toLatin1().data());

    strcpy(tempfile.filepath,path.toLatin1().data());

    len = sizeof(EzFtp_NetMsg) + sizeof(EzFtp_File);
    msg = (EzFtp_NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmdValueLen=sizeof(EzFtp_File);
    msg->cmd = cmd;
    memcpy(msg->cmdValue,&tempfile,sizeof(EzFtp_File));

    acklen = sizeof(EzFtp_NetMsg);
    ackMsg = (EzFtp_NetMsg *)malloc(acklen);
    ret = SendServerMsg(cmdSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog(QString("Get list failed"));
        qDebug()<<"Get list failed";
        free(msg);
        free(ackMsg);
        return;
    }
    free(msg);
    free(ackMsg);
    return;
}

void DM8127Ftp_Service::get(QString file)
{
    EzFtp_NetMsg *msg;
    EzFtp_NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;
    int cmd = FTP_MSG_GET;
    emit sendToLog(QString("Get file..."));
    qDebug()<<"Get file ...";
    EzFtp_File tempfile;
    tempfile.port=dataPort;

    QString addr = getLocalIP("本地连接");
    strcpy((char*)tempfile.ip,addr.toLatin1().data());

    QString filepath = currentDir+"/"+file;
    strcpy(tempfile.filepath,filepath.toLatin1().data());

    len = sizeof(EzFtp_NetMsg) + sizeof(EzFtp_File);
    msg = (EzFtp_NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmdValueLen=sizeof(EzFtp_File);
    msg->cmd = cmd;
    memcpy(msg->cmdValue,&tempfile,sizeof(EzFtp_File));

    acklen = sizeof(EzFtp_NetMsg);
    ackMsg = (EzFtp_NetMsg *)malloc(acklen);
    ret = SendServerMsg(cmdSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog(QString("Get file failed"));
        qDebug()<<"Get file failed";
        free(msg);
        free(ackMsg);
        return;
    }
    free(msg);
    free(ackMsg);
    return;
}

void DM8127Ftp_Service::put(QString path,QByteArray *data)
{
    sendData=data;
    putFtpFlag=1;

    EzFtp_NetMsg *msg;
    EzFtp_NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;
    int cmd = FTP_MSG_PUT;
    emit sendToLog(QString("Put file..."));
    qDebug()<<"Put file ...";
    EzFtp_File tempfile;
    tempfile.port=dataPort;

    QString addr = getLocalIP("本地连接");
    strcpy((char*)tempfile.ip,addr.toLatin1().data());

    strcpy(tempfile.filepath,path.toLatin1().data());
    tempfile.fileSize=data->length();

    len = sizeof(EzFtp_NetMsg) + sizeof(EzFtp_File);
    msg = (EzFtp_NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmdValueLen=sizeof(EzFtp_File);
    msg->cmd = cmd;
    memcpy(msg->cmdValue,&tempfile,sizeof(EzFtp_File));

    acklen = sizeof(EzFtp_NetMsg);
    ackMsg = (EzFtp_NetMsg *)malloc(acklen);
    ret = SendServerMsg(cmdSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog(QString("Put file failed"));
        qDebug()<<"Put file failed";
        free(msg);
        free(ackMsg);
        return;
    }
    free(msg);
    free(ackMsg);
    return;
}

void DM8127Ftp_Service::newConnectionService()
{
    QTcpSocket *remoteSocket=dataServer->nextPendingConnection();
    if(putFtpFlag==1)
    {
        ftpData* dataRcv = new ftpData(remoteSocket,sendData);
        putFtpFlag=0;
        connect(dataRcv,SIGNAL(dataFinished(int,QByteArray)),this,SLOT(remoteDataRcv(int,QByteArray)));
    }
    else
    {
        ftpData* dataRcv = new ftpData(remoteSocket);
        connect(dataRcv,SIGNAL(dataFinished(int,QByteArray)),this,SLOT(remoteDataRcv(int,QByteArray)));
    }

}

void DM8127Ftp_Service::remoteDataRcv(int cmd, QByteArray rcvData)
{
    switch(cmd)
    {
    case FTP_MSG_LIST:
    {
        FileList *temp= (FileList*)malloc(rcvData.length());
        memcpy(temp,rcvData.data(),rcvData.length());
        emit listInfo(temp);
        break;
    }
    case FTP_MSG_PUT:
    {
        break;
    }
    case FTP_MSG_GET:
    {
        emit getData(rcvData);
        break;
    }
    default:
    {
        break;
    }
    }
}

void DM8127Ftp_Service::close()
{
    dataServer->close();
    cmdSocket->disconnectFromHost();
}
