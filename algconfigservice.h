#ifndef ALGCONFIGSERVICE_H
#define ALGCONFIGSERVICE_H

#include <QObject>
#include <QVector>
#include <QXmlStreamReader>
#include <QFile>
#include <QtCharts>
#include "algconfigstr.h"
#include "./algConfig/alg_gvss.h"
#include "./utils/rtchart.h"
#include <QDir>
#include <QPainter>

class ALGConfigService : public QObject
{
    Q_OBJECT
public:
    explicit ALGConfigService(QObject *parent = 0);
    ~ALGConfigService();
    void initConfigList(ALG_TYPE type);
    void clearList(QVector<ALGConfigStr>* list);
    void clearList(QVector<ALGResultStr> *list);
    void clearList(QVector<ALGParamContainer>*list);
    void setConfig(void*);
    void getConfig(void*);
    void refreshResult(void*);
    void saveConfig();
    void loadConfig();
    int getConfigSize(){return configSize;}
    int getResultSize(){return resultSize;}
    ALGConfig_ui createConfigTag(QString);
    QLayout* createResultLabel(QString);
    QLayout* createResultRect(QString);
    QLayout* createResultBlock(QString);
    QLayout* createResultChart(RTChart*);
    ALGParamContainer createConfigContainer(int,int);
    ALGParamContainer createResultContainer(int,int);
    void deleteTag(ALGConfig_ui tag);
    void deleteTag(QLayout* tag);
    void deleteTag(ALGParamContainer tag);
    void generateConfig(ALG_TYPE);
    void generateResult(ALG_TYPE);
    QImage resultImage();
    //void getParamsFromXml();

signals:
    void algConfigTag(QVBoxLayout*,QPoint);
    void algResultTag(QVBoxLayout*,QPoint);
    void sendToLog(QString);
public slots:
private:
    QVector<ALGConfigStr> configList;
    QVector<ALGResultStr> resultList;
    QVector<ALGParamContainer> configContainerList;
    QVector<ALGParamContainer> resultContainerList;
    ALG_TYPE currentAlg;
    QString algParamDir = "./system/";
    int configSize=0;
    int resultSize=0;
    QString value2Text(void*,VALUE_TYPE);
    void text2Value(QString,void*,VALUE_TYPE);
    int valueSize(VALUE_TYPE);
    QString algParamPath(ALG_TYPE);
    void (*updateResultImg)(QPainter* painter,QVector<ALGResultStr>* result,QVector<ALGConfigStr>* config);

};

#endif // ALGCONFIGSERVICE_H
