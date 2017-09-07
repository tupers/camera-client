#include "alg_gvss.h"

void updateResultImg_GVSS(QPainter* painter,QVector<ALGResultStr>* result,QVector<ALGConfigStr>* config)
{
    int i;
    //step0. draw ROI and edge
    QPen pen;
    pen.setWidth(3);
    pen.setColor(QColor(150,150,0,200));
    painter->setPen(pen);
    for(i=0;i<config->count();i++)
    {
        ALGConfigStr tempconfig = config->at(i);
        if(tempconfig.name=="frameLeftRatio"||tempconfig.name=="frameRightRatio")
        {
            float pos = *(float*)tempconfig.value*1280;
            pos = (int)pos/32;
            pos = (int)pos*32;
            painter->drawLine(pos,0,pos,720);
        }
        else if(tempconfig.name=="frameTopRatio"||tempconfig.name=="frameBottomRatio")
        {
            float pos = *(float*)tempconfig.value*720;
            pos = (int)pos/32;
            pos = (int)pos*32;
            painter->drawLine(0,pos,1280,pos);
        }
    }

    //step1. find block num
    int blockNum=0;
    for(i=0;i<result->count();i++)
    {
        ALGResultStr temp = result->at(i);
        if(temp.name=="Block_Num")
        {
            QLabel* labelName = (QLabel*)temp.ui->itemAt(1)->widget();
            blockNum = labelName->text().toInt();
            break;
        }
    }
    //step2. draw valid block
    pen.setColor(QColor(223,50,0,200));
    painter->setPen(pen);
    int code_top,code_bottom,code_left,code_right;
    for(i=0;i<result->count();i++)
    {
        if(blockNum==0)
            break;
        ALGResultStr temp = result->at(i);
        if(temp.paramType==PARAM_BLOCK)
        {
            QLabel* x = (QLabel*)temp.ui->itemAt(2)->widget();
            QLabel* y = (QLabel*)temp.ui->itemAt(3)->widget();
            painter->drawLine(x->text().toInt()-7,y->text().toInt(),x->text().toInt()+7,y->text().toInt());
            painter->drawLine(x->text().toInt(),y->text().toInt()-7,x->text().toInt(),y->text().toInt()+7);
            blockNum--;
        }
        else if(temp.name=="top")
        {
            QLabel* label = (QLabel*)temp.ui->itemAt(1)->widget();
            code_top=label->text().toInt();
        }
        else if(temp.name=="bottom")
        {
            QLabel* label = (QLabel*)temp.ui->itemAt(1)->widget();
            code_bottom=label->text().toInt();
        }
        else if(temp.name=="left")
        {
            QLabel* label = (QLabel*)temp.ui->itemAt(1)->widget();
            code_left=label->text().toInt();
        }
        else if(temp.name=="right")
        {
            QLabel* label = (QLabel*)temp.ui->itemAt(1)->widget();
            code_right=label->text().toInt();
        }

    }
    painter->drawRect(code_left,code_top,code_right-code_left,code_bottom-code_top);

}
