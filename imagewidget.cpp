#include "imagewidget.h"

ImageWidget::ImageWidget(QWidget *parent) : QWidget(parent)
{
    img.fill(QColor(0,0,0,0));
}

void ImageWidget::setImage(QImage src, qreal hr, qreal vr)
{
    HRatio = hr;
    VRatio = vr;
    img=src.copy();
    imgSize=img.size();
    update();
}

void ImageWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.scale(HRatio,VRatio);
    painter.drawImage(0,0,img);
}
