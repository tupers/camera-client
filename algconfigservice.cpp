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

void ALGConfigService::reflashResult(void *params)
{
    int i;
    int offset=0;
    for(i=0;i<resultList.count();i++)
    {
        ALGResultStr temp = resultList.at(i);
        int size=valueSize(temp.type);
        memcpy(temp.value,(char*)params+offset,size);
        temp.ui.valueLabel->setText(value2Text(temp.value,temp.type));
        offset+=size;
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

ALGResult_ui ALGConfigService::createResultTag(QString name)
{
    ALGResult_ui temp;
    temp.valueLayout = new QHBoxLayout;
    temp.nameLabel = new QLabel(name);
    temp.nameLabel->setStyleSheet("border-style:outset;"
                                  "border-top-width:1px;"
                                  "border-right-width:1px;"
                                  "border-color:rgba(50,50,50,255);"
                                  "padding-left:15px;");
    temp.valueLabel = new QLabel;
    temp.valueLabel->setStyleSheet("border-style:outset;"
                                   "border-top-width:1px;"
                                   "border-color:rgba(50,50,50,255);"
                                   "padding-left:15px;");
    temp.valueLayout->addWidget(temp.nameLabel);
    temp.valueLayout->addWidget(temp.valueLabel);

    return temp;
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

void ALGConfigService::deleteTag(ALGResult_ui tag)
{
    delete tag.nameLabel;
    delete tag.valueLabel;
    delete tag.valueLayout;
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
                        else if(reader.name()=="Label")
                        {
                            QXmlStreamAttributes att = reader.attributes();
                            QString labelName = reader.readElementText();
                            VALUE_TYPE type = VALUE_TYPE(att.value("type").toInt());
                            resultSize+=valueSize(type);
                            void* pdata = malloc(valueSize(type));
                            resultList.append({labelName,type,pdata,createResultTag(labelName)});
                            resultContainerList.last().containerLayout->addLayout(resultList.last().ui.valueLayout);
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

//void ALGConfigService::getParamsFromXml()
//{
//    QFile file("xmltest.xml");
//    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        QXmlStreamReader reader(&file);
//        while(!reader.atEnd())
//        {
//            reader.readNext();
//            if(reader.isStartElement()&&reader.name()=="Config")
//            {
//                configSize=0;
//                while(!(reader.isEndElement()&&reader.name()=="Config"))
//                {
//                    if(reader.isStartElement())
//                    {
//                        qDebug()<<reader.name();
//                        if(reader.name()=="Label")
//                        {
//                            QXmlStreamAttributes att = reader.attributes();
//                            configSize+=valueSize(VALUE_TYPE(att.value("type").toInt()));
//                        }
//                        //                    if(reader.name()=="Tag")
//                        //                    {
//                        //                        QXmlStreamAttributes att = reader.attributes();
//                        //                        qDebug()<<att.value("posx")<<att.value("posy");
//                        //                    }
//                        //                    if(reader.name()=="Title")
//                        //                        qDebug()<<reader.name()<<":"<<reader.readElementText();
//                    }
//                    reader.readNext();
//                }
//                break;
//            }
//            else
//                continue;
//        }
//    }
//    qDebug()<<configSize;
//    file.close();
//}

QString ALGConfigService::value2Text(void *pdata, VALUE_TYPE type)
{
    switch(type)
    {
    case VALUE_INT:
        return QString::number(*(int*)pdata,10);
    case VALUE_UINT:
        return QString::number(*(unsigned int*)pdata,10);
    case VALUE_SHORT:
        return QString::number(*(int*)pdata,10);
    case VALUE_USHORT:
        return QString::number(*(unsigned int*)pdata,10);
    case VALUE_CHAR:
        return QString::number(*(int*)pdata,10);
    case VALUE_UCHAR:
        return QString::number(*(unsigned int*)pdata,10);
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
