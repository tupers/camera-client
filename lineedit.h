#ifndef LINEEDIT_H
#define LINEEDIT_H
#include <QLineEdit>
#include <QDebug>
#include <QSlider>

class LineEdit:public QLineEdit
{
    Q_OBJECT
public:
    explicit LineEdit(QWidget *parent=0);
    void editValue(int);
private slots:
    void setValue(int);
    void LineEditAfterEdit();
    void sendValueSignal();
signals:
    void valueFinished(int);
    void valueUpdateStr(int);
    void valueUpdateStr(QString);
};

#endif // LINEEDIT_H
