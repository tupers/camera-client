#ifndef DM8127SCAN_SERVICE_H
#define DM8127SCAN_SERVICE_H

#include <QObject>
#include <QtNetwork>
#include <time.h>
#include "ezstream.h"

class dm8127scan_service : public QObject
{
    Q_OBJECT
public:
    explicit dm8127scan_service(QObject *parent = 0);
    ~dm8127scan_service();

    bool check(const NetCamScanData*);
signals:
    void findDevice(int id,QString ip);
    void sendToLog(QString);
public slots:
    void scan();
    void dataRecv();
private:
    QUdpSocket *BroadcastSocket;
    int m_nRandom;
};

#endif // DM8127SCAN_SERVICE_H
