#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QScrollBar>
#include <QThread>
#include "ui_serialport.h"
//#include "console.h"
#include "spconsole.h"

namespace Ui {
class SerialPort;
}

class SerialPort : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPort(QWidget *parent = 0);
    ~SerialPort();
    Ui::SerialPort* SerialPort_Ui(){return ui;}

private slots:
    void on_openSPButton_clicked();
    void SerialPort_ReadData(QString);
    void SerialPort_Status(QString);
    void on_clearButton_clicked();
    void on_closeSPButton_clicked();
    void SerialPort_Init();
    void udpDataRcv(QByteArray);

    void on_DebugModeSelectBox_activated(int index);

private:
    Ui::SerialPort *ui;
    QSerialPort *P_SerialPort;
    QThread *SPConsoleThread;
    SPConsole *spconsole;
signals:
    void SerialPort_Open(int);
    void SerialPort_Close();
    void setDebugMode(int);
protected:
    void closeEvent(QCloseEvent*event);
};

#endif // SERIALPORT_H
