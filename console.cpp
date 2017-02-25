#include "console.h"



Console::Console(QWidget *parent)
    : QPlainTextEdit(parent)
{
    writeEnable=true;
    document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);
}
void Console::keyPressEvent(QKeyEvent *e)
{
    if(writeEnable==false)
        return;
    switch (e->key()) {
    case Qt::Key_Backspace:
        emit getData(QString("\b").toLocal8Bit());
        this->textCursor().deletePreviousChar();
        break;
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    default:
        emit getData(e->text().toLocal8Bit());
    }
}

void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}
