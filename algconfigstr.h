#ifndef ALGCONFIGSTR_H
#define ALGCONFIGSTR_H
#include <QString>
#include <QHBoxLayout>
#include <QLabel>
#include "lineedit.h"
enum VALUE_TYPE
{
    VALUE_INT=0,
    VALUE_UINT,
    VALUE_SHORT,
    VALUE_USHORT,
    VALUE_CHAR,
    VALUE_UCHAR,
    VALUE_FLOAT,
    VALUE_DOUBLE
};


typedef struct
{
    QHBoxLayout* valueLayout;
    QLabel* valueLabel;
    LineEdit* valuelineEdit;
}ALGConfig_ui;

typedef struct
{
    QHBoxLayout* valueLayout;
    QLabel* nameLabel;
    QLabel* valueLabel;
}ALGResult_ui;

typedef struct
{
    QVBoxLayout* containerLayout;
    QLabel* titleLabel;
    int posx;
    int posy;
}ALGParamContainer;

typedef struct
{
    QString name;
//    int size;
    VALUE_TYPE type;
    void* value;
    ALGConfig_ui ui;
}ALGConfigStr;
typedef struct
{
    QString name;
//    int size;
    VALUE_TYPE type;
    void* value;
    ALGResult_ui ui;
}ALGResultStr;
enum ALG_TYPE
{
    ALG_SDS=0,
    ALG_GVSS
};

#endif // ALGCONFIGSTR_H
