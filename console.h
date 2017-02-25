#ifndef CONSOLE_H
#define CONSOLE_H
#include <QPlainTextEdit>
#include <QDebug>

class Console : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit Console(QWidget *parent = 0);
    void setwriteEnable(bool bo){writeEnable=bo;}
signals:
    void getData(const QByteArray &data);
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
private:
    bool writeEnable;
};

#endif // CONSOLE_H
