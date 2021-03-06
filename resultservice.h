#ifndef RESULTSERVICE_H
#define RESULTSERVICE_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class resultService : public QObject
{
    Q_OBJECT
public:
    explicit resultService(QObject *parent = 0);
    QAbstractSocket::SocketState socketState(){return resultSocket->state();}
    void connectSocket(QString ip,int port);
    void disconnectSocket(){isRun=false;retryTimer.stop(); resultSocket->abort();}
signals:
    void resultData(QByteArray ba);
    void sendToLog(QString);
    void resultTimeout();
public slots:
    void readData();
    void connectSuccessfully();
    void reportError(QAbstractSocket::SocketError err){qDebug()<<"error: "<<err;}
    void disconnectfromServer();
    void retryConnection();
private:
    QTcpSocket* resultSocket;
    QString hostIP;
    int hostPort;
    bool isRun=false;
    QTimer retryTimer;
    QTimer readTimer;
};

#endif // RESULTSERVICE_H
