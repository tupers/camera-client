#include "ftp_service.h"

/**
 * @brief ftp_service::ftp_service
 */
ftp_service::ftp_service()
{
}

/**
 * @brief ftp_service::~ftp_service
 */
ftp_service::~ftp_service()
{
    deinitFtp();
}

/**
 * @brief ftp_service::initFtp
 */
void ftp_service::initFtp()
{
    deinitFtp();
    //create socket and server
    m_hCmdSocket=new QTcpSocket;
    m_hdataServer=new QTcpServer;
    connect(m_hCmdSocket,&QTcpSocket::connected,this,[=](){
        if(checkAccount())
        {
            emit sendToLog("connected.open data server");
            //if(!m_hdataServer->listen(QHostAddress(m_strRemoteIP),0))
            if(!m_hdataServer->listen(QHostAddress::Any,0))
            {
                closeFtp();
                emit sendToLog("listen falied");
            }
            else
            {
                m_hTimer->stop();
                m_eStatus = FTP_CONNECTED;
                m_nDataPort = m_hdataServer->serverPort();
                list();
            }
        }
        else
        {
            emit sendToLog("ftp account error.");
            closeFtp();
        }
    });
    connect(m_hdataServer,&QTcpServer::newConnection,this,[=](){
        QTcpSocket* remoteSocket = m_hdataServer->nextPendingConnection();
        Ftp_RemoteManager man;
        man.isFirst=0;
        man.cmd=FTP_CMD(0);
        m_mapRemote.insert(remoteSocket,man);
        connect(remoteSocket,SIGNAL(readyRead()),this,SLOT(readRemoteData()));

        connect(remoteSocket,&QTcpSocket::disconnected,this,[=](){
            if(m_mapRemote[remoteSocket].cmd==FTP_PUT)
                emit sendToLog("Put file finished");
        });
    });

    //init timeout timer
    m_hTimer = new QTimer;
    m_hTimer->setInterval(FTP_TIMEOUT);
    m_hTimer->setSingleShot(true);
    connect(m_hTimer,&QTimer::timeout,this,[=](){
        emit sendToLog("login connection time out");
        closeFtp();
    });

    //init status
    m_bInit=true;
}

/**
 * @brief ftp_service::deinitFtp
 */
void ftp_service::deinitFtp()
{
    if(m_hTimer!=NULL)
    {
        delete m_hTimer;
        m_hTimer=NULL;
    }

    for(auto i = m_mapRemote.begin();i!=m_mapRemote.end();++i)
    {
        i.key()->abort();
        i.key()->deleteLater();
    }
    m_mapRemote.clear();

    if(m_hCmdSocket!=NULL)
    {
        m_hCmdSocket->abort();
        delete m_hCmdSocket;
    }
    if(m_hdataServer!=NULL)
    {
        m_hdataServer->close();
        delete m_hdataServer;
    }
    m_bInit=false;
}

/**
 * @brief ftp_service::loginFtp
 * @param ip
 * @param port
 * @param username
 * @param password
 */
void ftp_service::loginFtp(QString ip, int port, QString username, QString password)
{
    //init ftp
    if(!m_bInit)
        initFtp();
    //check connection
    if(m_eStatus==FTP_CONNECTED)
    {
        emit sendToLog("ftp service has already connected.");
        return;
    }
    //connect to remote server
    closeFtp();
    m_hCmdSocket->connectToHost(ip,port);
    m_strUserName=username;
    m_strPassword=password;
    m_strRemoteIP=ip;
    m_nRemotePort=port;

    //start timer
    m_hTimer->start();
}

/**
 * @brief ftp_service::closeFtp
 */
void ftp_service::closeFtp()
{
    if(m_hdataServer!=NULL)
        m_hdataServer->close();
    if(m_hCmdSocket!=NULL)
        m_hCmdSocket->abort();

    m_strCurDir == DEFAULT_PATH;
    m_eStatus=FTP_DISCONNECTED;
    m_strCurDir=DEFAULT_PATH;
}

/**
 * @brief ftp_service::del
 * @param name
 * @return
 */
bool ftp_service::del(QString name)
{
    //check status
    if(m_eStatus!=FTP_CONNECTED)
    {
        emit sendToLog("ftp service has no connection");
        return false;
    }

    //check args
    if(name.length()>=MAX_FILE_SIZE)
    {
        emit sendToLog("get file name exceed");
        return false;
    }

    //pack data
    FTP_File data;
    strcpy(data.filepath,name.toLatin1().data());

    //send msg;
    return sendCMD(FTP_DELETE,&data,sizeof(FTP_File));
}

/**
 * @brief ftp_service::get
 * @param name
 * @return
 */
bool ftp_service::get(QString name)
{
    //check status
    if(m_eStatus!=FTP_CONNECTED)
    {
        emit sendToLog("ftp service has no connection");
        return false;
    }

    //check args
    if(name.length()>=MAX_FILE_SIZE)
    {
        emit sendToLog("get file name exceed");
        return false;
    }

    //pack data
    FTP_File data;
    name = m_strCurDir+"/"+name;
    data.port=m_nDataPort;
    strcpy(data.ip,getLocalIP().toLatin1().data());
    strcpy(data.filepath,name.toLatin1().data());

    //send msg;
    return sendCMD(FTP_GET,&data,sizeof(FTP_File));
}

/**
 * @brief ftp_service::put
 * @param name
 * @param ba
 * @return
 */
bool ftp_service::put(QString name, QByteArray ba)
{
    //check status
    if(m_eStatus!=FTP_CONNECTED)
    {
        emit sendToLog("ftp service has no connection");
        return false;
    }

    //check args
    if(name.length()>=MAX_FILE_SIZE)
    {
        emit sendToLog("get file name exceed");
        return false;
    }

    //pack data
    m_arrPut.clear();
    m_arrPut.append(ba.data(),ba.length());
    FTP_File data;
    data.port=m_nDataPort;
    strcpy(data.ip,getLocalIP().toLatin1().data());
    strcpy(data.filepath,name.toLatin1().data());
    data.fileSize = m_arrPut.length();

    //send msg
    return sendCMD(FTP_PUT,&data,sizeof(FTP_File));
}

/**
 * @brief ftp_service::list
 * @param name
 * @return
 */
bool ftp_service::list(QString name)
{
    m_strCurDir = name;
    //check status
    if(m_eStatus!=FTP_CONNECTED)
    {
        emit sendToLog("ftp service has no connection");
        return false;
    }

    //check args
    if(name.length()>=MAX_FILE_SIZE)
    {
        emit sendToLog("get file name exceed");
        return false;
    }

    //pack data
    FTP_File data;
    data.port=m_nDataPort;
    strcpy(data.ip,getLocalIP().toLatin1().data());
    strcpy(data.filepath,name.toLatin1().data());

    //send msg
    return sendCMD(FTP_LIST,&data,sizeof(FTP_File));
}

/**
 * @brief ftp_service::readRemoteData
 */
void ftp_service::readRemoteData()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());

    QByteArray ba;
    Ftp_Ack ack;
    ack.status = FTP_ACK_SOK;

    while(socket->bytesAvailable())
        ba = socket->readAll();

    //check first time
    if(m_mapRemote[socket].isFirst==0)
    {
        if(ba.length()==sizeof(Ftp_Header))
        {
            Ftp_Header header;
            memcpy(&header,ba.data(),ba.length());
            if(header.status==FTP_ACK_SOK&&header.magic1==MAGIC_HEADER1&&header.magic2==MAGIC_HEADER2&&(header.cmd==FTP_GET||header.cmd==FTP_LIST||header.cmd==FTP_PUT))
            {
                m_mapRemote[socket].cmd=FTP_CMD(header.cmd);
                if(header.cmd==FTP_PUT)//if cmd is PUT, send data immediatly
                {
                    emit sendToLog("data write");
                    socket->write(m_arrPut.data(),m_arrPut.length());
                }
                else//if cmd is LIST or GET, ready to receive
                {
                    m_arrGet.clear();
                    m_mapRemote[socket].size = header.pad==0?header.len:header.len-1;
                    m_mapRemote[socket].curSize = 0;
                    m_mapRemote[socket].isFirst=1;
                    ack.len=ba.length();
                    //qDebug()<<ack.len;
                    socket->write((const char*)&ack,sizeof(Ftp_Ack));
                }
            }
            else
            {
                emit sendToLog("header check failed or invalid ftp cmd");
                ack.status=FTP_ACK_SFAIL;
                socket->write((const char*)&ack,sizeof(Ftp_Ack));
                abortConnection(socket);

            }
        }
        else
        {
            emit sendToLog("header size error.");
            ack.status=FTP_ACK_SFAIL;
            socket->write((const char*)&ack,sizeof(Ftp_Ack));
            abortConnection(socket);
        }
    }
    else
    {
        m_arrGet.append(ba.data(),ba.length());
        m_mapRemote[socket].curSize += ba.length();
        ack.len=ba.length();
        socket->write((const char*)&ack,sizeof(Ftp_Ack));
        if(m_mapRemote[socket].curSize>=m_mapRemote[socket].size)
        {
            emit sendToLog("data receive finished.");
            if(!processData(m_mapRemote[socket].cmd,m_arrGet))
                emit sendToLog("invalid ftp cmd.");
        }
    }
}

/**
 * @brief ftp_service::getType
 * @param type
 * @return
 */
QString ftp_service::getType(int type)
{
    if(type==DIR_TYPE)
        return "Dir";
    else if(type==FILE_TYPE)
        return "File";
    else
        return "Unknown";
}

/**
 * @brief ftp_service::sendCMD
 * @param cmd
 * @param data
 * @param size
 * @return
 */
bool ftp_service::sendCMD(FTP_CMD cmd, void *data, int size)
{
    emit sendToLog(QString("Send ftp cmd:%1...").arg(cmd));

    int len = sizeof(netMsg) + size;
    netMsg* msg = (netMsg *)malloc(len);
    memset(msg,0,len);
    msg->cmdValueLen=size;
    msg->cmd = cmd;
    memcpy(msg->cmdValue,data,size);

    int acklen = sizeof(netMsg);
    netMsg* ackMsg = (netMsg *)malloc(acklen);
    int ret = SendMsg(m_hCmdSocket,msg,len,ackMsg,&acklen);
    if(ret != MSG_SOK)
    {
        emit sendToLog(QString("Send ftp cmd:%1 failed").arg(cmd));

        free(msg);
        free(ackMsg);
        return false;
    }
    free(msg);
    free(ackMsg);
    return true;
}

/**
 * @brief ftp_service::checkAccount
 * @return
 */
bool ftp_service::checkAccount()
{
    emit sendToLog("Checking account...");

    if((m_strUserName.length()>MAX_USERNAME)||(m_strUserName.length()<MIN_USERNAME)||(m_strPassword.length()>MAX_PASSWD)||(m_strPassword.length()<MIN_PASSWD))
    {
        emit sendToLog("user name is invalid!!!");
        return false;
    }

    Ftp_Account account;
    memset(&account,0,sizeof(Ftp_Account));
    memcpy(account.UserName,m_strUserName.toLatin1().data(),m_strUserName.size());
    memcpy(account.Passwd,m_strPassword.toLatin1().data(),m_strPassword.size());

    return sendCMD(FTP_LOGIN,&account,sizeof(Ftp_Account));
}

/**
 * @brief ftp_service::abortConnection
 * @param sock
 */
void ftp_service::abortConnection(QTcpSocket * sock)
{
    sock->abort();
    delete sock;
    m_mapRemote.remove(sock);
}

/**
 * @brief ftp_service::processData
 * @param cmd
 * @param ba
 * @return
 */
bool ftp_service::processData(FTP_CMD cmd, QByteArray ba)
{
    bool ret=true;
    switch (cmd) {
    case FTP_GET:
        emit receiveData(ba);
        break;
    case FTP_LIST:
        emit receiveFileList(ba);
        break;
    default:
        ret=false;
        break;
    }
    return ret;
}
