#include "alg_gvss.h"

void updateResultImg_GVSS(QPainter* painter,QVector<ALGResultStr>* list)
{
    //step1. find block num
    int blockNum=0;
    int i;
    for(i=0;i<list->count();i++)
    {
        ALGResultStr temp = list->at(i);
        if(temp.name=="Block_Num")
        {
            QLabel* labelName = (QLabel*)temp.ui->itemAt(1)->widget();
            blockNum = labelName->text().toInt();
            break;
        }
    }
    //step2. draw valid block
    for(i=0;i<list->count();i++)
    {
        if(blockNum==0)
            break;
        ALGResultStr temp = list->at(i);
        if(temp.paramType==PARAM_BLOCK)
        {
            QLabel* x = (QLabel*)temp.ui->itemAt(2)->widget();
            QLabel* y = (QLabel*)temp.ui->itemAt(3)->widget();
            painter->drawLine(x->text().toInt()-7,y->text().toInt(),x->text().toInt()+7,y->text().toInt());
            painter->drawLine(x->text().toInt(),y->text().toInt()-7,x->text().toInt(),y->text().toInt()+7);
            blockNum--;
        }
    }
}
