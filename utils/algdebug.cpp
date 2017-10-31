#include "algdebug.h"

algDebug::algDebug(QObject *parent) : QObject(parent)
{

}

algDebug::~algDebug()
{
}

void algDebug::loadLocalImage(QString adr)
{
    QImage src(adr);
    src=src.convertToFormat(QImage::Format_Grayscale8);
    updateImg(src);

    EzImgFrameHeader header;
    header.height = src.height();
    header.width = src.width();
    header.pitch = src.bytesPerLine();
    header.infoSize = 0;
    m_Ba.append((char*)&header,sizeof(EzImgFrameHeader));
    m_Ba.append((char*)src.bits(),src.byteCount());
    emit sendFrameToServer(m_Ba);
}

void algDebug::updateImg(QImage img)
{
    m_hDisplayWidget->setImage(img,0,SCALE_WIDTH);
}


