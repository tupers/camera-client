#ifndef ALGDEBUG_H
#define ALGDEBUG_H

#include <QObject>
#include "ezstream.h"
#include "multiimagewidget.h"

class algDebug : public QObject
{
    Q_OBJECT
public:
    explicit algDebug(QObject *parent = nullptr);
    ~algDebug();
    void setDisplayWidget(MultiImageWidget* imgWidget){m_hDisplayWidget=imgWidget;}
    void loadLocalImage(QString);
private:
    void updateImg(QImage img);
    MultiImageWidget* m_hDisplayWidget;
    QByteArray m_Ba;
    //QVector<QImage> m_vecImg;
    //int m_nIdx=0;
signals:
    void sendFrameToServer(QByteArray);

public slots:
};

#endif // ALGDEBUG_H
