#ifndef FTPDATA_H
#define FTPDATA_H

#include <QObject>
#include <QtNetwork/QtNetwork>
#include "ftp_cmdlist.h"
class ftpData : public QObject
{
    Q_OBJECT
public:
    explicit ftpData(QTcpSocket* socket,QByteArray*data=NULL, QObject *parent = 0);
signals:
    void dataFinished(int,QByteArray);
    void putProgress(int);
public slots:
    void readData();
    void diconnectservice();
private:
    int cmd=0;
    int size=0;
    int currentsize=0;
    int m_nPutSize=0;
    QTcpSocket* dataSocket;
    QByteArray rcvData;
    QByteArray* sendData=NULL;

};

#endif // FTPDATA_H
