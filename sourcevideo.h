#ifndef SOURCEVIDEO_H
#define SOURCEVIDEO_H

#include <QWidget>
#include <QImage>

namespace Ui {
class SourceVideo;
}

class SourceVideo : public QWidget
{
    Q_OBJECT

public:
    explicit SourceVideo(QWidget *parent = 0);
    ~SourceVideo();
    void clearImage();

private:
    Ui::SourceVideo *ui;
private slots:
    void setImage(QImage);
protected:
    void closeEvent(QCloseEvent*event);
signals:
    void SourceVideoClose();
};

#endif // SOURCEVIDEO_H
