#include "serialport.h"
#include "spconsole.h"


SerialPort::SerialPort(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialPort)
{
    ui->setupUi(this);
    ui->SPTextEdit->setEnabled(false);
    SPConsoleThread = new QThread;
    spconsole = new SPConsole;
    spconsole->moveToThread(SPConsoleThread);
    connect(spconsole,SIGNAL(SP_StatusSignal(QString)),this,SLOT(SerialPort_Status(QString)));
    connect(this,SIGNAL(SerialPort_Open(int)),spconsole,SLOT(SP_Open(int)));
    connect(spconsole,SIGNAL(SP_ReadDataSignal(QString)),this,SLOT(SerialPort_ReadData(QString)));
    connect(ui->SPTextEdit,SIGNAL(getData(QByteArray)),spconsole,SLOT(SP_WriteData(const QByteArray)));
    connect(this,SIGNAL(SerialPort_Close()),spconsole,SLOT(SP_Close()));
    SPConsoleThread->start();
    //    this->show();
}
SerialPort::~SerialPort()
{
    SPConsoleThread->exit();
    delete spconsole;
    delete SPConsoleThread;
    delete ui;
}
void SerialPort::closeEvent(QCloseEvent*event)
{
    Q_UNUSED(event);
    ui->SPTextEdit->clear();
    if(spconsole->SP_isOpen())
        emit SerialPort_Close();
}
void SerialPort::SerialPort_Init()
{
    while(ui->SPSelectBox->count()>0)
        ui->SPSelectBox->removeItem(ui->SPSelectBox->count()-1);
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->SPSelectBox->addItem(info.portName());
    }
    if(ui->DebugModeSelectBox->currentIndex()==1)
    {
        if(ui->SPSelectBox->currentIndex()==-1)
        {
            ui->openSPButton->setEnabled(false);
            ui->clearButton->setEnabled(false);
            ui->closeSPButton->setEnabled(false);
        }
        else
        {
            ui->openSPButton->setEnabled(true);
            ui->clearButton->setEnabled(true);
            ui->closeSPButton->setEnabled(true);
        }
    }
    else
    {
        ui->SPSelectBox->setEnabled(false);
        ui->openSPButton->setEnabled(false);
        ui->clearButton->setEnabled(false);
        ui->closeSPButton->setEnabled(false);
    }
    ui->SPStatusLabel->setText("");
}

void SerialPort::udpDataRcv(QByteArray data)
{
    ui->SPTextEdit->appendPlainText(QString(data));
}

void SerialPort::SerialPort_Status(QString status)
{
    ui->SPStatusLabel->setText(status);
    qDebug()<<status;
}

void SerialPort::on_openSPButton_clicked()
{
    ui->SPTextEdit->setEnabled(true);
    emit SerialPort_Open(ui->SPSelectBox->currentIndex());
}
void SerialPort::SerialPort_ReadData(QString data)
{

    ui->SPTextEdit->insertPlainText(data);
    ui->SPTextEdit->verticalScrollBar()->setValue(ui->SPTextEdit->verticalScrollBar()->maximum());
}
void SerialPort::on_clearButton_clicked()
{
    ui->SPTextEdit->clear();
}


void SerialPort::on_closeSPButton_clicked()
{
    ui->SPTextEdit->setEnabled(false);
    emit SerialPort_Close();
}


void SerialPort::on_DebugModeSelectBox_activated(int index)
{
    ui->SPTextEdit->clear();

    if(index==0)
    {
        ui->SPSelectBox->setEnabled(false);
        ui->openSPButton->setEnabled(false);
        ui->clearButton->setEnabled(false);
        ui->closeSPButton->setEnabled(false);
        ui->SPTextEdit->setEnabled(false);
    }
    else if(index==1)
    {
        ui->SPTextEdit->setwriteEnable(true);
        ui->SPSelectBox->setEnabled(true);
        if(ui->SPSelectBox->currentIndex()==-1)
        {
            ui->openSPButton->setEnabled(false);
            ui->clearButton->setEnabled(false);
            ui->closeSPButton->setEnabled(false);
        }
        else
        {
            ui->openSPButton->setEnabled(true);
            ui->clearButton->setEnabled(true);
            ui->closeSPButton->setEnabled(true);
        }
        ui->SPTextEdit->setEnabled(false);
    }
    else
    {
        ui->SPTextEdit->setwriteEnable(false);
        ui->SPTextEdit->setEnabled(true);
        ui->SPSelectBox->setEnabled(false);
        ui->openSPButton->setEnabled(false);
        ui->clearButton->setEnabled(false);
        ui->closeSPButton->setEnabled(false);
    }
    emit setDebugMode(index);
}
