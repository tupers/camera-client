#ifndef FTP_SERVICE_H
#define FTP_SERVICE_H

#include <QObject>
#include <QTcpServer>
#include <QTimer>
#include <QMap>
#include "network_service.h"

#define MAX_USERNAME 32
#define MAX_PASSWD 32
#define MIN_USERNAME 4
#define MIN_PASSWD 4

#define FTP_TIMEOUT 3000

#define MAX_FILE_SIZE 160

#define MAGIC_HEADER1 0x159753
#define MAGIC_HEADER2 0x456183

#define DIR_TYPE 0
#define FILE_TYPE 1
#define FILE_NAME_MAX_SIZE 32

typedef struct __FILE_INFO
{
    unsigned int type;
    char name[FILE_NAME_MAX_SIZE];
    unsigned int size;
}FileInfo;

typedef struct __FILE_LIST
{
    unsigned int dirCount;
    FileInfo finfo[0];
}FileList;

typedef enum
{
    FTP_LOGIN=0,
    FTP_CLOSE,
    FTP_LIST,
    FTP_PUT,
    FTP_GET,
    FTP_DELETE
}FTP_CMD;

typedef struct
{
    int isFirst;//the first recive data should be header 0:false 1:true
    FTP_CMD cmd;
    int size;//used only when recieve list or get ;
    int curSize;//used only when recieve list or get;

}Ftp_RemoteManager;

typedef enum
{
    FTP_ACK_SOK=0,
    FTP_ACK_SFAIL
}FtpAck_Status;

typedef struct
{

    unsigned int magic1;
    unsigned int magic2;
    unsigned int cmd;
    unsigned int len;
    unsigned int pad;
    int status;
}Ftp_Header;

typedef struct Ftp_Ack
{
    int status;
    unsigned int len;
}Ftp_Ack;

typedef struct
{
    unsigned int port;
    char ip[20];
    char filepath[MAX_FILE_SIZE];
    unsigned int fileSize;
}FTP_File;



typedef struct
{
    char UserName[MAX_USERNAME+1];
    char Passwd[MAX_PASSWD+1];

}Ftp_Account;

enum FTP_STATE
{
    FTP_DISCONNECTED=0,
    FTP_CONNECTED,
};

class ftp_service : public network_service
{
    Q_OBJECT
public:
    ftp_service();
    ~ftp_service();
    FTP_STATE ftpStatus(){return m_eStatus;}
    QString getCurrentDir(){return m_strCurDir;}
public slots:
    void initFtp();
    void deinitFtp();
    void loginFtp(QString ip,int port,QString username,QString password);
    void closeFtp();
    bool del(QString name);
    bool get(QString name);
    bool put(QString name,QByteArray ba);
    bool list(){return list(m_strCurDir);}
    bool list(QString name);
    void readRemoteData();
private:
    bool sendCMD(FTP_CMD cmd, void* data, int size);
    bool checkAccount();
    void abortConnection(QTcpSocket*);
    bool processData(FTP_CMD,QByteArray);
    bool m_bInit=false;
    QTcpSocket *m_hCmdSocket=NULL;
    QTcpServer *m_hdataServer=NULL;
    QString m_strUserName;
    QString m_strPassword;
    QString m_strRemoteIP;
    int m_nRemotePort;
    int m_nDataPort;
    FTP_STATE m_eStatus=FTP_DISCONNECTED;
    QTimer* m_hTimer=NULL;

    QByteArray m_arrPut;
    QByteArray m_arrGet;
    QMap<QTcpSocket*,Ftp_RemoteManager> m_mapRemote;
    QString m_strCurDir=DEFAULT_PATH;

signals:
    //void receiveFinished(FTP_CMD,QByteArray*);
    void receiveData(QByteArray);
    void receiveFileList(QByteArray);

};

#endif // FTP_SERVICE_H
