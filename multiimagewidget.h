#ifndef MULTIIMAGEWIDGET_H
#define MULTIIMAGEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QStackedLayout>
#include <QDebug>
#include <QResizeEvent>
#include "imagewidget.h"

typedef enum
{
    SCALE_NONE=0,
    SCALE_HEIGHT,
    SCALE_WIDTH
}IMGSCALE_TYPE;

class MultiImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MultiImageWidget(QWidget *parent = 0);
    ~MultiImageWidget();
    void resizeImageCount(int);
    void setImage(QImage,int ,qreal,qreal);
    void setImage(QImage,int,IMGSCALE_TYPE type =SCALE_NONE);
    void setImageVisible(bool,int);
    void clearImage(int);
    qreal ratio(){return (qreal)widgetWidth/(qreal)widgetHeight;}
    const QImage getImage(int index);
signals:

public slots:
private:
    QStackedLayout* mainLayout;
    int imgCnt;
    int widgetWidth=1;
    int widgetHeight=1;
    virtual void resizeEvent(QResizeEvent *event);

};

#endif // MULTIIMAGEWIDGET_H
