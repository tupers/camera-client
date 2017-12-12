#include "network.h"


NetWork::NetWork(QObject *parent) : QObject(parent)
{
    ClientSocket = new QTcpSocket(this);
    LogTimer = new QTimer(this);
    LogTimer->setInterval(10000);
    LogTimer->setSingleShot(true);
    LogTimer->setObjectName("Timer");
    ClientSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    algResult = new resultService(this);
    connect(algResult,SIGNAL(resultData(QByteArray)),this,SIGNAL(sendAlgRsult(QByteArray)));
    connect(algResult,SIGNAL(sendToLog(QString)),this,SIGNAL(sendToLog(QString)));

    //     qDebug()<<"network thread id: "<<QThread::currentThreadId();
    QHostAddress localIP;
    localIP.setAddress(getLocalIP("本机连接"));

    /* ftp service */
    m_hFtpThread = new QThread;
    m_hFtp = new ftp_service;
    connect(m_hFtp,&ftp_service::sendToLog,this,[=](QString log){
        qDebug()<<log;
        emit sendToLog(log);
    });
    m_hFtp->moveToThread(m_hFtpThread);
    m_hFtpThread->start();
    connect(this,&NetWork::ftp_close,m_hFtp,&ftp_service::closeFtp);
    connect(this,&NetWork::ftp_put,m_hFtp,&ftp_service::put);
    connect(this,&NetWork::ftp_del,m_hFtp,&ftp_service::del);
    connect(this,&NetWork::ftp_get,m_hFtp,&ftp_service::get);
    connect(this,static_cast<void (NetWork::*)()>(&NetWork::ftp_list),m_hFtp,static_cast<bool(ftp_service::*)()>(&ftp_service::list));
    connect(this,static_cast<void(NetWork::*)(QString)>(&NetWork::ftp_list),m_hFtp,static_cast<bool(ftp_service::*)(QString)>(&ftp_service::list));

    connect(m_hFtp,&ftp_service::receiveFileList,this,&NetWork::ftp_receiveFileList);
    connect(m_hFtp,&ftp_service::receiveData,this,&NetWork::ftp_receiveData);
    connect(this,&NetWork::ftp_login,m_hFtp,&ftp_service::loginFtp);

    udpFrameRcv = new udpservice(localIP,1920*1080);
    udpThread = new QThread;
    udpFrameRcv->moveToThread(udpThread);
    connect(udpFrameRcv,SIGNAL(sendToLog(QString)),this,SIGNAL(sendToLog(QString)));
    connect(udpFrameRcv,SIGNAL(recvDataInfo(uchar*,int,int,int)),this,SIGNAL(udpRcvDataInfo(uchar*,int,int,int)));
    udpThread->start();

    udpDebugRcv = new udpservice(localIP);
    udpDebugThread = new QThread;
    udpDebugRcv->moveToThread(udpDebugThread);
    connect(udpDebugRcv,SIGNAL(sendToLog(QString)),this,SIGNAL(sendToLog(QString)));
    connect(udpDebugRcv,SIGNAL(recvDataInfo(QByteArray)),this,SIGNAL(udpRcvDataInfo(QByteArray)));
    udpDebugThread->start();

    cameraSearch = new dm8127scan_service;
    cameraSearchThread = new QThread;
    cameraSearch->moveToThread(cameraSearchThread);
    connect(cameraSearch,SIGNAL(sendToLog(QString)),this,SIGNAL(sendToLog(QString)));
    connect(this,SIGNAL(cameraScan()),cameraSearch,SLOT(scan()));
    connect(cameraSearch,SIGNAL(findDevice(int,QString)),this,SIGNAL(cameraDevice(int,QString)));
    cameraSearchThread->start();

    connect(ClientSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(reportError(QAbstractSocket::SocketError)));
    connect(ClientSocket,SIGNAL(disconnected()),this,SIGNAL(disconnectfromServer()));
    connect(ClientSocket,SIGNAL(connected()),this,SLOT(LoginService()));
    connect(LogTimer,SIGNAL(timeout()),this,SLOT(LoginError()));
    QFileInfo ftemp(networkconfigpath);
    if(ftemp.isFile())
    {
        emit sendToLog("find system network.ini");
        qDebug()<<"find system network.ini";
    }
    else
    {
        emit sendToLog("create new system network.ini");
        qDebug()<<"create new system network.ini";
        networkconfig = new QSettings(networkconfigpath,QSettings::IniFormat);
        saveNetwork();
        delete networkconfig;
        networkconfig=NULL;
    }

    if(networkconfig!=NULL)
    {
        delete networkconfig;
        networkconfig=NULL;
    }
    networkconfig = new QSettings(networkconfigpath,QSettings::IniFormat);
    loadNetwork();
    delete networkconfig;
    networkconfig=NULL;

}

NetWork::~NetWork()
{
    if(m_hFtp!=NULL)
    {
        delete m_hFtp;
        m_hFtp=NULL;
    }
    if(m_hFtpThread!=NULL)
    {
        m_hFtpThread->quit();
        m_hFtpThread->deleteLater();
    }
    if(LogTimer!=NULL)
    {
        delete LogTimer;
        LogTimer=NULL;
    }
    if(ClientSocket!=NULL)
    {
        delete ClientSocket;
        ClientSocket=NULL;
    }
    if(udpFrameRcv!=NULL)
    {
        delete udpFrameRcv;
        udpFrameRcv=NULL;
    }
    if(udpThread!=NULL)
    {
        delete udpThread;
        udpThread=NULL;
    }
    if(udpDebugRcv!=NULL)
    {
        delete udpDebugRcv;
        udpDebugRcv=NULL;
    }
    if(udpDebugThread!=NULL)
    {
        delete udpDebugThread;
        udpDebugThread=NULL;
    }
    if(cameraSearch!=NULL)
    {
        delete cameraSearch;
        cameraSearch=NULL;
    }
    if(cameraSearchThread!=NULL)
    {
        delete cameraSearchThread;
        cameraSearchThread=NULL;
    }
}

int NetWork::SendServerMsg(QTcpSocket *ClientSocket, NetMsg *msg, int len, NetMsg *ackMsg, int32_t *acklen)
{
    int32_t ret = MSG_SOK;
    int32_t size;
    NetMsg *tmpMsg;
    QByteArray ackData;

    size = ClientSocket->write((const char *)msg,len);
    if(size != len)
    {
        ret = MSG_NETERROR;

        return ret;
    }

    if(ClientSocket->waitForReadyRead(2000))
    {
        while(ClientSocket->bytesAvailable())
            ackData = ClientSocket->readAll();
        //qDebug()<<ackData.length();
        if(ackData.length()!=*acklen)
        {

            ret = MSG_SFAIL;

            return ret;
        }

        *acklen = ackData.length();
        tmpMsg = (NetMsg *)(ackData.data());

        if((tmpMsg->ackCmd == ACKCMD_SOK)&&(tmpMsg->cmd==msg->cmd))
            memcpy(ackMsg,tmpMsg,*acklen);
        else
            ret = MSG_SFAIL;
    }
    else
    {
        emit sendToLog("tcp socket receive time out!!!");
        qDebug()<<"tcp socket receive time out!!!";
        ret = MSG_OVERTIME;
    }

    return ret;
}

void NetWork::Login(QString username,QString password)
{
    ClientSocket->abort();


    ClientSocket->connectToHost(network.value.ports_ipaddress,EZ_SERVERPORT);
    emit sendToLog("Connecting server...");
    qDebug()<<"Connecting server...";
    LogUserName=username;
    LogPassWd=password;
    LogTimer->start();
}

void NetWork::LoginError()
{
    if(sender()->objectName()=="Timer")
    {
        ClientSocket->abort();
        emit sendToLog("login timeout, can not connect server.");
        qDebug()<<"login timeout, can not connect server.";
    }
    else
    {
        ClientSocket->disconnectFromHost();
        emit sendToLog("Account Error.");
        qDebug()<<"Account Error.";
    }
    emit loginFailed();
}

void NetWork::LoginService()
{

    LogTimer->stop();
    emit sendToLog("Connected successfully");
    qDebug()<<"Connected successfully";
    //
    //校对账户信息
    //
    if(LoginCheckAccount(&LogAuthority))
    {
        emit sendToLog("Account confirmed.");
        qDebug()<<"Account confirmed.";
        while(ClientSocket->socketOption(QAbstractSocket::KeepAliveOption).toInt()!=1)
        {
            ClientSocket->setSocketOption(QAbstractSocket::KeepAliveOption,1);
        }
        emit loginSuccess(LogAuthority);

    }
    else
        LoginError();
}

void NetWork::sendConfigToServer(_NET_MSG cmd , unsigned char val)
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    emit sendToLog(QString("Send cmd:%1...,set val is %2").arg(cmd).arg(val));
    qDebug()<<"Send cmd: "<<cmd<<"..., set val is "<<val;

    len = sizeof(NetMsg) + sizeof(unsigned char);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmdValueLen=sizeof(unsigned char);
    msg->cmd = cmd;
    memcpy(msg->cmdValue,&val,sizeof(unsigned char));

    acklen = sizeof(NetMsg);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog(QString("Send cmd:%1 failed").arg(cmd));
        qDebug()<<"Send cmd: "<<cmd<<" failed";

        free(msg);
        free(ackMsg);
        return;
    }
    free(msg);
    free(ackMsg);

}

void NetWork::sendConfigToServer(_NET_MSG cmd, unsigned char val1, unsigned char val2)
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    emit sendToLog(QString("Send cmd:%1...,set first val is %2, second val is %3").arg(cmd).arg(val1).arg(val2));
    qDebug()<<"Send cmd: "<<cmd<<"..., set first val is "<<val1<<", second val is "<<val2;
    unsigned short gainval = (val1<<8)|val2;
    len = sizeof(NetMsg) + sizeof(unsigned short);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmdValueLen=sizeof(unsigned short);
    msg->cmd = cmd;
    memcpy(msg->cmdValue,&gainval,sizeof(unsigned short));

    acklen = sizeof(NetMsg);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog(QString("Send cmd:%1 failed").arg(cmd));
        qDebug()<<"Send cmd: "<<cmd<<" failed";

        free(msg);
        free(ackMsg);
        return;
    }
    free(msg);
    free(ackMsg);
}

void NetWork::sendConfigToServer(_NET_MSG cmd, void *src, int size)
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    emit sendToLog(QString("Send cmd:%1...").arg(cmd));
    qDebug()<<"Send cmd: "<<cmd<<"...";

    len = sizeof(NetMsg) + size;
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmdValueLen=size;
    msg->cmd = cmd;
    memcpy(msg->cmdValue,src,size);

    acklen = sizeof(NetMsg);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog(QString("Send cmd:%1 failed").arg(cmd));
        qDebug()<<"Send cmd: "<<cmd<<" failed";

        free(msg);
        free(ackMsg);
        return;
    }
    free(msg);
    free(ackMsg);
}

int NetWork::getConfigFromSever(_NET_MSG cmd, void *dst, int size)
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    emit sendToLog(QString("Send cmd:%1...").arg(cmd));
    qDebug()<<"Send cmd: "<<cmd<<"...";

    len = sizeof(NetMsg);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmdValueLen=0;
    msg->cmd = cmd;
    // memcpy(msg->cmdValue,&val,sizeof(unsigned char));

    acklen = sizeof(NetMsg)+sizeof(unsigned char)*size;
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog(QString("Send cmd:%1 failed").arg(cmd));
        qDebug()<<"Send cmd: "<<cmd<<" failed";

        free(msg);
        free(ackMsg);
        return -1;
    }
    memcpy(dst,ackMsg->cmdValue,sizeof(unsigned char)*size);
    free(msg);
    free(ackMsg);
    return 0;
}

void NetWork::sendConfigToServerEntire(QVariant configentire)
{           
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    emit sendToLog("Send entire config to server...");
    qDebug()<<"Send entire config to server...";
    ConfigStr tempconfig;
    tempconfig = configentire.value<ConfigStruct>();
    SysInfo info;

    configClienttoServer(&info,&tempconfig);
    len = sizeof(NetMsg) + sizeof(SysInfo);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmdValueLen=sizeof(SysInfo);
    msg->cmd = NET_MSG_SETSYSINFO;
    memcpy(msg->cmdValue,&info,sizeof(SysInfo));

    acklen = sizeof(NetMsg);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog("Send entire config to server failed");
        qDebug()<<"Send entire config to server failed";

        free(msg);
        free(ackMsg);
        return;
    }
    free(msg);
    free(ackMsg);

}

bool NetWork::sendConfigToServerEntireForBoot(QVariant configentire)
{

    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    ConfigStr tempconfig;
    tempconfig = configentire.value<ConfigStruct>();
    SysInfo info;

    configClienttoServer(&info,&tempconfig);

    emit sendToLog("Set Default boot config...");
    qDebug()<<"Set Default boot config...";

    len = sizeof(NetMsg) + sizeof(SysInfo);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmdValueLen=sizeof(SysInfo);
    msg->cmd = NET_MSG_UPDATE_DEFAULTPARAM;
    memcpy(msg->cmdValue,&info,sizeof(SysInfo));

    acklen = sizeof(NetMsg);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog("Set Default boot config failed");
        qDebug()<<"Set Default boot config failed";

        free(msg);
        free(ackMsg);
        return false;
    }
    free(msg);
    free(ackMsg);

    return true;

}

bool NetWork::sendConfigToServerALG(QVariant algparam)
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    EzsdsAlgParam tempconfig;
    tempconfig = algparam.value<EzsdsAlgParam>();

    emit sendToLog("Set alg param...");
    qDebug()<<"Set alg param...";

    len = sizeof(NetMsg) + sizeof(EzsdsAlgParam);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmdValueLen=sizeof(EzsdsAlgParam);
    msg->cmd = NET_MSG_IMGALG_SET_PARAM;
    memcpy(msg->cmdValue,&tempconfig,sizeof(EzsdsAlgParam));

    acklen = sizeof(NetMsg);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog("Set alg param failed");
        qDebug()<<"Set alg param failed";

        free(msg);
        free(ackMsg);
        return false;
    }
    free(msg);
    free(ackMsg);

    return true;
}

bool NetWork::sendConfigToServerH3A(QVariant h3aparam)
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    EzCamH3AParam tempconfig;
    tempconfig = h3aparam.value<EzCamH3AParam>();

    emit sendToLog("Set h3a param...");
    qDebug()<<"Set h3a param...";

    len = sizeof(NetMsg) + sizeof(EzCamH3AParam);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmdValueLen=sizeof(EzCamH3AParam);
    msg->cmd = NET_MSG_SET_2A_PARAM;
    memcpy(msg->cmdValue,&tempconfig,sizeof(EzCamH3AParam));

    acklen = sizeof(NetMsg);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog("Set h3a param failed");
        qDebug()<<"Set h3a param failed";

        free(msg);
        free(ackMsg);
        return false;
    }
    free(msg);
    free(ackMsg);

    return true;
}

void NetWork::sendConfigToServerLightConfig(QVariant data)
{
    CalibrateValueStr temp;
    temp = data.value<CalibrateValueStr>();
    FillLightConfig lightconfig[2];
    lightconfig[0].enable = temp.light_config[0].enable;
    lightconfig[0].pwmduty = temp.light_config[0].pwmduty;
    lightconfig[1].enable = temp.light_config[1].enable;
    lightconfig[1].enable = temp.light_config[1].pwmduty;
    sendConfigToServer(NET_MSG_SET_LED,lightconfig,sizeof(lightconfig));
}

void NetWork::setSocketDebugMode()
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;
    emit sendToLog("Set socket debug mode...");
    qDebug()<<"Set socket debug mode...";
    DebugParams debuginfo;
    debuginfo.port = udpDebugRcv->socket()->localPort();

    QString addr = getLocalIP("本地连接");
        if(addr==NULL)
            addr = getLocalIP("以太网");
    strcpy((char*)debuginfo.ip,addr.toLatin1().data());

    len = sizeof(NetMsg) + sizeof(DebugParams);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmdValueLen=sizeof(DebugParams);
    msg->cmd = NET_MSG_DEBUG_SOCKET;
    memcpy(msg->cmdValue,&debuginfo,sizeof(DebugParams));
    acklen = sizeof(NetMsg);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog("Set socket debug mode failed");
        qDebug()<<"Set socket debug mode failed";
        free(msg);
        free(ackMsg);
        return;
    }
    free(msg);
    free(ackMsg);
}

void NetWork::sendNetworkConfig()
{

    //1.send to server
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    SysInfo info;
    networkconfigClienttoServer(&info,&network);

    emit sendToLog("Send network config...");
    qDebug()<<"Send network config...";

    len = sizeof(NetMsg) + sizeof(Network_Config);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmdValueLen=sizeof(Network_Config);
    msg->cmd = NET_MSG_NET_UPDATE;
    memcpy(msg->cmdValue,&info.net,sizeof(Network_Config));

    acklen = sizeof(NetMsg);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog("Send network config failed.");
        qDebug()<<"Send network config failed.";

        free(msg);
        free(ackMsg);
        return;
    }
    free(msg);
    free(ackMsg);
    //2.save network config
    if(networkconfig!=NULL)
    {
        delete networkconfig;
        networkconfig=NULL;
    }
    networkconfig = new QSettings(networkconfigpath,QSettings::IniFormat);
    saveNetwork();
    delete networkconfig;
    networkconfig=NULL;

}

bool NetWork::LoginCheckAccount(uint8_t *authority)
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    emit sendToLog("Checking account...");
    qDebug()<<"Checking account...";

    /** send  Log information*/
    QByteArray ba;

    if((LogUserName.length()>USER_LEN)||(LogUserName.length()<MIN_USER_LEN))
    {
        emit sendToLog("user name is invalid!!!");
        qDebug()<<"user name is invalid!!!";
        return false;
    }

    if((LogPassWd.length()>PASSWORD_LEN)||(LogPassWd.length()<MIN_PASSWORD_LEN))
    {
        emit sendToLog("password is invalid!!!");
        qDebug()<<"password is invalid!!!";
        return false;
    }

    len = sizeof(NetMsg) + sizeof(Acount_t);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmd = NET_MSG_LOGIN;
    msg->cmdValueLen = sizeof(Acount_t);
    Acount_t *acount = (Acount_t *)(msg->cmdValue);

    ba = LogUserName.toLatin1();
    memcpy(acount->user,ba.data(),ba.length());
    ba = LogPassWd.toLatin1();
    memcpy(acount->password,ba.data(),ba.length());

    acklen = sizeof(NetMsg) + sizeof(Acount_t);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog("UserName or Passwd incorrect");
        qDebug()<<"UserName or Passwd incorrect";

        free(msg);
        free(ackMsg);
        return false;
    }

    acount = (Acount_t *)(ackMsg->cmdValue);
    *authority = acount->authority;

    free(msg);
    free(ackMsg);

    return true;
}

bool NetWork::GetSysinfo(ConfigStr* pstr)
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;
    EzSysInfo info;

    emit sendToLog("get system info...");
    qDebug()<<"get system info...";
    len = sizeof(NetMsg);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmd = NET_MSG_GETSYSINFO;

    acklen = sizeof(NetMsg) + sizeof(EzSysInfo);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog("get system info fail");
        qDebug()<<"get system info fail";

        free(msg);
        free(ackMsg);
        return false;
    }

    memcpy(&info,ackMsg->cmdValue,sizeof(EzSysInfo));
    configServertoClient(pstr,&info);
    networkconfigServertoClinet(&network,&info);

    if(networkconfig!=NULL)
    {
        delete networkconfig;
        networkconfig=NULL;
    }
    networkconfig = new QSettings(networkconfigpath,QSettings::IniFormat);
    saveNetwork();
    delete networkconfig;
    networkconfig=NULL;

    free(msg);
    free(ackMsg);

    return true;
}

//bool NetWork::GetAlgresult(EzAlgResult *algresult)
//{
//    NetMsg *msg;
//    NetMsg *ackMsg;
//    int32_t ret;
//    int32_t acklen;
//    int32_t len;

//    len = sizeof(NetMsg);
//    msg = (NetMsg *)malloc(len);
//    memset(msg,0,len);
//    msg->cmd = NET_MSG_GET_ALG_RESULT;

//    acklen = sizeof(NetMsg) + sizeof(EzAlgResult);
//    ackMsg = (NetMsg *)malloc(acklen);
//    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
//    if(ret !=MSG_SOK)
//    {
//        emit sendToLog("get ALG result failed");
//        qDebug()<<"get ALG result failed";

//        free(msg);
//        free(ackMsg);
//        return false;
//    }
//    memcpy(algresult,ackMsg->cmdValue,sizeof(EzAlgResult));

//    free(msg);
//    free(ackMsg);

//    return true;
//}

bool NetWork::GetCpuloadresult(EzcpuDynamicParam *cpuload)
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;
    len = sizeof(NetMsg);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmd = NET_MSG_GET_PAYLOAD;

    acklen = sizeof(NetMsg) + sizeof(EzcpuDynamicParam);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret !=MSG_SOK)
    {
        emit sendToLog("get CPU load failed");
        qDebug()<<"get CPU load failed";

        free(msg);
        free(ackMsg);
        return false;
    }
    memcpy(cpuload,ackMsg->cmdValue,sizeof(EzcpuDynamicParam));

    free(msg);
    free(ackMsg);

    return true;
}

void NetWork::resetPayload()
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    emit sendToLog("reset payload");
    qDebug()<<"reset payload";

    len = sizeof(NetMsg);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmd = NET_MSG_RESET_PAYLOAD;

    acklen =sizeof(NetMsg);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret !=MSG_SOK)
    {
        emit sendToLog("Reset payload failed");
        qDebug()<<"Reset payload failed";

        free(msg);
        free(ackMsg);
        return;
    }
    free(msg);
    free(ackMsg);

    return;
}

void NetWork::startPayload()
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    emit sendToLog("start payload");
    qDebug()<<"start payload";

    len = sizeof(NetMsg);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmd = NET_MSG_START_PAYLOAD;

    acklen =sizeof(NetMsg);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret !=MSG_SOK)
    {
        emit sendToLog("Start payload failed");
        qDebug()<<"Start payload failed";

        free(msg);
        free(ackMsg);
        return;
    }
    free(msg);
    free(ackMsg);

    return;
}

void NetWork::printPayload()
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    emit sendToLog("print payload");
    qDebug()<<"ptint payload";

    len = sizeof(NetMsg);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmd = NET_MSG_PRINT_PAYLOAD;

    acklen =sizeof(NetMsg);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret !=MSG_SOK)
    {
        emit sendToLog("Print payload failed");
        qDebug()<<"Print payload failed";

        free(msg);
        free(ackMsg);
        return;
    }
    free(msg);
    free(ackMsg);

    return;
}


bool NetWork::GetUser(get_userdata_t *userinfo)
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    emit sendToLog("get user info...");
    qDebug()<<"get user info...";

    len = sizeof(NetMsg);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmd = NET_MSG_GETUSER;

    acklen = sizeof(NetMsg) + sizeof(get_userdata_t);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog("get user info fail");
        qDebug()<<"get user info fail";

        free(msg);
        free(ackMsg);
        return false;
    }

    memcpy(userinfo,ackMsg->cmdValue,sizeof(get_userdata_t));

    free(msg);
    free(ackMsg);

    return true;
}

bool NetWork::GetParams(_NET_MSG cmd , void *src , int size)
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    emit sendToLog(QString("get cmd:%1 Params ...").arg(cmd));
    qDebug()<<QString("get cmd:%1 Params ...").arg(cmd);

    len = sizeof(NetMsg);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmd = cmd;
    acklen = sizeof(NetMsg) + size;
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog(QString("get cmd:%1 Params fail").arg(cmd));
        qDebug()<<QString("get cmd:%1 Params fail, ret: %2").arg(cmd).arg(ret);

        free(msg);
        free(ackMsg);
        return false;
    }

    memcpy(src,ackMsg->cmdValue,size);

    free(msg);
    free(ackMsg);

    return true;
}

bool NetWork::AddUser(QString username,QString password, int authority)
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    emit sendToLog("Add New User");
    qDebug()<<"Add New User";


    QByteArray ba;


    if((username.length()>USER_LEN)||(username.length()<MIN_USER_LEN))
    {
        emit sendToLog("user name is invalid!!!");
        qDebug()<<"user name is invalid!!!";
        return -1;
    }

    if((password.length()>PASSWORD_LEN)||(password.length()<MIN_PASSWORD_LEN))
    {
        emit sendToLog("password is invalid!!!");
        qDebug()<<"password is invalid!!!";
        return -1;
    }

    len = sizeof(NetMsg) + sizeof(Acount_t);
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmd = NET_MSG_ADDUSER;
    msg->cmdValueLen = sizeof(Acount_t);
    Acount_t *acount = (Acount_t *)(msg->cmdValue);

    ba = username.toLatin1();
    memcpy(acount->user,ba.data(),ba.length());
    ba = password.toLatin1();
    memcpy(acount->password,ba.data(),ba.length());
    acount->authority=(__u8)authority;
    acklen =sizeof(NetMsg);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret !=MSG_SOK)
    {
        emit sendToLog("Add User failed");
        qDebug()<<"Add User failed";

        free(msg);
        free(ackMsg);
        return false;
    }
    free(msg);
    free(ackMsg);

    emit AccountUpdate();

    return true;
}

bool NetWork::DeleteUser(QString username)
{
    NetMsg *msg;
    NetMsg *ackMsg;
    int32_t ret;
    int32_t acklen;
    int32_t len;

    emit sendToLog("Delete User");
    qDebug()<<"Delete User";


    QByteArray ba;

    ba = username.toLatin1();
    if((ba.length()>USER_LEN)||(ba.length()<MIN_USER_LEN))
    {
        emit sendToLog("user name is invalid!!!");
        qDebug()<<"user name is invalid!!!";
        return -1;
    }
    len = sizeof(NetMsg) + ba.length();
    msg = (NetMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmd = NET_MSG_DELUSER;
    msg->cmdValueLen = ba.length();
    memcpy(msg->cmdValue,ba.data(),ba.length());
    acklen =sizeof(NetMsg);
    ackMsg = (NetMsg *)malloc(acklen);
    ret = SendServerMsg(ClientSocket,msg,len,ackMsg,&acklen);
    if(ret !=MSG_SOK)
    {
        emit sendToLog("Delete User failed");
        qDebug()<<"Delete User failed";

        free(msg);
        free(ackMsg);
        return false;
    }
    free(msg);
    free(ackMsg);

    emit AccountUpdate();

    return true;
}

void NetWork::configServertoClient(ConfigStr* dst,SysInfo* src)
{
    int i;
    for(i=0;i<EZCAM_LED_NUM;i++)
    {
        dst->calibrate.value.light_config[i].enable=src->light_config.led[i].enable;
        dst->calibrate.value.light_config[i].pwmduty=src->light_config.led[i].pwmduty;
    }
    dst->calibrate.value.camera_WhiteBalanceMode=src->lan_config.nWhiteBalance;
    dst->calibrate.value.camera_DayNightMode=src->lan_config.nDayNight;
    dst->calibrate.value.camera_Binning=src->lan_config.nBinning;
    dst->calibrate.value.camera_BacklightControl=src->lan_config.nBacklightControl;
    dst->calibrate.value.camera_Backlight=src->lan_config.nBackLight;
    dst->calibrate.value.camera_Mirror=src->lan_config.mirror;
    dst->calibrate.value.camera_ExpPriority=src->lan_config.expPriority;
    dst->calibrate.value.camera_Contrast=src->lan_config.nContrast;
    dst->calibrate.value.camera_Brightness=src->lan_config.nBrightness;
    dst->calibrate.value.camera_Saturation=src->lan_config.nSaturation;
    dst->calibrate.value.camera_Sharpness=src->lan_config.nSharpness;
    dst->calibrate.value.camera_Exposure=src->lan_config.exposure;
    dst->calibrate.value.camera_SensorGain=src->lan_config.sensorGain;
    dst->calibrate.value.camera_PipeGain=src->lan_config.pipeGain;
    dst->calibrate.value.camera_2AMode=src->lan_config.nAEWtype;
    dst->calibrate.value.camera_2AVendor=src->lan_config.nAEWswitch;
    dst->calibrate.value.camera_histogram=src->lan_config.histogram;
    dst->calibrate.value.camera_framectrl=src->lan_config.framectrl;
    dst->calibrate.value.camera_frameRateVal1=src->lan_config.frameRateVal1;
    dst->calibrate.value.camera_frameRateVal2=src->lan_config.frameRateVal2;
    dst->calibrate.value.camera_frameRateVal3=src->lan_config.frameRateVal3;
    dst->calibrate.value.camera_VsEnable=src->lan_config.advanceMode.vsEnable;
    dst->calibrate.value.camera_LdcEnable=src->lan_config.advanceMode.ldcEnable;
    dst->calibrate.value.camera_VnfEnable=src->lan_config.advanceMode.vnfEnable;
    dst->calibrate.value.camera_VnfMode=src->lan_config.advanceMode.vnfMode;
    dst->calibrate.value.camera_VnfStrength=src->lan_config.advanceMode.vnfStrength;
    dst->calibrate.value.camera_dynRange=src->lan_config.advanceMode.dynRange;
    dst->calibrate.value.camera_dynRangeStrength=src->lan_config.advanceMode.dynRangeStrength;
    dst->calibrate.value.camera_2A_minExposure=src->lan_config.H3AParams.minExposure;
    dst->calibrate.value.camera_2A_maxExposure=src->lan_config.H3AParams.maxExposure;
    dst->calibrate.value.camera_2A_stepSize=src->lan_config.H3AParams.stepSize;
    dst->calibrate.value.camera_2A_aGainMin=src->lan_config.H3AParams.aGainMin;
    dst->calibrate.value.camera_2A_aGainMax=src->lan_config.H3AParams.aGainMax;
    dst->calibrate.value.camera_2A_dGainMin=src->lan_config.H3AParams.dGainMin;
    dst->calibrate.value.camera_2A_dGainMax=src->lan_config.H3AParams.dGainMax;
    dst->calibrate.value.camera_2A_TargetBrightness=src->lan_config.H3AParams.targetBrightness;
    dst->calibrate.value.camera_2A_MaxBrightness=src->lan_config.H3AParams.targetBrightnessMax;
    dst->calibrate.value.camera_2A_MinBrightness=src->lan_config.H3AParams.targetBrightnessMin;
    dst->calibrate.value.camera_2A_Threshold=src->lan_config.H3AParams.threshold;
    dst->calibrate.value.camera_2A_width=src->lan_config.H3AParams.width;
    dst->calibrate.value.camera_2A_height=src->lan_config.H3AParams.height;
    dst->calibrate.value.camera_2A_HCount=src->lan_config.H3AParams.HCount;
    dst->calibrate.value.camera_2A_VCount=src->lan_config.H3AParams.VCount;
    dst->calibrate.value.camera_2A_HStart=src->lan_config.H3AParams.HStart;
    dst->calibrate.value.camera_2A_VStart=src->lan_config.H3AParams.VStart;
    dst->calibrate.value.camera_2A_horzIncr=src->lan_config.H3AParams.horzIncr;
    dst->calibrate.value.camera_2A_vertIncr=src->lan_config.H3AParams.vertIncr;
    dst->calibrate.value.camera_2A_weight_width1=src->lan_config.H3AWeight.width1;
    dst->calibrate.value.camera_2A_weight_height1=src->lan_config.H3AWeight.height1;
    dst->calibrate.value.camera_2A_weight_h_start2=src->lan_config.H3AWeight.h_start2;
    dst->calibrate.value.camera_2A_weight_v_start2=src->lan_config.H3AWeight.v_start2;
    dst->calibrate.value.camera_2A_weight_width2=src->lan_config.H3AWeight.width2;
    dst->calibrate.value.camera_2A_weight_height2=src->lan_config.H3AWeight.height2;
    dst->calibrate.value.camera_2A_weight_weight=src->lan_config.H3AWeight.weight;
    dst->calibrate.value.camera_video_StreamType=src->codec_config.nStreamType;
    dst->calibrate.value.camera_video_VideoCodecMode=src->codec_config.nVideocodecmode;
    dst->calibrate.value.camera_video_JPEGQuality=src->codec_config.njpegquality;
    dst->calibrate.value.camera_video_combo=src->codec_config.nVideocombo;
    dst->calibrate.value.camera_video_codecres=src->codec_config.nVideocodecres;
    dst->calibrate.value.camera_video_frameRate1=src->codec_config.nFrameRate1;
    dst->calibrate.value.camera_video_frameRate2=src->codec_config.nFrameRate1;
    dst->calibrate.value.camera_video_frameRate3=src->codec_config.nFrameRate1;
    dst->calibrate.value.camera_video_Mpeg41Bitrate=src->codec_config.nMpeg41bitrate;
    dst->calibrate.value.camera_video_Mpeg42Bitrate=src->codec_config.nMpeg42bitrate;
    dst->calibrate.value.camera_video_rateControl1=src->codec_config.nRateControl1;
    dst->calibrate.value.camera_video_rateControl2=src->codec_config.nRateControl2;
    dst->calibrate.value.camera_video_codectype1=src->codec_config.codectype1;
    dst->calibrate.value.camera_video_codectype2=src->codec_config.codectype2;
    dst->calibrate.value.camera_video_codectype3=src->codec_config.codectype3;
    for(i=0;i<3;i++)
    {
        dst->calibrate.value.codec_advconfig[i].ipRatio=src->codec_config.codec_advconfig[i].ipRatio;
        dst->calibrate.value.codec_advconfig[i].fIframe=src->codec_config.codec_advconfig[i].fIframe;
        dst->calibrate.value.codec_advconfig[i].qpInit=src->codec_config.codec_advconfig[i].qpInit;
        dst->calibrate.value.codec_advconfig[i].qpMin=src->codec_config.codec_advconfig[i].qpMin;
        dst->calibrate.value.codec_advconfig[i].qpMax=src->codec_config.codec_advconfig[i].qpMax;
        dst->calibrate.value.codec_advconfig[i].meConfig=src->codec_config.codec_advconfig[i].meConfig;
        dst->calibrate.value.codec_advconfig[i].packetSize=src->codec_config.codec_advconfig[i].packetSize;
        dst->calibrate.value.codec_advconfig[i].unkown1=src->codec_config.codec_advconfig[i].unkown1;
        dst->calibrate.value.codec_advconfig[i].unkown2=src->codec_config.codec_advconfig[i].unkown2;
    }
    //    dst->run.value.LoadAlg=src->algparam.algEnable;
    //    dst->run.value.LoadAlg=0;
    //    dst->calibrate.value.algorithm_ROI_startX=src->algparam.ROI_startX;
    //    dst->calibrate.value.algorithm_ROI_endX=src->algparam.ROI_endX;
    //    dst->calibrate.value.algorithm_ROI_startY=src->algparam.ROI_startY;
    //    dst->calibrate.value.algorithm_ROI_endY=src->algparam.ROI_endY;
    //    dst->calibrate.value.algorithm_f_RDifSideCir=src->algparam.f_RDifSideCir;
    //    dst->calibrate.value.algorithm_fCirGood_1=src->algparam.fCirGood_1;
    //    dst->calibrate.value.algorithm_fCirGood_2=src->algparam.fCirGood_2;
    //    dst->calibrate.value.algorithm_fCirGood_3=src->algparam.fCirGood_3;
    //    dst->calibrate.value.algorithm_fCirWarning_1=src->algparam.fCirWarning_1;
    //    dst->calibrate.value.algorithm_fCirWarning_2=src->algparam.fCirWarning_2;
    //    dst->calibrate.value.algorithm_fCirWarning_3=src->algparam.fCirWarning_3;
    //    dst->calibrate.value.algorithm_fCenterCirGoodOffset=src->algparam.fCenterCirGoodOffset;
    //    dst->calibrate.value.algorithm_fCenterCirWarningOffset=src->algparam.fCenterCirWarningOffset;
    //    dst->calibrate.value.algorithm_nBiggestRaduis=src->algparam.nBiggestRaduis;
    //    dst->calibrate.value.algorithm_nSmallestRaduis=src->algparam.nSmallestRaduis;
    //    dst->calibrate.value.algorithm_Blot_BiggestArea=src->algparam.Blot_BiggestArea;
    //    dst->calibrate.value.algorithm_Blot_SmallestArea=src->algparam.Blot_SmallestArea;
    //    dst->calibrate.value.algorithm_Blot_xyOffset=src->algparam.Blot_xyOffset;
    //    dst->calibrate.value.algorithm_Cir_SmallestArea=src->algparam.Cir_SmallestArea;
    //    dst->calibrate.value.algorithm_Cir_xyOffset=src->algparam.Cir_xyOffset;
    //    dst->calibrate.value.algorithm_Cir_errf=src->algparam.Cir_errf;
    //    dst->calibrate.value.algorithm_Judge_xyOffset=src->algparam.Judge_xyOffset;
    //    dst->calibrate.value.algorithm_Speed_k1=src->algparam.Speed_k1;
    //    dst->calibrate.value.algorithm_Speed_k2=src->algparam.Speed_k2;
    //    dst->calibrate.value.algorithm_Speed_ExpandPixel1=src->algparam.Speed_ExpandPixel1;
    //    dst->calibrate.value.algorithm_Speed_ExpandPixel2=src->algparam.Speed_ExpandPixel2;
    dst->information.value.resource_totalmem=src->camInfo.total_mem;
    dst->information.value.resource_freemem=src->camInfo.free_mem;
    dst->information.value.resource_sharedmem=src->camInfo.share_mem;
    dst->information.value.resource_corearmmem=src->camInfo.core_mem[0];
    dst->information.value.resource_coredspmem=src->camInfo.core_mem[1];
    dst->information.value.resource_corem3vpssmem=src->camInfo.core_mem[2];
    dst->information.value.resource_corem3videomem=src->camInfo.core_mem[3];
    dst->information.value.resource_storagemem=src->camInfo.storage_mem;
    dst->information.value.resource_storagefreemem=src->camInfo.storage_free_mem;
    dst->information.value.resource_corearmfreq=src->camInfo.core_freq[0];
    dst->information.value.resource_coredspfreq=src->camInfo.core_freq[1];
    dst->information.value.resource_corem3vpssfreq=src->camInfo.core_freq[2];
    dst->information.value.resource_corem3videofreq=src->camInfo.core_freq[3];
    dst->information.value.device_version=src->camInfo.version;
    dst->state.algType=src->state.algType;
    dst->state.algTriggle=src->state.algTriggle;
    dst->state.algImgsrc=src->state.algImgsrc;
    dst->state.enableEncode=src->state.enableEncode;

}

void NetWork::configClienttoServer(SysInfo *dst,ConfigStr *src)
{
    int i;
    for(i=0;i<EZCAM_LED_NUM;i++)
    {
        dst->light_config.led[i].enable=src->calibrate.value.light_config[i].enable;
        dst->light_config.led[i].pwmduty=src->calibrate.value.light_config[i].pwmduty;
    }
    dst->lan_config.nWhiteBalance=src->calibrate.value.camera_WhiteBalanceMode;
    dst->lan_config.nDayNight=src->calibrate.value.camera_DayNightMode;
    dst->lan_config.nBinning=src->calibrate.value.camera_Binning;
    dst->lan_config.nBacklightControl=src->calibrate.value.camera_BacklightControl;
    dst->lan_config.nBackLight=src->calibrate.value.camera_Backlight;
    dst->lan_config.nBrightness=src->calibrate.value.camera_Brightness;
    dst->lan_config.nContrast=src->calibrate.value.camera_Contrast;
    dst->lan_config.mirror=src->calibrate.value.camera_Mirror;
    dst->lan_config.expPriority=src->calibrate.value.camera_ExpPriority;
    dst->lan_config.nSaturation=src->calibrate.value.camera_Saturation;
    dst->lan_config.nSharpness=src->calibrate.value.camera_Sharpness;
    dst->lan_config.exposure=src->calibrate.value.camera_Exposure;
    dst->lan_config.sensorGain=src->calibrate.value.camera_SensorGain;
    dst->lan_config.pipeGain=src->calibrate.value.camera_PipeGain;
    dst->lan_config.nAEWtype=src->calibrate.value.camera_2AMode;
    dst->lan_config.nAEWswitch=src->calibrate.value.camera_2AVendor;
    dst->lan_config.histogram=src->calibrate.value.camera_histogram;
    dst->lan_config.framectrl=src->calibrate.value.camera_framectrl;
    dst->lan_config.frameRateVal1=src->calibrate.value.camera_frameRateVal1;
    dst->lan_config.frameRateVal2=src->calibrate.value.camera_frameRateVal2;
    dst->lan_config.frameRateVal3=src->calibrate.value.camera_frameRateVal3;
    dst->lan_config.advanceMode.vsEnable=src->calibrate.value.camera_VsEnable;
    dst->lan_config.advanceMode.ldcEnable=src->calibrate.value.camera_LdcEnable;
    dst->lan_config.advanceMode.vnfEnable=src->calibrate.value.camera_VnfEnable;
    dst->lan_config.advanceMode.vnfMode=src->calibrate.value.camera_VnfMode;
    dst->lan_config.advanceMode.vnfStrength=src->calibrate.value.camera_VnfStrength;
    dst->lan_config.advanceMode.dynRange=src->calibrate.value.camera_dynRange;
    dst->lan_config.advanceMode.dynRangeStrength=src->calibrate.value.camera_dynRangeStrength;
    dst->lan_config.H3AParams.minExposure=src->calibrate.value.camera_2A_minExposure;
    dst->lan_config.H3AParams.maxExposure=src->calibrate.value.camera_2A_maxExposure;
    dst->lan_config.H3AParams.stepSize=src->calibrate.value.camera_2A_stepSize;
    dst->lan_config.H3AParams.aGainMin=src->calibrate.value.camera_2A_aGainMin;
    dst->lan_config.H3AParams.aGainMax=src->calibrate.value.camera_2A_aGainMax;
    dst->lan_config.H3AParams.dGainMin=src->calibrate.value.camera_2A_dGainMin;
    dst->lan_config.H3AParams.dGainMax=src->calibrate.value.camera_2A_dGainMax;
    dst->lan_config.H3AParams.targetBrightness=src->calibrate.value.camera_2A_TargetBrightness;
    dst->lan_config.H3AParams.targetBrightnessMax=src->calibrate.value.camera_2A_MaxBrightness;
    dst->lan_config.H3AParams.targetBrightnessMin=src->calibrate.value.camera_2A_MinBrightness;
    dst->lan_config.H3AParams.threshold=src->calibrate.value.camera_2A_Threshold;
    dst->lan_config.H3AParams.width=src->calibrate.value.camera_2A_width;
    dst->lan_config.H3AParams.height=src->calibrate.value.camera_2A_height;
    dst->lan_config.H3AParams.HCount=src->calibrate.value.camera_2A_HCount;
    dst->lan_config.H3AParams.VCount=src->calibrate.value.camera_2A_VCount;
    dst->lan_config.H3AParams.HStart=src->calibrate.value.camera_2A_HStart;
    dst->lan_config.H3AParams.VStart=src->calibrate.value.camera_2A_VStart;
    dst->lan_config.H3AParams.horzIncr=src->calibrate.value.camera_2A_horzIncr;
    dst->lan_config.H3AParams.vertIncr=src->calibrate.value.camera_2A_vertIncr;
    dst->lan_config.H3AWeight.width1=src->calibrate.value.camera_2A_weight_width1;
    dst->lan_config.H3AWeight.height1=src->calibrate.value.camera_2A_weight_height1;
    dst->lan_config.H3AWeight.h_start2=src->calibrate.value.camera_2A_weight_h_start2;
    dst->lan_config.H3AWeight.v_start2=src->calibrate.value.camera_2A_weight_v_start2;
    dst->lan_config.H3AWeight.width2=src->calibrate.value.camera_2A_weight_width2;
    dst->lan_config.H3AWeight.height2=src->calibrate.value.camera_2A_weight_height2;
    dst->lan_config.H3AWeight.weight=src->calibrate.value.camera_2A_weight_weight;
    dst->codec_config.nStreamType=src->calibrate.value.camera_video_StreamType;
    dst->codec_config.nVideocodecmode=src->calibrate.value.camera_video_VideoCodecMode;
    dst->codec_config.njpegquality=src->calibrate.value.camera_video_JPEGQuality;
    dst->codec_config.nVideocombo=src->calibrate.value.camera_video_combo;
    dst->codec_config.nVideocodecres=src->calibrate.value.camera_video_codecres;
    dst->codec_config.nFrameRate1=src->calibrate.value.camera_video_frameRate1;
    dst->codec_config.nFrameRate2=src->calibrate.value.camera_video_frameRate2;
    dst->codec_config.nFrameRate3=src->calibrate.value.camera_video_frameRate3;
    dst->codec_config.nMpeg41bitrate=src->calibrate.value.camera_video_Mpeg41Bitrate;
    dst->codec_config.nMpeg42bitrate=src->calibrate.value.camera_video_Mpeg42Bitrate;
    dst->codec_config.nRateControl1=src->calibrate.value.camera_video_rateControl1;
    dst->codec_config.nRateControl2=src->calibrate.value.camera_video_rateControl2;
    dst->codec_config.codectype1=src->calibrate.value.camera_video_codectype1;
    dst->codec_config.codectype2=src->calibrate.value.camera_video_codectype2;
    dst->codec_config.codectype3=src->calibrate.value.camera_video_codectype3;
    for(i=0;i<3;i++)
    {
        dst->codec_config.codec_advconfig[i].ipRatio=src->calibrate.value.codec_advconfig[i].ipRatio;
        dst->codec_config.codec_advconfig[i].fIframe=src->calibrate.value.codec_advconfig[i].fIframe;
        dst->codec_config.codec_advconfig[i].qpInit=src->calibrate.value.codec_advconfig[i].qpInit;
        dst->codec_config.codec_advconfig[i].qpMin=src->calibrate.value.codec_advconfig[i].qpMin;
        dst->codec_config.codec_advconfig[i].qpMax=src->calibrate.value.codec_advconfig[i].qpMax;
        dst->codec_config.codec_advconfig[i].meConfig=src->calibrate.value.codec_advconfig[i].meConfig;
        dst->codec_config.codec_advconfig[i].packetSize=src->calibrate.value.codec_advconfig[i].packetSize;
        dst->codec_config.codec_advconfig[i].unkown1=src->calibrate.value.codec_advconfig[i].unkown1;
        dst->codec_config.codec_advconfig[i].unkown2=src->calibrate.value.codec_advconfig[i].unkown2;
    }
    dst->state.algType=src->state.algType;
    dst->state.algTriggle=src->state.algTriggle;
    dst->state.algImgsrc=src->state.algImgsrc;
    dst->state.enableEncode=src->state.enableEncode;
    //    dst->algparam.ROI_startX=src->calibrate.value.algorithm_ROI_startX;
    //    dst->algparam.ROI_endX=src->calibrate.value.algorithm_ROI_endX;
    //    dst->algparam.ROI_startY=src->calibrate.value.algorithm_ROI_startY;
    //    dst->algparam.ROI_endY=src->calibrate.value.algorithm_ROI_endY;
    //    dst->algparam.f_RDifSideCir=src->calibrate.value.algorithm_f_RDifSideCir;
    //    dst->algparam.fCirGood_1=src->calibrate.value.algorithm_fCirGood_1;
    //    dst->algparam.fCirGood_2=src->calibrate.value.algorithm_fCirGood_2;
    //    dst->algparam.fCirGood_3=src->calibrate.value.algorithm_fCirGood_3;
    //    dst->algparam.fCirWarning_1=src->calibrate.value.algorithm_fCirWarning_1;
    //    dst->algparam.fCirWarning_2=src->calibrate.value.algorithm_fCirWarning_2;
    //    dst->algparam.fCirWarning_3=src->calibrate.value.algorithm_fCirWarning_3;
    //    dst->algparam.fCenterCirGoodOffset=src->calibrate.value.algorithm_fCenterCirGoodOffset;
    //    dst->algparam.fCenterCirWarningOffset=src->calibrate.value.algorithm_fCenterCirWarningOffset;
    //    dst->algparam.nBiggestRaduis=src->calibrate.value.algorithm_nBiggestRaduis;
    //    dst->algparam.nSmallestRaduis=src->calibrate.value.algorithm_nSmallestRaduis;
    //    dst->algparam.Blot_BiggestArea=src->calibrate.value.algorithm_Blot_BiggestArea;
    //    dst->algparam.Blot_SmallestArea=src->calibrate.value.algorithm_Blot_SmallestArea;
    //    dst->algparam.Blot_xyOffset=src->calibrate.value.algorithm_Blot_xyOffset;
    //    dst->algparam.Cir_SmallestArea=src->calibrate.value.algorithm_Cir_SmallestArea;
    //    dst->algparam.Cir_xyOffset=src->calibrate.value.algorithm_Cir_xyOffset;
    //    dst->algparam.Cir_errf=src->calibrate.value.algorithm_Cir_errf;
    //    dst->algparam.Judge_xyOffset=src->calibrate.value.algorithm_Judge_xyOffset;
    //    dst->algparam.Speed_k1=src->calibrate.value.algorithm_Speed_k1;
    //    dst->algparam.Speed_k2=src->calibrate.value.algorithm_Speed_k2;
    //    dst->algparam.Speed_ExpandPixel1=src->calibrate.value.algorithm_Speed_ExpandPixel1;
    //    dst->algparam.Speed_ExpandPixel2=src->calibrate.value.algorithm_Speed_ExpandPixel2;
    //    dst->algparam.algEnable=src->run.value.LoadAlg;


}

void NetWork::networkconfigServertoClinet(NetworkStr *dst, SysInfo *src)
{
    //    dst->value.ports_ipaddress=inet_ntoa(src->net.net_config.ip);
    dst->value.ports_ipaddress=src->net.net_config.ip;
    //    dst->value.ports_netmask=inet_ntoa(src->net.net_config.netmask);
    //    dst->value.ports_gateway=inet_ntoa(src->net.net_config.gateway);
    //    dst->value.ports_dns=inet_ntoa(src->net.net_config.dns);
    dst->value.ports_netmask=src->net.net_config.netmask;
    dst->value.ports_gateway=src->net.net_config.gateway;
    dst->value.ports_dns=src->net.net_config.dns;
    dst->value.ports_httpport=src->net.net_config.http_port;
    dst->value.ports_httpsport=src->net.net_config.https_port;
    dst->value.ports_dhcpenable=src->net.net_config.dhcp_enable;
    dst->value.ports_dhcpconfig=src->net.net_config.dhcp_config;
    dst->value.ports_ntpserver=src->net.net_config.ntp_server;
    dst->value.ports_ntptimezone=src->net.net_config.ntp_timezone;
    dst->value.ports_mac=QString::number(src->net.net_config.MAC[0],16)+":"+QString::number(src->net.net_config.MAC[1],16)+":"\
            +QString::number(src->net.net_config.MAC[2],16)+":"+QString::number(src->net.net_config.MAC[3],16)+":"+QString::number(src->net.net_config.MAC[4],16)+\
            ":"+QString::number(src->net.net_config.MAC[5],16);
    dst->value.ports_ftpserverip=src->net.ftp_config.servier_ip;
    dst->value.ports_ftpserverport=QString::number(src->net.ftp_config.port,10);
    dst->value.ports_ftpusername=src->net.ftp_config.username;
    dst->value.ports_ftppassword=src->net.ftp_config.password;
    dst->value.ports_ftpfoldername=src->net.ftp_config.foldername;
    dst->value.ports_pid=src->net.ftp_config.pid;
    dst->value.ports_rftpenable=src->net.ftp_config.rftpenable;
    dst->value.ports_ftpfileformat=src->net.ftp_config.ftpfileformat;
    dst->value.ports_rtspurl.clear();
    dst->value.ports_rtspurl.append(src->net.rtsp_config.rtspUrl);
    dst->value.ports_rtspmulticast=src->net.rtsp_config.multicast_enable;
    dst->value.ports_mtype=src->net.rtsp_config.mtype;
    dst->value.ports_rtspport[0]=src->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[0];
    dst->value.ports_rtspport[1]=src->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[1];
    dst->value.ports_rtspport[2]=src->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[2];
    dst->value.ports_rtspport[3]=src->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[3];
    dst->value.ports_rtspport[4]=src->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[4];
    dst->value.ports_sizeport=src->net.rtsp_config.size_Port;
    dst->value.ports_rtspenable=src->net.rtsp_config.RTSP_Enable;
    dst->value.ports_change=src->net.rtsp_config.change;
}

void NetWork::networkconfigClienttoServer(SysInfo *dst, NetworkStr *src)
{
    qDebug()<<src->value.ports_ipaddress<<src->value.ports_netmask<<src->value.ports_gateway<<src->value.ports_dns;
    //    dst->net.net_config.ip.S_un.S_addr=inet_addr(src->value.ports_ipaddress.toLatin1().data());
    memcpy(dst->net.net_config.ip,src->value.ports_ipaddress.toLatin1().data(),strlen(src->value.ports_ipaddress.toLatin1().data())+1);
    memcpy(dst->net.net_config.netmask,src->value.ports_netmask.toLatin1().data(),strlen(src->value.ports_netmask.toLatin1().data())+1);
    memcpy(dst->net.net_config.gateway,src->value.ports_gateway.toLatin1().data(),strlen(src->value.ports_gateway.toLatin1().data())+1);
    memcpy(dst->net.net_config.dns,src->value.ports_dns.toLatin1().data(),strlen(src->value.ports_dns.toLatin1().data())+1);
    //    dst->net.net_config.netmask.S_un.S_addr=inet_addr(src->value.ports_netmask.toLatin1().data());
    //    dst->net.net_config.gateway.S_un.S_addr=inet_addr(src->value.ports_gateway.toLatin1().data());
    //    dst->net.net_config.dns.S_un.S_addr=inet_addr(src->value.ports_dns.toLatin1().data());
    dst->net.net_config.http_port=src->value.ports_httpport;
    dst->net.net_config.https_port=src->value.ports_httpsport;
    dst->net.net_config.dhcp_enable=src->value.ports_dhcpenable;
    dst->net.net_config.dhcp_config=src->value.ports_dhcpconfig;
    strcpy(dst->net.net_config.ntp_server,src->value.ports_ntpserver.toLatin1().data());
    dst->net.net_config.ntp_timezone=src->value.ports_ntptimezone;
    QString macS=src->value.ports_mac;
    bool ok;
    dst->net.net_config.MAC[0]=macS.left(macS.indexOf(":")).toInt(&ok,16);
    macS=macS.right(macS.count()-macS.indexOf(":")-1);
    dst->net.net_config.MAC[1]=macS.left(macS.indexOf(":")).toInt(&ok,16);
    macS=macS.right(macS.count()-macS.indexOf(":")-1);
    dst->net.net_config.MAC[2]=macS.left(macS.indexOf(":")).toInt(&ok,16);
    macS=macS.right(macS.count()-macS.indexOf(":")-1);
    dst->net.net_config.MAC[3]=macS.left(macS.indexOf(":")).toInt(&ok,16);
    macS=macS.right(macS.count()-macS.indexOf(":")-1);
    dst->net.net_config.MAC[4]=macS.left(macS.indexOf(":")).toInt(&ok,16);
    macS=macS.right(macS.count()-macS.indexOf(":")-1);
    dst->net.net_config.MAC[5]=macS.left(macS.indexOf(":")).toInt(&ok,16);
    //    qDebug()<<dst->net.net_config.MAC[0]<<dst->net.net_config.MAC[1]<<dst->net.net_config.MAC[2]<<dst->net.net_config.MAC[3]<<dst->net.net_config.MAC[4]<<dst->net.net_config.MAC[5];
    strcpy(dst->net.ftp_config.servier_ip,src->value.ports_ftpserverip.toLatin1().data());
    dst->net.ftp_config.port=src->value.ports_ftpserverport.toInt();
    strcpy(dst->net.ftp_config.username,src->value.ports_ftpusername.toLatin1().data());
    strcpy(dst->net.ftp_config.password,src->value.ports_ftppassword.toLatin1().data());
    strcpy(dst->net.ftp_config.foldername,src->value.ports_ftpfoldername.toLatin1().data());
    dst->net.ftp_config.pid=src->value.ports_pid;
    dst->net.ftp_config.rftpenable=src->value.ports_rftpenable;
    dst->net.ftp_config.ftpfileformat=src->value.ports_ftpfileformat;
    strcpy(dst->net.rtsp_config.rtspUrl,src->value.ports_rtspurl.toLatin1().data());
    dst->net.rtsp_config.multicast_enable=src->value.ports_rtspmulticast;
    dst->net.rtsp_config.mtype=src->value.ports_mtype;
    dst->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[0]=src->value.ports_rtspport[0];
    dst->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[1]=src->value.ports_rtspport[1];
    dst->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[2]=src->value.ports_rtspport[2];
    dst->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[3]=src->value.ports_rtspport[3];
    dst->net.rtsp_config.rtspServerPort_H264_MPEG4_MJPEG[4]=src->value.ports_rtspport[4];
    dst->net.rtsp_config.size_Port=src->value.ports_sizeport;
    dst->net.rtsp_config.RTSP_Enable=src->value.ports_rtspenable;
    dst->net.rtsp_config.change=src->value.ports_change;
}

void NetWork::saveNetwork()
{

    networkconfig->setValue(network.name.ports_ipaddress,network.value.ports_ipaddress);
    networkconfig->setValue(network.name.ports_netmask,network.value.ports_netmask);
    networkconfig->setValue(network.name.ports_gateway,network.value.ports_gateway);
    networkconfig->setValue(network.name.ports_dns,network.value.ports_dns);
    networkconfig->setValue(network.name.ports_httpport,network.value.ports_httpport);
    networkconfig->setValue(network.name.ports_httpsport,network.value.ports_httpsport);
    networkconfig->setValue(network.name.ports_dhcpenable,network.value.ports_dhcpenable);
    networkconfig->setValue(network.name.ports_dhcpconfig,network.value.ports_dhcpconfig);
    networkconfig->setValue(network.name.ports_ntpserver,network.value.ports_ntpserver);
    networkconfig->setValue(network.name.ports_ntptimezone,network.value.ports_ntptimezone);
    networkconfig->setValue(network.name.ports_mac,network.value.ports_mac);
    networkconfig->setValue(network.name.ports_ftpserverip,network.value.ports_ftpserverip);
    networkconfig->setValue(network.name.ports_ftpserverport,network.value.ports_ftpserverport);
    networkconfig->setValue(network.name.ports_ftpusername,network.value.ports_ftpusername);
    networkconfig->setValue(network.name.ports_ftppassword,network.value.ports_ftppassword);
    networkconfig->setValue(network.name.ports_ftpfoldername,network.value.ports_ftpfoldername);
    networkconfig->setValue(network.name.ports_pid,network.value.ports_pid);
    networkconfig->setValue(network.name.ports_rftpenable,network.value.ports_rftpenable);
    networkconfig->setValue(network.name.ports_ftpfileformat,network.value.ports_ftpfileformat);
    networkconfig->setValue(network.name.ports_rtspurl,network.value.ports_rtspurl);
    networkconfig->setValue(network.name.ports_rtspmulticast,network.value.ports_rtspmulticast);
    networkconfig->setValue(network.name.ports_mtype,network.value.ports_mtype);
    networkconfig->setValue(network.name.ports_rtspport[0],network.value.ports_rtspport[0]);
    networkconfig->setValue(network.name.ports_rtspport[1],network.value.ports_rtspport[1]);
    networkconfig->setValue(network.name.ports_rtspport[2],network.value.ports_rtspport[2]);
    networkconfig->setValue(network.name.ports_rtspport[3],network.value.ports_rtspport[3]);
    networkconfig->setValue(network.name.ports_rtspport[4],network.value.ports_rtspport[4]);
    networkconfig->setValue(network.name.ports_sizeport,network.value.ports_sizeport);
    networkconfig->setValue(network.name.ports_rtspenable,network.value.ports_rtspenable);
    networkconfig->setValue(network.name.ports_change,network.value.ports_change);
}

void NetWork::loadNetwork()
{
    network.value.ports_ipaddress=networkconfig->value(network.name.ports_ipaddress).toString();
    network.value.ports_netmask=networkconfig->value(network.name.ports_netmask).toString();
    network.value.ports_gateway=networkconfig->value(network.name.ports_gateway).toString();
    network.value.ports_dns=networkconfig->value(network.name.ports_dns).toString();
    network.value.ports_httpport=networkconfig->value(network.name.ports_httpport).toInt();
    network.value.ports_httpsport=networkconfig->value(network.name.ports_httpsport).toInt();
    network.value.ports_dhcpenable=networkconfig->value(network.name.ports_dhcpenable).toInt();
    network.value.ports_dhcpconfig=networkconfig->value(network.name.ports_dhcpconfig).toInt();
    network.value.ports_ntpserver=networkconfig->value(network.name.ports_ntpserver).toString();
    network.value.ports_ntptimezone=networkconfig->value(network.name.ports_ntptimezone).toInt();
    network.value.ports_mac=networkconfig->value(network.name.ports_mac).toString();
    network.value.ports_ftpserverip=networkconfig->value(network.name.ports_ftpserverip).toString();
    network.value.ports_ftpserverport=networkconfig->value(network.name.ports_ftpserverport).toString();
    network.value.ports_ftpusername=networkconfig->value(network.name.ports_ftpusername).toString();
    network.value.ports_ftppassword=networkconfig->value(network.name.ports_ftppassword).toString();
    network.value.ports_ftpfoldername=networkconfig->value(network.name.ports_ftpfoldername).toString();
    network.value.ports_pid=networkconfig->value(network.name.ports_pid).toInt();
    network.value.ports_rftpenable=networkconfig->value(network.name.ports_rftpenable).toInt();
    network.value.ports_ftpfileformat=networkconfig->value(network.name.ports_ftpfileformat).toInt();
    network.value.ports_rtspurl=networkconfig->value(network.name.ports_rtspurl).toString();
    network.value.ports_rtspmulticast=networkconfig->value(network.name.ports_rtspmulticast).toInt();
    network.value.ports_mtype=networkconfig->value(network.name.ports_mtype).toInt();
    network.value.ports_rtspport[0]=networkconfig->value(network.name.ports_rtspport[0]).toInt();
    network.value.ports_rtspport[1]=networkconfig->value(network.name.ports_rtspport[1]).toInt();
    network.value.ports_rtspport[2]=networkconfig->value(network.name.ports_rtspport[2]).toInt();
    network.value.ports_rtspport[3]=networkconfig->value(network.name.ports_rtspport[3]).toInt();
    network.value.ports_rtspport[4]=networkconfig->value(network.name.ports_rtspport[4]).toInt();
    network.value.ports_sizeport=networkconfig->value(network.name.ports_sizeport).toInt();
    network.value.ports_rtspenable=networkconfig->value(network.name.ports_rtspenable).toInt();
    network.value.ports_change=networkconfig->value(network.name.ports_change).toInt();
}

QString NetWork::getLocalIP(QString name)
{
    QList<QNetworkInterface>list=QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface interfaces,list)
    {
        if(interfaces.humanReadableName()==name)
        {
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

void NetWork::ftp_refresh()
{
    if(m_hFtp->ftpStatus()!=FTP_CONNECTED)
        emit ftp_login(network.value.ports_ipaddress,8010,"root","ftpcam");
}

void NetWork::disconnectConnection()
{
    if(ClientSocket->state()!=QAbstractSocket::UnconnectedState)
        ClientSocket->disconnectFromHost();
}

void NetWork::reportError(QAbstractSocket::SocketError err)
{
    qDebug()<<"error: "<<err;
}

void NetWork::openAlgResultService()
{
    if(algResult->socketState()!=QAbstractSocket::ConnectedState)
        algResult->connectSocket(network.value.ports_ipaddress,EZ_SERVERRESULTPORT);
}

