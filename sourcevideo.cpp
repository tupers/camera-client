#include "sourcevideo.h"
#include "ui_sourcevideo.h"

SourceVideo::SourceVideo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SourceVideo)
{
    ui->setupUi(this);
    this->setFixedSize(1280,720);
    ui->inputwidget->resizeImageCount(2);
}

SourceVideo::~SourceVideo()
{
    delete ui;
}

void SourceVideo::clearImage()
{
//    ui->inputLabel->clear();
    ui->inputwidget->clearImage(0);
}

void SourceVideo::setImage(QImage image, int index)
{
    if (image.height()>0)
    {
//        QPixmap pix = QPixmap::fromImage(image);
//        ui->inputLabel->setPixmap(pix);
        ui->inputwidget->setImage(image,index);
    }
}

void SourceVideo::closeEvent(QCloseEvent *event)
{
    emit SourceVideoClose();
}
