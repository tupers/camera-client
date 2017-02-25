#ifndef SPCONSOLESLOT_H
#define SPCONSOLESLOT_H
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
//#include "serialport.h"

class SPConsole:public QObject
{
    Q_OBJECT
public:
    explicit SPConsole(QObject*parent=0);
public slots:
    void SP_Open(int);
    void SP_ReadData();
    void SP_WriteData(const QByteArray);
    void SP_Close();
    bool SP_isOpen(){return SP_isOpenflag;}
private:
    QSerialPort *P_SerialPort;
    bool SP_isOpenflag=false;

signals:
    void SP_ReadDataSignal(QString);
    void SP_StatusSignal(QString);

};

#endif // SPCONSOLESLOT_H
