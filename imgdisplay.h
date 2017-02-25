#ifndef IMGDISPLAY_H
#define IMGDISPLAY_H

#include <QWidget>
#include <QPainter>

class imgDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit imgDisplay(QWidget *parent = 0);
private:
    QPixmap img;
    QRect rect_L;
    QRect rect_C;
    QRect rect_R;
    QLine topLine;
    QLine bottomLine;
    QLine leftLine;
    QLine rightLine;
    bool isSearchRect=false;
signals:

public slots:
    void setImage(QImage srcimg);
    void clearImage();
    void setSearchRect(QRect a,QRect b,QRect c);
    void setDefaultSearchArea(int top,int bottom,int left,int right);
protected:
    void paintEvent(QPaintEvent * event);
};

#endif // IMGDISPLAY_H
