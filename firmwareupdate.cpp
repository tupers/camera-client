#include "firmwareupdate.h"
#include "ui_firmwareupdate.h"

FirmwareUpdate::FirmwareUpdate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FirmwareUpdate)
{
    ui->setupUi(this);
}

FirmwareUpdate::~FirmwareUpdate()
{
    delete ui;
}

void FirmwareUpdate::on_comboBox_activated(int index)
{
    updateoption=index;
}

void FirmwareUpdate::on_pushButton_clicked()
{
    emit updateOption(updateoption);
}
