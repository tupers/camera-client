#ifndef ALGCONFIGSTR_H
#define ALGCONFIGSTR_H
#include <QString>
#include <QHBoxLayout>
#include <QLabel>
#include "lineedit.h"
typedef enum
{
    VALUE_INT=0,
    VALUE_UINT,
    VALUE_SHORT,
    VALUE_USHORT,
    VALUE_CHAR,
    VALUE_UCHAR,
    VALUE_FLOAT,
    VALUE_DOUBLE
}VALUE_TYPE;

typedef struct
{
    QHBoxLayout* valueLayout;
    QLabel* valueLabel;
    LineEdit* valuelineEdit;
}ALGConfig_ui;

//typedef struct
//{
//    QHBoxLayout* valueLayout;
//    QLabel* nameLabel;
//    QLabel* valueLabel;
//}ALGResult_ui;

typedef struct
{
    QVBoxLayout* containerLayout;
    QWidget* containerWidget;
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
typedef enum
{
    PARAM_LABEL=0,
    PARAM_RECT,
    PARAM_BLOCK,
    PARAM_CIRCLE
}PARAM_TYPE;
typedef struct
{
    QString name;
    PARAM_TYPE paramType;
    int visible;
    VALUE_TYPE type;
    void* value;
    QLayout* ui;
}ALGResultStr;
typedef enum
{
    ALG_SDS=0,
    ALG_GVSS
}ALG_TYPE;



#endif // ALGCONFIGSTR_H
