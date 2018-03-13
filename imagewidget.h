#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPainter>

class ImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageWidget(QWidget *parent = 0);
    void setImage(QImage src,qreal hr=1.0,qreal vr=1.0);
    void clearImage(){img.fill(QColor(0,0,0,0));update();}
    qreal getHRatio(){return HRatio;}
    qreal getVRatio(){return VRatio;}
    QSize getSize(){return imgSize;}
    const QImage getImage(){return img;}
protected:
    void paintEvent(QPaintEvent * event);
private:
    QImage img;
    qreal HRatio=1.0;
    qreal VRatio=1.0;
    QSize imgSize=QSize(1,1);
signals:

public slots:
};

#endif // IMAGEWIDGET_H
