#include "multiimagewidget.h"

MultiImageWidget::MultiImageWidget(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QStackedLayout;
    this->setLayout(mainLayout);
    mainLayout->setStackingMode(QStackedLayout::StackAll);
    ImageWidget* temp = new ImageWidget();
    mainLayout->addWidget(temp);
    imgCnt = mainLayout->count();
}

MultiImageWidget::~MultiImageWidget()
{
    int i;
    for(i=0;i<imgCnt;i++)
    {
        ImageWidget* temp = (ImageWidget*)mainLayout->widget(mainLayout->count()-1);
        mainLayout->removeWidget(temp);
        delete temp;
    }
    delete mainLayout;
}

void MultiImageWidget::resizeImageCount(int size)
{
    int i;
    if(imgCnt<size)
    {
        for(i=0;i<size-imgCnt;i++)
        {
            ImageWidget* temp = new ImageWidget();
            mainLayout->addWidget(temp);
        }
    }
    else if(imgCnt>size)
    {
        for(i=0;i<imgCnt-size;i++)
        {
            ImageWidget* temp = (ImageWidget*)mainLayout->widget(mainLayout->count()-1);
            mainLayout->removeWidget(temp);
            delete temp;
        }
    }
    imgCnt = size;
}

void MultiImageWidget::setImage(QImage img, int index, qreal hr, qreal vr)
{
    if(index>=imgCnt)
        return;
    ImageWidget* temp = (ImageWidget*)mainLayout->widget(index);
    temp->setImage(img,hr,vr);
}

void MultiImageWidget::setImage(QImage img, int index, IMGSCALE_TYPE type)
{
    if(index>=imgCnt)
        return;
    ImageWidget* temp = (ImageWidget*)mainLayout->widget(index);
    if(type==SCALE_NONE)
    {
        temp->setImage(img);
    }
    else if(type==SCALE_HEIGHT)
    {
        qreal vr = (qreal)widgetHeight/(qreal)img.size().height();
//        qreal hr = (qreal)widgetWidth/((qreal)img.size().width()*vr);
//        qDebug()<<widgetHeight<<vr*img.size().height();
//        qDebug()<<widgetWidth<<hr*vr*img.size().width();
        temp->setImage(img,vr,vr);
    }
    else if(type==SCALE_WIDTH)
    {
        qreal hr = (qreal)widgetWidth/(qreal)img.size().width();
//        qreal vr = (qreal)widgetHeight/((qreal)img.size().height()*hr);
        temp->setImage(img,hr,hr);
    }
}

void MultiImageWidget::setImageVisible(bool state, int index)
{
    if(index>=imgCnt)
        return;
    ImageWidget* temp = (ImageWidget*)mainLayout->widget(index);
    temp->setVisible(state);
}

void MultiImageWidget::clearImage(int index)
{
    if(index>=imgCnt)
        return;
    ImageWidget* temp = (ImageWidget*)mainLayout->widget(index);
    temp->clearImage();

}

void MultiImageWidget::resizeEvent(QResizeEvent *event)
{
    widgetHeight = event->size().height();
    widgetWidth = event->size().width();
}
