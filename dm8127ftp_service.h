#ifndef DM8127FTP_SERVICE_H
#define DM8127FTP_SERVICE_H

#include <QObject>
#include <QtNetwork/QtNetwork>
#include "ftp_cmdlist.h"
#include "ftpdata.h"

typedef struct _Ftp_Save_INFO
{
    FILE* ftpFile=NULL;
    QString ftpFileName="";
    QString FilePath="";
    int ftpFileSize=0;
    int ftpFileValid=0;
}ftpSaveInfo;

class DM8127Ftp_Service : public QObject
{
    Q_OBJECT
public:
    explicit DM8127Ftp_Service(QObject *parent = 0);
    ~DM8127Ftp_Service();
    int SendServerMsg(QTcpSocket *ClientSocket, EzFtp_NetMsg *msg, int len, EzFtp_NetMsg *ackMsg, int32_t *acklen);
    Server_STATE getState(){return serverState;}
    QString getLocalIP(QString);
signals:
    void sendToLog(QString);
    void listInfo(FileList*);
    void getData(QByteArray);
public slots:
    void connectToHost(QString, int, QString username, QString password);
    void loginService();
    bool loginAccountCheck(QString username,QString password);
    void list(QString path=DEFAULT_PATH);
    void get(QString);
    void put(QString, QByteArray*);
    void newConnectionService();
    void remoteDataRcv(int,QByteArray);
    void close();

    QString getCurrentDir(){return currentDir;}
private:
    QTcpSocket *cmdSocket=NULL;
    QTcpServer *dataServer=NULL;
    QString userName;
    QString passWord;
    Server_STATE serverState=Server_Loggedin;
    QString remoteip;
    QString remoteport;
    QString currentDir;
    unsigned short dataPort=0;
    QByteArray* sendData;
    int putFtpFlag=0;
};

#endif // DM8127FTP_SERVICE_H
