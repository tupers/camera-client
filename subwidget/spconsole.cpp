#include "spconsole.h"

SPConsole::SPConsole(QObject *parent):QObject(parent)
{

}
void SPConsole::SP_Open(int comindex)
{
    if(SP_isOpenflag)
    {
        emit SP_StatusSignal(QString("The %1 is already open, need to close it first").arg(P_SerialPort->portName()));
    }
    else
    {
    P_SerialPort = new QSerialPort;
    P_SerialPort->setPortName(QSerialPortInfo::availablePorts()[comindex].portName());
    P_SerialPort->setBaudRate(115200);
    P_SerialPort->setDataBits(QSerialPort::Data8);
    P_SerialPort->setParity(QSerialPort::NoParity);
    P_SerialPort->setStopBits(QSerialPort::OneStop);
    P_SerialPort->setFlowControl(QSerialPort::NoFlowControl);
    P_SerialPort->setReadBufferSize(1024);
    connect(P_SerialPort,SIGNAL(readyRead()),this,SLOT(SP_ReadData()),Qt::DirectConnection);
    if(P_SerialPort->open(QIODevice::ReadWrite))
    {
        SP_isOpenflag=true;
        emit SP_StatusSignal(QString("Open %1 successfully").arg(P_SerialPort->portName()));
    }
    else
        emit SP_StatusSignal(QString("Open %1 failed").arg(P_SerialPort->portName()));
    }

}
void SPConsole::SP_ReadData()
{
    QByteArray data=P_SerialPort->readAll();
    emit SP_ReadDataSignal(QString(data));
}
void SPConsole::SP_WriteData(const QByteArray data)
{
    P_SerialPort->write(data);
}
void SPConsole::SP_Close()
{
    if(SP_isOpenflag)
    {
    P_SerialPort->close();
    SP_isOpenflag = false;
    emit SP_StatusSignal(QString("Close %1 successfully").arg(P_SerialPort->portName()));
    delete P_SerialPort;
    }
    else
        emit SP_StatusSignal(QString("There is no activated port."));
}
