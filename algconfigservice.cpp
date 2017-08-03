#include "algconfigservice.h"

ALGConfigService::ALGConfigService(QObject *parent) : QObject(parent)
{
    QDir algdir;
    if(algdir.mkdir(algParamDir))
    {
        emit sendToLog("did not find algParam dir, create algParam dir.");
        qDebug()<<"did not find algParam dir, create algParam dir.";
    }
    else
    {
        emit sendToLog("find algParam dir.");
        qDebug()<<"find algParam dir.";
    }


}

ALGConfigService::~ALGConfigService()
{
    clearList(&configList);
    clearList(&resultList);
    clearList(&configContainerList);
    clearList(&resultContainerList);
}

void ALGConfigService::initConfigList(ALG_TYPE type)
{
    currentAlg=type;
    generateConfig(type);
    generateResult(type);
    if(type==ALG_GVSS)
        updateResultImg = updateResultImg_GVSS;
}

void ALGConfigService::clearList(QVector<ALGConfigStr> *list)
{
    int i;
    for(i=0;i<list->count();i++)
    {
        ALGConfigStr temp = list->at(i);
        free(temp.value);
        deleteTag(temp.ui);
    }
    list->clear();
}

void ALGConfigService::clearList(QVector<ALGResultStr> *list)
{
    int i;
    for(i=0;i<list->count();i++)
    {
        ALGResultStr temp = list->at(i);
        free(temp.value);
        deleteTag(temp.ui);
    }
    list->clear();
}

void ALGConfigService::clearList(QVector<ALGParamContainer> *list)
{
    int i;
    for(i=0;i<list->count();i++)
    {
        ALGParamContainer temp = list->at(i);
        //        free(temp.value);
        deleteTag(temp);
    }
    list->clear();
}


void ALGConfigService::setConfig(void *params)
{
    int i;
    int offset=0;
    for(i=0;i<configList.count();i++)
    {
        ALGConfigStr temp = configList.at(i);
        int size = valueSize(temp.type);
        memcpy(temp.value,(char*)params+offset,size);
        offset+=size;
    }
}

void ALGConfigService::getConfig(void *params)
{
    int i;
    int offset=0;
    for(i=0;i<configList.count();i++)
    {
        ALGConfigStr temp = configList.at(i);
        int size = valueSize(temp.type);
        memcpy((char*)params+offset,temp.value,size);
        offset+=size;
    }
}

void ALGConfigService::refreshResult(void *params)
{
    int i;
    int offset=0;
    for(i=0;i<resultList.count();i++)
    {
        ALGResultStr temp = resultList.at(i);
        if(temp.paramType==PARAM_LABEL)
        {
            int size=valueSize(temp.type);
            memcpy(temp.value,(char*)params+offset,size);
            QLabel* label = (QLabel*)temp.ui->itemAt(1)->widget();
            label->setText(value2Text(temp.value,temp.type));
            offset+=size;
        }
        else if(temp.paramType==PARAM_RECT)
        {
            int size = valueSize(temp.type)*4;
            memcpy(temp.value,(char*)params+offset,size);
            QLabel* left = (QLabel*)temp.ui->itemAt(1)->widget();
            left->setText(value2Text(temp.value,temp.type));
            QLabel* top = (QLabel*)temp.ui->itemAt(2)->widget();
            top->setText(value2Text((char*)temp.value+valueSize(temp.type),temp.type));
            QLabel* right = (QLabel*)temp.ui->itemAt(3)->widget();
            right->setText(value2Text((char*)temp.value+2*valueSize(temp.type),temp.type));
            QLabel* bottom = (QLabel*)temp.ui->itemAt(4)->widget();
            bottom->setText(value2Text((char*)temp.value+3*valueSize(temp.type),temp.type));
            offset+=size;
        }
        else if(temp.paramType==PARAM_BLOCK)
        {
            int size = valueSize(temp.type)*2;
            memcpy(temp.value,(char*)params+offset,size);
            QLabel* area = (QLabel*)temp.ui->itemAt(1)->widget();
            area->setText(value2Text(temp.value,temp.type));
            QLabel* x = (QLabel*)temp.ui->itemAt(2)->widget();
            x->setText(value2Text((char*)temp.value+valueSize(temp.type),VALUE_SHORT));
            QLabel* y = (QLabel*)temp.ui->itemAt(3)->widget();
            y->setText(value2Text((char*)temp.value+valueSize(temp.type)+valueSize(VALUE_SHORT),VALUE_SHORT));
            offset+=size;
        }
        else if(temp.paramType==PARAM_CHART)
        {
            int size=valueSize(temp.type);
            memcpy(temp.value,(char*)params+offset,size);
            RTChart* chart = (RTChart*)temp.ui->itemAt(0)->widget();
            chart->updateData(*(float*)temp.value,0);
            offset+=size;
        }

    }
}

void ALGConfigService::saveConfig()
{
    int i;
    for(i=0;i<configList.count();i++)
    {
        ALGConfigStr temp=configList.at(i);
        text2Value(temp.ui.valuelineEdit->text(),temp.value,temp.type);
    }
}

void ALGConfigService::loadConfig()
{
    int i;
    for(i=0;i<configList.count();i++)
    {
        ALGConfigStr temp = configList.at(i);
        temp.ui.valuelineEdit->setText(value2Text(temp.value,temp.type));
    }
}

ALGConfig_ui ALGConfigService::createConfigTag(QString name)
{
    ALGConfig_ui temp;
    temp.valueLayout = new QHBoxLayout;
    temp.valueLabel = new QLabel(name);
    temp.valuelineEdit = new LineEdit;
    temp.valueLabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    temp.valueLayout->addWidget(temp.valueLabel,1);
    temp.valueLayout->addWidget(temp.valuelineEdit,2);
    temp.valueLayout->setAlignment(temp.valuelineEdit,Qt::AlignRight|Qt::AlignVCenter);
    temp.valueLayout->setContentsMargins(10,0,10,0);

    return temp;
}

QLayout *ALGConfigService::createResultLabel(QString name)
{
    QLayout* layout = new QHBoxLayout;
    QLabel* nameLabel = new QLabel(name);
    nameLabel->setStyleSheet("border-style:outset;"
                             "border-top-width:1px;"
                             "border-right-width:1px;"
                             "border-color:rgba(50,50,50,255);"
                             "padding-left:15px;");
    QLabel* valueLabel = new QLabel;
    valueLabel->setStyleSheet("border-style:outset;"
                              "border-top-width:1px;"
                              "border-color:rgba(50,50,50,255);"
                              "padding-left:15px;");
    layout->addWidget(nameLabel);
    layout->addWidget(valueLabel);

    return layout;
}

QLayout *ALGConfigService::createResultRect(QString name)
{
    QLayout* layout = new QHBoxLayout;
    QLabel* nameLabel = new QLabel(name);
    nameLabel->setStyleSheet("border-style:outset;"
                             "border-top-width:1px;"
                             "border-right-width:1px;"
                             "border-color:rgba(50,50,50,255);"
                             "padding-left:15px;");
    QLabel* leftLabel = new QLabel;
    leftLabel->setStyleSheet("border-style:outset;"
                             "border-top-width:1px;"
                             "border-color:rgba(50,50,50,255);"
                             "padding-left:15px;");
    QLabel* topLabel = new QLabel;
    topLabel->setStyleSheet("border-style:outset;"
                            "border-top-width:1px;"
                            "border-color:rgba(50,50,50,255);"
                            "padding-left:15px;");
    QLabel* rightLabel = new QLabel;
    rightLabel->setStyleSheet("border-style:outset;"
                              "border-top-width:1px;"
                              "border-color:rgba(50,50,50,255);"
                              "padding-left:15px;");
    QLabel* bottomLabel = new QLabel;
    bottomLabel->setStyleSheet("border-style:outset;"
                               "border-top-width:1px;"
                               "border-color:rgba(50,50,50,255);"
                               "padding-left:15px;");
    layout->addWidget(nameLabel);
    layout->addWidget(leftLabel);
    layout->addWidget(topLabel);
    layout->addWidget(rightLabel);
    layout->addWidget(bottomLabel);

    return layout;
}

QLayout *ALGConfigService::createResultBlock(QString name)
{
    QLayout* layout = new QHBoxLayout;
    QLabel* nameLabel = new QLabel(name);
    nameLabel->setStyleSheet("border-style:outset;"
                             "border-top-width:1px;"
                             "border-right-width:1px;"
                             "border-color:rgba(50,50,50,255);"
                             "padding-left:15px;");
    QLabel* areaLabel = new QLabel;
    areaLabel->setStyleSheet("border-style:outset;"
                             "border-top-width:1px;"
                             "border-color:rgba(50,50,50,255);"
                             "padding-left:15px;");
    QLabel* xLabel = new QLabel;
    xLabel->setStyleSheet("border-style:outset;"
                          "border-top-width:1px;"
                          "border-color:rgba(50,50,50,255);"
                          "padding-left:15px;");
    QLabel* yLabel = new QLabel;
    yLabel->setStyleSheet("border-style:outset;"
                          "border-top-width:1px;"
                          "border-color:rgba(50,50,50,255);"
                          "padding-left:15px;");
    layout->addWidget(nameLabel);
    layout->addWidget(areaLabel);
    layout->addWidget(xLabel);
    layout->addWidget(yLabel);

    return layout;
}

QLayout *ALGConfigService::createResultChart(RTChart* chart)
{
    QLayout* layout = new QHBoxLayout;
    layout->addWidget(chart);
    return layout;
}

ALGParamContainer ALGConfigService::createResultContainer(int posx, int posy)
{
    ALGParamContainer temp;
    temp.containerWidget = new QWidget;
    temp.containerLayout=new QVBoxLayout;
    temp.containerLayout->setSpacing(0);
    temp.titleLabel=new QLabel;
    temp.titleLabel->setStyleSheet("QLabel"
                                   "{color:white;"
                                   "min-width:0px;"
                                   "max-width:1000px;"
                                   "font:bold 15pt Arial;"
                                   "margin-top:10px;"
                                   "}");
    //    temp.containerLayout->setMargin(20);
    //    temp.titleLabel->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    temp.posx=posx;
    temp.posy=posy;
    temp.containerLayout->addWidget(temp.titleLabel);
    temp.containerWidget->setLayout(temp.containerLayout);
    return temp;
}

ALGParamContainer ALGConfigService::createConfigContainer(int posx, int posy)
{
    ALGParamContainer temp;
    temp.containerWidget = new QWidget;
    temp.containerWidget->setObjectName("containnerWidget");

    temp.containerWidget->setStyleSheet("QWidget#"+temp.containerWidget->objectName()+
                                        "{background-color: rgba(50, 50, 50, 120);"
                                        "margin:20px;}"
                                        "QLabel{color:white;"
                                        "font:bold 13pt Arial;"
                                        "}");
    temp.containerLayout=new QVBoxLayout;
    temp.titleLabel=new QLabel;
    temp.titleLabel->setStyleSheet("QLabel"
                                   "{color:white;"
                                   "min-width:0px;"
                                   "max-width:1000px;"
                                   "border-style:outset;"
                                   "border-bottom-width:1px;"
                                   "border-color:rgba(50,50,50,255);"
                                   "font:bold 15pt Arial;"
                                   "padding-left:15px;"
                                   "margin-top:10px;"
                                   "}");
    temp.containerLayout->setMargin(20);
    //    temp.titleLabel->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    temp.posx=posx;
    temp.posy=posy;
    temp.containerLayout->addWidget(temp.titleLabel);
    temp.containerWidget->setLayout(temp.containerLayout);
    return temp;
}

void ALGConfigService::deleteTag(ALGConfig_ui tag)
{
    delete tag.valueLabel;
    delete tag.valuelineEdit;
    delete tag.valueLayout;
}

void ALGConfigService::deleteTag(QLayout *tag)
{
    QLayoutItem *temp;
    while((temp = tag->takeAt(0))!=0)
        delete temp;
    delete tag;
}

void ALGConfigService::deleteTag(ALGParamContainer tag)
{
    delete tag.titleLabel;
    delete tag.containerLayout;
    delete tag.containerWidget;
}

void ALGConfigService::generateConfig(ALG_TYPE algtype)
{
    clearList(&configList);
    clearList(&configContainerList);

    QFile file(algParamDir+algParamPath(algtype));
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QXmlStreamReader reader(&file);
        while(!reader.atEnd())
        {
            reader.readNext();
            if(reader.isStartElement()&&reader.name()=="Config")
            {
                configSize=0;
                while(!(reader.isEndElement()&&reader.name()=="Config"))
                {
                    if(reader.isStartElement())
                    {
                        //qDebug()<<reader.name();
                        if(reader.name()=="Tag")
                        {
                            QXmlStreamAttributes att = reader.attributes();
                            configContainerList.append(createConfigContainer(att.value("posx").toInt(),att.value("posy").toInt()));
                        }
                        else if(reader.name()=="Title")
                            configContainerList.last().titleLabel->setText(reader.readElementText());
                        else if(reader.name()=="Label")
                        {
                            QXmlStreamAttributes att = reader.attributes();
                            QString labelName = reader.readElementText();
                            VALUE_TYPE type = VALUE_TYPE(att.value("type").toInt());
                            configSize+=valueSize(type);
                            void* pdata = malloc(valueSize(type));
                            configList.append({labelName,type,pdata,createConfigTag(labelName)});
                            configContainerList.last().containerLayout->addLayout(configList.last().ui.valueLayout);
                        }
                    }

                    reader.readNext();
                }
                break;
            }
            else
                continue;

        }


    }
    else
    {
        emit sendToLog("could not read specified xml file");
        qDebug()<<"could not read specified xml file";
        return;
    }
    int i;
    for(i=0;i<configContainerList.count();i++)
        emit algConfigTag(configContainerList.at(i).containerLayout,QPoint(configContainerList.at(i).posx,configContainerList.at(i).posy));
    file.close();
}

void ALGConfigService::generateResult(ALG_TYPE algtype)
{
    clearList(&resultList);
    clearList(&resultContainerList);

    QFile file(algParamDir+algParamPath(algtype));
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QXmlStreamReader reader(&file);
        while(!reader.atEnd())
        {
            reader.readNext();
            if(reader.isStartElement()&&reader.name()=="Result")
            {
                resultSize=0;
                while(!(reader.isEndElement()&&reader.name()=="Result"))
                {
                    if(reader.isStartElement())
                    {
                        if(reader.name()=="Tag")
                        {
                            QXmlStreamAttributes att = reader.attributes();
                            resultContainerList.append(createResultContainer(att.value("posx").toInt(),att.value("posy").toInt()));
                        }
                        else if(reader.name()=="Title")
                            resultContainerList.last().titleLabel->setText(reader.readElementText());
                        else if(reader.name()=="Rect")
                        {
                            QXmlStreamAttributes att = reader.attributes();
                            int i;
                            int count = 1;
                            if(att.value("count")!="")
                                count=att.value("count").toInt();
                            QString labelName = reader.readElementText();
                            int visible = att.value("visible").toInt();
                            VALUE_TYPE type = VALUE_TYPE(att.value("type").toInt());
                            for(i=0;i<count;i++)
                            {
                            resultSize+=4*valueSize(type);
                            void* pdata = malloc(valueSize(type)*4);
                            resultList.append({labelName,PARAM_RECT,visible,type,pdata,createResultRect(labelName)});
                            if(visible==1)
                                resultContainerList.last().containerLayout->addLayout(resultList.last().ui);
                            }
                        }
                        else if(reader.name()=="Block")
                        {
                            QXmlStreamAttributes att = reader.attributes();
                            int i;
                            int count = 1;
                            if(att.value("count")!="")
                                count=att.value("count").toInt();
                            QString labelName = reader.readElementText();
                            VALUE_TYPE type = VALUE_TYPE(att.value("type").toInt());
                            int visible = att.value("visible").toInt();
                            for(i=0;i<count;i++)
                            {
                            resultSize+=2*valueSize(type);
                            void* pdata = malloc(valueSize(type)*3);
                            resultList.append({labelName,PARAM_BLOCK,visible,type,pdata,createResultBlock(labelName)});
                            if(visible==1)
                                resultContainerList.last().containerLayout->addLayout(resultList.last().ui);
                            }
                        }
                        else if(reader.name()=="Label")
                        {
                            QXmlStreamAttributes att = reader.attributes();
                            int i;
                            int count = 1;
                            if(att.value("count")!="")
                                count=att.value("count").toInt();
                            QString labelName = reader.readElementText();
                            VALUE_TYPE type = VALUE_TYPE(att.value("type").toInt());
                            int visible = att.value("visible").toInt();
                            for(i=0;i<count;i++)
                            {
                            resultSize+=valueSize(type);
                            void* pdata = malloc(valueSize(type));
                            resultList.append({labelName,PARAM_LABEL,visible,type,pdata,createResultLabel(labelName)});
                            if(visible==1)
                                resultContainerList.last().containerLayout->addLayout(resultList.last().ui);
                            }
                        }
                        else if(reader.name()=="Chart")
                        {
                            QXmlStreamAttributes att = reader.attributes();
                            int i;
                            int count = 1;
                            if(att.value("count")!="")
                                count=att.value("count").toInt();
                            QString labelName = reader.readElementText();
                            VALUE_TYPE type = VALUE_TYPE(att.value("type").toInt());
                            int visible = att.value("visible").toInt();
                            for(i=0;i<count;i++)
                            {
                                resultSize += valueSize(type);
                                void* pdata = malloc(valueSize(type));
                                RTChart* chart = new RTChart(labelName);
                                qreal min=0;
                                qreal max=1;
                                if(att.value("min")!="")
                                    min = att.value("min").toFloat();
                                if(att.value("max")!="")
                                    max = att.value("max").toFloat();
                                chart->setDataRange(min,max);
                                resultList.append({labelName,PARAM_CHART,visible,type,pdata,createResultChart(chart)});
                                if(visible==1)
                                    resultContainerList.last().containerLayout->addLayout(resultList.last().ui);
                            }
                        }
                    }
                    reader.readNext();
                }
                break;
            }
            else
                continue;
        }
    }
    else
    {
        emit sendToLog("could not read specified xml file");
        qDebug()<<"could not read specified xml file";
        return;
    }
    int i;
    for(i=0;i<resultContainerList.count();i++)
        emit algResultTag(resultContainerList.at(i).containerLayout,QPoint(resultContainerList.at(i).posx,resultContainerList.at(i).posy));
    file.close();
}

//support for different algorithm
QImage ALGConfigService::resultImage()
{
    QImage pic(1280,720,QImage::Format_ARGB32);
    pic.fill(QColor(0,0,0,0));
    QPainter pa(&pic);
    updateResultImg(&pa,&resultList,&configList);
    return pic;
}


QString ALGConfigService::value2Text(void *pdata, VALUE_TYPE type)
{
    switch(type)
    {
    case VALUE_INT:
        return QString::number(*(int*)pdata,10);
    case VALUE_UINT:
        return QString::number(*(unsigned int*)pdata,10);
    case VALUE_SHORT:
        return QString::number((int)*(short*)pdata,10);
    case VALUE_USHORT:
        return QString::number((unsigned int)*(ushort*)pdata,10);
    case VALUE_CHAR:
        return QString::number((int)*(char*)pdata,10);
    case VALUE_UCHAR:
        return QString::number((unsigned int)*(uchar*)pdata,10);
    case VALUE_FLOAT:
        return QString::number((double)*(float*)pdata,'g',6);
    case VALUE_DOUBLE:
        return QString::number(*(double*)pdata,'g',16);
    }
    return "";
}

void ALGConfigService::text2Value(QString src , void *dst, VALUE_TYPE type)
{
    switch(type)
    {
    case VALUE_INT:
        *(int*)dst=src.toInt();
        break;
    case VALUE_UINT:
        *(unsigned int*)dst=src.toUInt();
        break;
    case VALUE_SHORT:
        *(short*)dst=src.toShort();
        break;
    case VALUE_USHORT:
        *(unsigned short*)dst=src.toUShort();
        break;
    case VALUE_CHAR:
        *(char*)dst=src.toLatin1().data()[0];
        break;
    case VALUE_UCHAR:
        *(char*)dst=src.toLatin1().data()[0];
        break;
    case VALUE_FLOAT:
        *(float*)dst=src.toFloat();
        break;
    case VALUE_DOUBLE:
        *(double*)dst=src.toDouble();
        break;
    }
}


int ALGConfigService::valueSize(VALUE_TYPE type)
{
    switch(type)
    {
    case VALUE_INT:
        return sizeof(int);
    case VALUE_UINT:
        return sizeof(unsigned int);
    case VALUE_SHORT:
        return sizeof(short);
    case VALUE_USHORT:
        return sizeof(unsigned short);
    case VALUE_CHAR:
        return sizeof(char);
    case VALUE_UCHAR:
        return sizeof(unsigned char);
    case VALUE_FLOAT:
        return sizeof(float);
    case VALUE_DOUBLE:
        return sizeof(double);
    }
    return 0;
}

QString ALGConfigService::algParamPath(ALG_TYPE type)
{
    switch(type)
    {
    case ALG_SDS:
        return "ALGParam_SDS.xml";
    case ALG_GVSS:
        return "ALGParam_GVSS.xml";
    }
    return "";
}
