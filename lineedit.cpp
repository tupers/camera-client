#include "lineedit.h"

LineEdit::LineEdit(QWidget *parent)
    :QLineEdit(parent)
{
//    this->setAlignment(Qt::AlignCenter);
    connect(this,SIGNAL(editingFinished()),this,SLOT(LineEditAfterEdit()));
}

void LineEdit::editValue(int value)
{
    this->setText(QString::number(value,10));
    emit valueFinished(value);
}

void LineEdit::setValue(int value)
{
    this->setText(QString::number(value,10));
}

void LineEdit::LineEditAfterEdit()
{
    emit valueFinished(this->text().toInt());
    emit valueUpdateStr(this->text().toInt());
    emit valueUpdateStr(this->text());
}

void LineEdit::sendValueSignal()
{
    QSlider * tempslider = qobject_cast<QSlider*>(sender());
    emit valueUpdateStr(tempslider->value());
}
