#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QtNetwork/QtNetwork>
#include <QSettings>
#include <QThread>

#include "ezstream.h"
#include "configstruct.h"
#include "winsock.h"
#include "udpservice.h"
#include "dm8127ftp_service.h"
#include "dm8127scan_service.h"

class NetWork : public QObject
{
    Q_OBJECT
public:
    explicit NetWork(QObject *parent = 0);
    ~NetWork();
    int SendServerMsg(QTcpSocket *ClientSocket,NetMsg *msg,int len,NetMsg *ackMsg,int32_t *acklen);
    void Login(QString,QString);
    bool LoginCheckAccount(uint8_t *authority);
    bool GetSysinfo(ConfigStr*);
//    bool GetAlgresult(EzAlgResult*);
    bool GetCpuloadresult(EzcpuDynamicParam*);
    bool DeleteUser(QString);
    QString getUserName(){return LogUserName;}
    QString getRTSPurl(){return network.value.ports_rtspurl;}
    void configServertoClient(ConfigStr*,SysInfo*);
    void configClienttoServer(SysInfo*,ConfigStr*);
    void networkconfigServertoClinet(NetworkStr*,SysInfo*);
    void networkconfigClienttoServer(SysInfo*,NetworkStr*);
    void saveNetwork();
    void loadNetwork();
    void setNetworkConfig(NetworkStr str){network.value=str.value;}
    NetworkStr getNetworkConfig(){return network;}
    QString getLogUserName(){return LogUserName;}
    int getLogAuthority(){return LogAuthority;}
    QString getLocalIP(QString);
    bool isFtpLogin(){return ftp->isLogin();}
//    bool GetCpuload();
signals:
    void sendToLog(QString);
    void loginFailed();
    void loginSuccess(uint8_t authority);
    void disconnectfromServer();
    void AccountUpdate();
    void udpRcvDataInfo(uchar*data,int width,int height,int pitch);
    void udpRcvDataInfo(QByteArray data);
    void connectFtp(QString , int,QString ,QString);
    void listFtp(QString path=DEFAULT_PATH);
    void closeFtp();
    void listInfoFtp(FileList*);
    void getFtp(QString);
    void getDataFtp(QByteArray);
    void putFtp(QString,QByteArray*);
    void cameraScan();
    void cameraDevice(int,QString);


public slots:
    void resetPayload();
    void startPayload();
    void printPayload();

    void LoginError();
    void LoginService();
    bool GetUser(get_userdata_t *userinfo);
    bool GetParams(enum _NET_MSG,void*,int);
    bool AddUser(QString,QString,int);
    void sendConfigToServer(enum _NET_MSG,unsigned char);
    void sendConfigToServer(_NET_MSG, unsigned char,unsigned char);
    void sendConfigToServer(_NET_MSG, void*,int );
    int getConfigFromSever(enum _NET_MSG,void *,int size=1);
//    unsigned short getConfigFromSever(_NET_MSG, unsigned char);
    void sendConfigToServerEntire(QVariant);
    bool sendConfigToServerEntireForBoot(QVariant);
    bool sendConfigToServerALG(QVariant);
    bool sendConfigToServerH3A(QVariant);
    void sendConfigToServerLightConfig(QVariant);
    void GetFrameFromSensor();
    void setSocketDebugMode();
    void sendNetworkConfig();
    void disconnectConnection();
    void reportError(QAbstractSocket::SocketError);
    QString getFtpCurrentDir(){return ftp->getCurrentDir();}
private:
    QTcpSocket *ClientSocket;
    QTimer *LogTimer=NULL;
    QString LogUserName="";
    QString LogPassWd="";
    unsigned char LogAuthority=0;
    udpservice *udpFrameRcv=NULL;
    QThread *udpThread=NULL;
    udpservice *udpDebugRcv=NULL;
    QThread *udpDebugThread=NULL;
    DM8127Ftp_Service* ftp=NULL;
    QThread *ftpThread=NULL;
    dm8127scan_service* cameraSearch=NULL;
    QThread *cameraSearchThread=NULL;


    NetworkStr network=
    {
        {
            "Network/IPAddress",
            "Network/Netmask",
            "Network/Gateway",
            "Network/Dns",
            "Network/http_port",
            "Network/https_port",
            "Network/dhcp_enable",
            "Network/dhcp_config",
            "Network/ntp_server",
            "Network/ntp_timezone",
            "Network/MAC",
            "Ftp/ServerIP",
            "Ftp/Port",
            "Ftp/Username",
            "Ftp/Passwork",
            "Ftp/Foldername",
            "Ftp/pid",
            "Ftp/rftpenable",
            "Ftp/fileformat",
            "RTSP/Url",
            "RTSP/Multicast",
            "RTSP/mtype",
            {"RTSP/Port1","RTSP/Port2","RTSP/Port3","RTSP/Port4","RTSP/Port5"},
            "RTSP/size_port",
            "RTSP/rtspenable",
            "RTSP/change",

        },
        {
            "192.168.1.224",
            "255.255.255.0",
            "192.168.1.254",
            "192.168.1.1",
            0,
            0,
            0,
            0,
            "",
            0,
            "",
            "192.168.1.1",
            "21",
            "ftpuser",
            "9999",
            "default_folder",
            0,
            0,
            0,
            "rtsp://192.168.1.224:8557/PSIA/Streaming/channels/2?videoCodecType=H.264",
            0,
            0,
            {0,0,0,0,0},
            0,
            0,
            0,
        }
    };
    QSettings* networkconfig=NULL;
    QString networkconfigpath="./system/network.ini";
};

#endif // NETWORK_H
