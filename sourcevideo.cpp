#include "sourcevideo.h"
#include "ui_sourcevideo.h"

SourceVideo::SourceVideo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SourceVideo)
{
    ui->setupUi(this);
    this->setFixedSize(1280,720);
}

SourceVideo::~SourceVideo()
{
    delete ui;
}

void SourceVideo::clearImage()
{
//    ui->inputLabel->clear();
    ui->inputwidget->clearImage();
}

void SourceVideo::setImage(QImage image)
{
    if (image.height()>0)
    {
//        QPixmap pix = QPixmap::fromImage(image);
//        ui->inputLabel->setPixmap(pix);
        ui->inputwidget->setImage(image);
    }
}

void SourceVideo::setImage(QImage image, QRect rectl, QRect rectc, QRect rectr)
{
    if(image.height()>0)
    {
        ui->inputwidget->setSearchRect(rectl,rectc,rectr);
        ui->inputwidget->setImage(image);
    }
}

void SourceVideo::closeEvent(QCloseEvent *event)
{
    emit SourceVideoClose();
}
