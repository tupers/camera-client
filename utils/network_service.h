#ifndef NETWORK_SERVICE_H
#define NETWORK_SERVICE_H

#include <QObject>
#include <QTcpSocket>
#include <QHostInfo>

typedef struct
{
    char cmd;
    int ackCmd;
    int cmdValueLen;
    char cmdValue[0];
}netMsg;

#define MSG_TIMEOUT 3000

#define MSG_SOK         0
#define MSG_SFAIL       -1
#define MSG_OVERTIME    -2
#define MSG_NETERROR    -3
#define MSG_BUFOVERFLOW -4

#define ACKCMD_SOK      0
#define ACKCMD_SFAIL    -1

#define DEFAULT_PATH "nand"

class network_service : public QObject
{
    Q_OBJECT
public:
    explicit network_service(QObject *parent = nullptr);
    int SendMsg(QTcpSocket* socket,netMsg*msg,int len,netMsg*ackMsg,int* acklen);
    QString getLocalIP();
signals:
    void sendToLog(QString);
public slots:
};

#endif // NETWORK_SERVICE_H
