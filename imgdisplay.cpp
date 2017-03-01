#include "imgdisplay.h"

imgDisplay::imgDisplay(QWidget *parent) : QWidget(parent)
{
    img.fill(QColor(0,0,0,0));
}

void imgDisplay::setImage(QImage srcimg)
{
    img=QPixmap::fromImage(srcimg);
    update();
}

void imgDisplay::clearImage()
{
    img.fill(QColor(0,0,0,0));
    isSearchRect=false;
    update();
}

void imgDisplay::setSearchRect(QRect a, QRect b, QRect c)
{
    rect_L=a;
    rect_C=b;
    rect_R=c;
    isSearchRect=true;
}

void imgDisplay::setDefaultSearchArea(int top, int bottom, int left, int right)
{
    topLine=QLine(0,top,this->width(),top);
    bottomLine=QLine(0,bottom,this->width(),bottom);
    leftLine=QLine(left,0,left,this->height());
    rightLine=QLine(right,0,right,this->height());
}

void imgDisplay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,img);
    if(!(topLine.isNull()||bottomLine.isNull()||leftLine.isNull()||rightLine.isNull()))
    {
        QPen linePen(Qt::red);
        painter.setPen(linePen);
        painter.drawLine(topLine);
        painter.drawLine(bottomLine);
        painter.drawLine(leftLine);
        painter.drawLine(rightLine);
    }
    if(isSearchRect)
    {
        isSearchRect=false;
        QPen pen(Qt::white);
        painter.setPen(pen);
        painter.drawRect(rect_L);
        painter.drawRect(rect_C);
        painter.drawRect(rect_R);
    }
}

//void imgDisplay::resizeEvent(QResizeEvent *event)
//{

//   // resize(event->size());
//}
