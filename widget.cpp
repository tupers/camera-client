#include "widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{

    ui->setupUi(this);
    InitGUI();
}

Widget::~Widget()
{
    freeList();
    if(ftpPreView!=NULL)
    {
        delete ftpPreView;
        ftpPreView=NULL;
    }
    delete ui;
}
void Widget::InitGUI()
{
    this->setMinimumSize(1280,720);
    setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground,true);

    SetupAttachedWindow();
    //  setWindowOpacity(0.5);
    SetupLog();

    SerialPortWidget=new SerialPort();
    config = new ConfigFile;

    connect(config,SIGNAL(sendToLog(QString)),this,SLOT(LogDebug(QString)));
    //    connect(ui->button_close,SIGNAL(clicked()),SerialPortWidget,SLOT(close()));
    connect(ui->button_close,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->button_SerialPort,SIGNAL(clicked()),this,SLOT(OpenSubWindow_SerialPort()));
    connect(this,SIGNAL(SubWindow_Init()),SerialPortWidget,SLOT(SerialPort_Init()));

    SetupNetwork();
    SetupList();
    SetupMouseEvent();
    SetupVideo();
    SetupCalibrate();
    SetupRun();
    SetupConnection();
    SetupOptions();
    SetupInformations();
    SetupAccount();

    ui->mainlist->setVisible(false);
    ui->sublist->setCurrentIndex(ui->sublist->count()-1);
    ui->button_Save->setEnabled(false);
    ui->button_SaveAs->setEnabled(false);

#ifdef OFFLINE_DEBUG
    ui->sublist->setCurrentIndex(4);
    ui->mainlist->setVisible(true);
    config->initAlgService();
//    config->initAlgService();
//    qDebug()<<config->getAlgResultSize();
#endif

}

void Widget::SetupAttachedWindow()
{
    SaveasMenu = new QMenu;
    SaveasMenu->setObjectName("saveasmenu");
    QWidget* SaveasSubwidget = new QWidget();
    SaveasSubwidget->setObjectName("saveassubwidget");
    SaveasSubwidget->setStyleSheet("#saveassubwidget{background-color:rgba(50,50,50,220);}"
                                   "QLabel{color:white;font:bold 13pt Arial;}"
                                   "QPushButton{font:bold 13pt Arial;}");
    SaveasSubwidget->setFixedSize(200,100);
    QVBoxLayout *SaveasLayout = new QVBoxLayout;
    QLabel *SaveasLabel = new QLabel("Config File:");
    SaveasComboBox = new QComboBox;
    SaveasComboBox->setEditable(true);
    QPushButton *SaveasButton = new QPushButton("OK");
    connect(SaveasButton,SIGNAL(clicked()),this,SLOT(ConfigSaveAs()));
    SaveasLayout->addWidget(SaveasLabel);
    SaveasLayout->addWidget(SaveasComboBox);
    SaveasLayout->addWidget(SaveasButton);
    SaveasSubwidget->setLayout(SaveasLayout);
    QWidgetAction* SaveasAction=new QWidgetAction(SaveasMenu);
    SaveasAction->setDefaultWidget(SaveasSubwidget);
    SaveasMenu->addAction(SaveasAction);
    SaveasMenu->setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
    SaveasMenu->setAttribute(Qt::WA_TranslucentBackground);
    SaveasMenu->setStyleSheet("#saveasmenu{border-width:0px;background-color:rgba(0,0,0,0);}");
}
void Widget::OpenSubWindow_SerialPort()
{

    if(SerialPortWidget!=NULL)
        SerialPortWidget->show();
    emit SubWindow_Init();
}

void Widget::SetupMouseEvent()
{
    isLeftPressed=false;
    curPos=0;
    QCursor cursor;
    cursor.setShape(Qt::ArrowCursor);
    QWidget::setCursor(cursor);
    this->setMouseTracking(true);
    ui->mainframe->setMouseTracking(true);
}

void Widget::SetupOptions()
{
    QDir updatedir;
    if(updatedir.mkdir("./update/"))
    {
        LogDebug("did not find update dir, create update dir.");
        qDebug()<<"did not find update dir, create update dir.";
    }
    else
    {
        LogDebug("find update dir.");
        qDebug()<<"find update dir.";
    }

    updateWidget = new FirmwareUpdate(this);
    updateWidget->setWindowFlags(Qt::Popup);
    connect(updateWidget,SIGNAL(updateOption(int)),this,SLOT(firmwareUpdateService(int)));


    /* for config list browser*/
    ui->system_configlist->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->system_configlist->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->system_configlist->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //    ui->system_configlist->setAlternatingRowColors(true);
    ui->system_configlist->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->system_configlist->setColumnCount(5);
    QStringList sys_header;
    sys_header<<"Name"<<"Creator"<<"ChangedBy"<<"Loaded"<<" ";
    ui->system_configlist->setHorizontalHeaderLabels(sys_header);
    ui->system_configlist->verticalHeader()->setVisible(false);
    //    ui->system_configlist->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgba(0,0,0,0);}");

    //ui->system_configlist->setStyleSheet("background-color:rgba(50,50,50,120);");

    /* for user account browser */
    connect(ui->security_addusercontrolokButton,SIGNAL(clicked()),this,SLOT(UserAdd()));

    ui->security_tablewidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->security_tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->security_tablewidget->setAlternatingRowColors(true);
    ui->security_tablewidget->setColumnCount(3);
    QStringList security_Hheader;
    security_Hheader<<"User Name"<<"Authority"<<"";
    ui->security_tablewidget->setHorizontalHeaderLabels(security_Hheader);
    ui->security_tablewidget->horizontalHeader()->setSectionsClickable(false);
    ui->security_tablewidget->verticalHeader()->setVisible(false);
    ui->security_tablewidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //    ui->security_tablewidget->setStyleSheet("background-color:rgba(50,50,50,120);");
    //    for(i=0;i<ui->security_tablewidget->rowCount();i++)
    //        ui->security_tablewidget->horizontalHeaderItem(i)->setTextAlignment(Qt::AlignCenter);

    /*for ftp browser*/
    //    QFtp ftp;
    //    int ret = ftp.connectToHost("192.168.1.61");
    //    qDebug()<<"handle is: "<<ret;
    //    ftpthread =new QThread;
    //    ftpmanager = new Ftp_Manager;
    //    ftpmanager->moveToThread(ftpthread);
    connect(ui->diagnostic_refreshButton,SIGNAL(clicked()),this,SLOT(reflashFtp()));
    //    connect(ui->diagnostic_refreshButton,SIGNAL(clicked()),network,SIGNAL(listFtp()));
    //    connect(ftpmanager,SIGNAL(sendToLog(QString)),this,SLOT(LogDebug(QString)));
    connect(network,SIGNAL(listInfoFtp(FileList*)),this,SLOT(UpdateFtpList(FileList*)));
    connect(network,SIGNAL(getDataFtp(QByteArray)),this,SLOT(saveFtpData(QByteArray)));
    //    connect(this,SIGNAL(downloadFtpFile(QString,QFile*)),ftpmanager,SLOT(downloadFtp(QString,QFile*)));
    //    connect(ftpmanager,SIGNAL(getFtpState(bool)),this,SLOT(downloadFtpFinished(bool)));
    //    connect(this,SIGNAL(openFtp(NetworkStruct)),ftpmanager,SLOT(openFtp(NetworkStruct)));
    //    ftpthread->start();
    ui->diagnostic_ftpbrowsertablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->diagnostic_ftpbrowsertablewidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->diagnostic_ftpbrowsertablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->diagnostic_ftpbrowsertablewidget->setAlternatingRowColors(true);
    ui->diagnostic_ftpbrowsertablewidget->setColumnCount(3);
    QStringList diagnostic_header;
    diagnostic_header<<"Name"<<"Size"<<"Type";
    ui->diagnostic_ftpbrowsertablewidget->setHorizontalHeaderLabels(diagnostic_header);
    ui->diagnostic_ftpbrowsertablewidget->horizontalHeader()->setSectionsClickable(false);
    ui->diagnostic_ftpbrowsertablewidget->verticalHeader()->setVisible(false);
    ui->diagnostic_ftpbrowsertablewidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    /*first update config list*/
    UpdateConfigFile();

}

void Widget::SetupCalibrate()
{
    ui->light_led1pwmSlider->setRange(0,100);
    ui->light_led2pwmSlider->setRange(0,100);

    ui->camera_contrastSlider->setRange(0,255);
    ui->camera_saturationSlider->setRange(0,255);
    ui->camera_sharpnessSlider->setRange(0,255);
    ui->camera_brightnessSlider->setRange(0,255);
    ui->camera_exposureSlider->setRange(0,255);
    ui->camera_sensorgainSlider->setRange(0,255);
    ui->camera_pipegainSlider->setRange(0,255);
    ui->video_JPEGqualitySlider->setRange(0,100);

    connect(ui->light_led1pwmSlider,SIGNAL(sliderMoved(int)),ui->light_led1pwmlineEdit,SLOT(setValue(int)));
    connect(ui->light_led1pwmSlider,SIGNAL(sliderReleased()),ui->light_led1pwmlineEdit,SLOT(sendValueSignal()));
    connect(ui->light_led1pwmlineEdit,SIGNAL(valueFinished(int)),ui->light_led1pwmSlider,SLOT(setValue(int)));
    connect(ui->light_led1pwmlineEdit,SIGNAL(valueUpdateStr(int)),config,SLOT(setLightConfig(int)));
    connect(ui->light_led2pwmSlider,SIGNAL(sliderMoved(int)),ui->light_led2pwmlineEdit,SLOT(setValue(int)));
    connect(ui->light_led2pwmSlider,SIGNAL(sliderReleased()),ui->light_led2pwmlineEdit,SLOT(sendValueSignal()));
    connect(ui->light_led2pwmlineEdit,SIGNAL(valueFinished(int)),ui->light_led2pwmSlider,SLOT(setValue(int)));
    connect(ui->light_led2pwmlineEdit,SIGNAL(valueUpdateStr(int)),config,SLOT(setLightConfig(int)));
    connect(ui->light_led1enablecomboBox,SIGNAL(activated(int)),config,SLOT(setLightConfig(int)));
    connect(ui->light_led2enablecomboBox,SIGNAL(activated(int)),config,SLOT(setLightConfig(int)));
    connect(config,SIGNAL(sendToServerLightConfig(QVariant)),network,SLOT(sendConfigToServerLightConfig(QVariant)));

    //Camera_Normal immediate updating
    connect(ui->camera_contrastSlider,SIGNAL(sliderMoved(int)),ui->camera_contrastlineEdit,SLOT(setValue(int)));
    connect(ui->camera_contrastSlider,SIGNAL(sliderReleased()),ui->camera_contrastlineEdit,SLOT(sendValueSignal()));
    connect(ui->camera_contrastlineEdit,SIGNAL(valueFinished(int)),ui->camera_contrastSlider,SLOT(setValue(int)));
    connect(ui->camera_contrastlineEdit,SIGNAL(valueUpdateStr(int)),config,SLOT(setContrast(int)));
    connect(ui->camera_saturationSlider,SIGNAL(sliderMoved(int)),ui->camera_saturationlineEdit,SLOT(setValue(int)));
    connect(ui->camera_saturationSlider,SIGNAL(sliderReleased()),ui->camera_saturationlineEdit,SLOT(sendValueSignal()));
    connect(ui->camera_saturationlineEdit,SIGNAL(valueFinished(int)),ui->camera_saturationSlider,SLOT(setValue(int)));
    connect(ui->camera_saturationlineEdit,SIGNAL(valueUpdateStr(int)),config,SLOT(setSaturation(int)));
    connect(ui->camera_sharpnessSlider,SIGNAL(sliderMoved(int)),ui->camera_sharpnesslineEdit,SLOT(setValue(int)));
    connect(ui->camera_sharpnessSlider,SIGNAL(sliderReleased()),ui->camera_sharpnesslineEdit,SLOT(sendValueSignal()));
    connect(ui->camera_sharpnesslineEdit,SIGNAL(valueFinished(int)),ui->camera_sharpnessSlider,SLOT(setValue(int)));
    connect(ui->camera_sharpnesslineEdit,SIGNAL(valueUpdateStr(int)),config,SLOT(setSharpness(int)));
    connect(ui->camera_exposureSlider,SIGNAL(sliderMoved(int)),ui->camera_exposurelineEdit,SLOT(setValue(int)));
    connect(ui->camera_exposureSlider,SIGNAL(sliderReleased()),ui->camera_exposurelineEdit,SLOT(sendValueSignal()));
    connect(ui->camera_exposurelineEdit,SIGNAL(valueFinished(int)),ui->camera_exposureSlider,SLOT(setValue(int)));
    connect(ui->camera_exposurelineEdit,SIGNAL(valueUpdateStr(int)),config,SLOT(setExposure(int)));
    connect(ui->camera_sensorgainSlider,SIGNAL(sliderMoved(int)),ui->camera_sensorgainlineEdit,SLOT(setValue(int)));
    connect(ui->camera_sensorgainSlider,SIGNAL(sliderReleased()),ui->camera_sensorgainlineEdit,SLOT(sendValueSignal()));
    connect(ui->camera_sensorgainlineEdit,SIGNAL(valueFinished(int)),ui->camera_sensorgainSlider,SLOT(setValue(int)));
    connect(ui->camera_sensorgainlineEdit,SIGNAL(valueUpdateStr(int)),config,SLOT(setSensorGain(int)));
    connect(ui->camera_pipegainSlider,SIGNAL(sliderMoved(int)),ui->camera_pipegainlineEdit,SLOT(setValue(int)));
    connect(ui->camera_pipegainSlider,SIGNAL(sliderReleased()),ui->camera_pipegainlineEdit,SLOT(sendValueSignal()));
    connect(ui->camera_pipegainlineEdit,SIGNAL(valueFinished(int)),ui->camera_pipegainSlider,SLOT(setValue(int)));
    connect(ui->camera_pipegainlineEdit,SIGNAL(valueUpdateStr(int)),config,SLOT(setPipeGain(int)));
    connect(ui->camera_brightnessSlider,SIGNAL(sliderMoved(int)),ui->camera_brightnesslineEdit,SLOT(setValue(int)));
    connect(ui->camera_brightnessSlider,SIGNAL(sliderReleased()),ui->camera_brightnesslineEdit,SLOT(sendValueSignal()));
    connect(ui->camera_brightnesslineEdit,SIGNAL(valueFinished(int)),ui->camera_brightnessSlider,SLOT(setValue(int)));
    connect(ui->camera_brightnesslineEdit,SIGNAL(valueUpdateStr(int)),config,SLOT(setBrightness(int)));
    connect(ui->camera_2AmodecomboBox,SIGNAL(activated(int)),config,SLOT(set2AMode(int)));
    connect(ui->camera_2AmodecomboBox,SIGNAL(activated(int)),this,SLOT(camera_2AmodecomboBox_Service(int)));
    connect(ui->camera_whitebalancemodecomboBox,SIGNAL(activated(int)),config,SLOT(setWhiteBalanceMode(int)));
    connect(ui->camera_daynightmodecomboBox,SIGNAL(activated(int)),config,SLOT(setDayNightMode(int)));
    connect(ui->camera_binningcomboBox,SIGNAL(activated(int)),config,SLOT(setBinning(int)));
    connect(ui->camera_mirrorcomboBox,SIGNAL(activated(int)),config,SLOT(setMirror(int)));
    connect(ui->camera_expprioritycomboBox,SIGNAL(activated(int)),config,SLOT(setExpPriority(int)));

    //Camera_2A entire updating
    connect(ui->camera_2A_TargetBrightnessSlider,SIGNAL(sliderMoved(int)),ui->camera_2A_TargetBrightnesslineEdit,SLOT(setValue(int)));
    connect(ui->camera_2A_TargetBrightnessSlider,SIGNAL(sliderReleased()),ui->camera_2A_TargetBrightnesslineEdit,SLOT(sendValueSignal()));
    connect(ui->camera_2A_TargetBrightnesslineEdit,SIGNAL(valueFinished(int)),ui->camera_2A_TargetBrightnessSlider,SLOT(setValue(int)));
    connect(ui->camera_2A_MaxBrightnessSlider,SIGNAL(sliderMoved(int)),ui->camera_2A_MaxBrightnesslineEdit,SLOT(setValue(int)));
    connect(ui->camera_2A_MaxBrightnessSlider,SIGNAL(sliderReleased()),ui->camera_2A_MaxBrightnesslineEdit,SLOT(sendValueSignal()));
    connect(ui->camera_2A_MaxBrightnesslineEdit,SIGNAL(valueFinished(int)),ui->camera_2A_MaxBrightnessSlider,SLOT(setValue(int)));
    connect(ui->camera_2A_MinBrightnessSlider,SIGNAL(sliderMoved(int)),ui->camera_2A_MinBrightnesslineEdit,SLOT(setValue(int)));
    connect(ui->camera_2A_MinBrightnessSlider,SIGNAL(sliderReleased()),ui->camera_2A_MinBrightnesslineEdit,SLOT(sendValueSignal()));
    connect(ui->camera_2A_MinBrightnesslineEdit,SIGNAL(valueFinished(int)),ui->camera_2A_MinBrightnessSlider,SLOT(setValue(int)));
    connect(ui->camera_2A_ThresholdSlider,SIGNAL(sliderMoved(int)),ui->camera_2A_ThresholdlineEdit,SLOT(setValue(int)));
    connect(ui->camera_2A_ThresholdSlider,SIGNAL(sliderReleased()),ui->camera_2A_ThresholdlineEdit,SLOT(sendValueSignal()));
    connect(ui->camera_2A_ThresholdlineEdit,SIGNAL(valueFinished(int)),ui->camera_2A_ThresholdSlider,SLOT(setValue(int)));

    //Camera_advanced immediate updating
    connect(ui->camera_vsenablecomboBox,SIGNAL(activated(int)),config,SLOT(setVsEnable(int)));
    connect(ui->camera_ldcenablecomboBox,SIGNAL(activated(int)),config,SLOT(setLDCEnable(int)));
    connect(ui->camera_vnfenablecomboBox,SIGNAL(activated(int)),config,SLOT(setVnfEnable(int)));
    connect(ui->camera_vnfmodecomboBox,SIGNAL(activated(int)),config,SLOT(setVnfMode(int)));

    //Video immediate updating
    QStackedLayout* videoLayout = new QStackedLayout;
    camera_videoinputwidget = new MultiImageWidget;
    camera_videoinputwidget->resizeImageCount(2);
    ui->camera_videoWidget->setLayout(videoLayout);

    videoLayout->addWidget(ui->camera_2A_AEWeight_preWidget);
    videoLayout->addWidget(camera_videoinputwidget);
    videoLayout->setStackingMode(QStackedLayout::StackAll);

    //videoLayout->addWidget()
    connect(ui->video_streamtypecomboBox,SIGNAL(activated(int)),config,SLOT(setStreamType(int)));
    connect(ui->video_videocodecmodecomboBox,SIGNAL(activated(int)),config,SLOT(setVideoCodecMode(int)));
    connect(ui->video_JPEGqualitySlider,SIGNAL(sliderMoved(int)),ui->video_JPEGqualitylineEdit,SLOT(setValue(int)));
    connect(ui->video_JPEGqualitySlider,SIGNAL(sliderReleased()),ui->video_JPEGqualitylineEdit,SLOT(sendValueSignal()));
    connect(ui->video_JPEGqualitylineEdit,SIGNAL(valueFinished(int)),ui->video_JPEGqualitySlider,SLOT(setValue(int)));
    connect(ui->video_JPEGqualitylineEdit,SIGNAL(valueUpdateStr(int)),config,SLOT(setJPEGQuality(int)));

    //Algorithm
    //    connect(this,SIGNAL(setALGParams()),config,SLOT(setAlgParams()));
    connect(config,SIGNAL(algConfigTag(QVBoxLayout*,QPoint)),this,SLOT(loadALGConfigUi(QVBoxLayout*,QPoint)));
    connect(config,SIGNAL(algResultTag(QVBoxLayout*,QPoint)),this,SLOT(loadALGResultUi(QVBoxLayout*,QPoint)));
    connect(ui->camera_2A_AEWeight_preWidget,SIGNAL(sendWeightInfo(EzCamH3AWeight)),this,SLOT(getH3AWeight(EzCamH3AWeight)));
}

void Widget::UpdateConfigFile()
{
    LogDebug("update the cfg file dir");
    qDebug()<<"update the cfg file dir";
    QString cfgname = config->getCurrentConfigName();
    int i=0;
    QFileInfoList infolist =QDir(config->getConfigDir()).entryInfoList();
    ui->system_configlist->clearContents();
    ui->system_configlist->setRowCount(0);
    ui->run_processmodeConfigBox->clear();
    SaveasComboBox->clear();
    foreach(QFileInfo fileinfo,infolist)
    {
        if(!fileinfo.isFile())continue;
        if(fileinfo.suffix().compare("ini")==0)
        {
            config->OpenConfig(fileinfo.baseName());

            ui->run_processmodeConfigBox->insertItem(ui->run_processmodeConfigBox->count(),fileinfo.baseName());
            SaveasComboBox->insertItem(SaveasComboBox->count(),fileinfo.baseName());
            ui->system_configlist->insertRow(ui->system_configlist->rowCount());
            ui->system_configlist->setItem(i,0,new QTableWidgetItem(fileinfo.baseName()));
            ui->system_configlist->setItem(i,1,new QTableWidgetItem(config->ConfigCreator()));
            ui->system_configlist->setItem(i,2,new QTableWidgetItem(config->ConfigChangedby()));

            if(cfgname==fileinfo.baseName())
            {
                ui->system_configlist->setItem(i,3,new QTableWidgetItem("true"));
                ui->run_currentconfigLabel->setText("Current Config: "+fileinfo.baseName());
            }
            else
                ui->system_configlist->setItem(i,3,new QTableWidgetItem("false"));
            QPushButton *pbutton = new QPushButton("delete");
            connect(pbutton,SIGNAL(clicked()),this,SLOT(ConfigDelete()));
            ui->system_configlist->setCellWidget(i,4,pbutton);
            i++;
        }
    }
    if(cfgname=="")
    {
        LogDebug("first time update cfg file dir");
        qDebug()<<"first time update cfg file dir";
    }
    else
    {
        LogDebug("restore the current cfg file"+cfgname);
        qDebug()<<"restore the current cfg file"+cfgname;
        config->OpenConfig(cfgname);
        ui->run_processmodeConfigBox->setCurrentText(config->getCurrentConfigName());
    }
}

void Widget::UpdateUserAccountList()
{

    ui->security_tablewidget->clearContents();
    ui->security_tablewidget->setRowCount(0);
    get_userdata_t userlist;
    if(!network->GetUser(&userlist))
    {
        emit LogDebug("GetUserlist failed.");
        qDebug()<<"GetUserlist failed.";
        if(ui->Accountlist->currentIndex()==0)
            recoverLoginState();
        return;
    }
    for(int i=0;i<userlist.userCount;i++)
    {
        qDebug()<<"find user: "<<userlist.user[i].user;
        ui->security_tablewidget->insertRow(ui->security_tablewidget->rowCount());
        QTableWidgetItem* user = new QTableWidgetItem(userlist.user[i].user);
        user->setTextAlignment(Qt::AlignCenter);
        ui->security_tablewidget->setItem(i,0,user);
        QTableWidgetItem* authority = new QTableWidgetItem;
        if(userlist.user[i].authority==AUTHORITY_ADMIN)
            authority->setText("Admin");
        else
            authority->setText("Viewer");
        authority->setTextAlignment(Qt::AlignCenter);
        ui->security_tablewidget->setItem(i,1,authority);
        if(user->text()!="admin")
        {
            QPushButton *delbutton = new QPushButton("delete");
            connect(delbutton,SIGNAL(clicked()),this,SLOT(deleteUser()));
            ui->security_tablewidget->setCellWidget(i,2,delbutton);
        }

    }
}

void Widget::UpdateCameraList(int id, QString IP)
{
    //ui->Account_cameralist->
    //    ui->Account_cameralist->insertItem(ui->Account_cameralist->currentRow(),IP.toString());
    QString cameraInfo = QString::number(id,10)+"\t"+IP;
    ui->Account_cameralist->addItem(cameraInfo);

    //    QWidget* camerainfo=new QWidget;
    //    QHBoxLayout* camerainfolayout = new QHBoxLayout(camerainfo);
    //    camerainfolayout->addWidget(id);
}

void Widget::reflashFtp()
{
    if(!network->isFtpLogin())
    {
        ui->diagnostic_ftpbrowsertablewidget->clearContents();
        ui->diagnostic_ftpbrowsertablewidget->setRowCount(0);
        NetworkStr networkconfigFromServer = network->getNetworkConfig();
        networkconfigFromServer.value.ports_ftpserverip="192.168.1.224";
        networkconfigFromServer.value.ports_ftpusername="root";
        networkconfigFromServer.value.ports_ftppassword="ftpcam";
        networkconfigFromServer.value.ports_ftpserverport="8010";
        emit network->connectFtp(networkconfigFromServer.value.ports_ftpserverip,networkconfigFromServer.value.ports_ftpserverport.toInt(),
                                 networkconfigFromServer.value.ports_ftpusername,networkconfigFromServer.value.ports_ftppassword);
    }
}

void Widget::UpdateFtpList(FileList* list)
{
    ui->diagnostic_ftpbrowsertablewidget->insertRow(ui->diagnostic_ftpbrowsertablewidget->rowCount());
    ui->diagnostic_ftpbrowsertablewidget->setItem(ui->diagnostic_ftpbrowsertablewidget->rowCount()-1,0,new QTableWidgetItem("..."));
    int i;
    int cnt = list->dirCount;
    for(i=0;i<cnt;i++)
    {
        ui->diagnostic_ftpbrowsertablewidget->insertRow(ui->diagnostic_ftpbrowsertablewidget->rowCount());
        ui->diagnostic_ftpbrowsertablewidget->setItem(ui->diagnostic_ftpbrowsertablewidget->rowCount()-1,0,new QTableWidgetItem(QString(list->finfo[i].name)));
        ui->diagnostic_ftpbrowsertablewidget->setItem(ui->diagnostic_ftpbrowsertablewidget->rowCount()-1,1,new QTableWidgetItem(QString::number(list->finfo[i].size,10)));
        ui->diagnostic_ftpbrowsertablewidget->setItem(ui->diagnostic_ftpbrowsertablewidget->rowCount()-1,2,new QTableWidgetItem(QString::number(list->finfo[i].type,10)));
    }
    if(list!=NULL)
        free(list);
}

void Widget::saveFtpData(QByteArray data)//20170119, 解析保存文件
{
    //    FILE* fp;
    //    fp = fopen("D:/DM8127/data.dat","wb");
    //    fwrite(data.data(),1,data.count(),fp);
    //    fclose(fp);
    //get img info (result size)
    EzImgFileHeader header;
    memcpy(&header,data.data(),sizeof(EzImgFileHeader));
    //    int resultsize;
    //    memcpy(&resultsize,data.data()+sizeof(int)*2,sizeof(int));

    int headsize = sizeof(EzImgFileHeader)+header.imgInfoSize;
    if(ftpPreView!=NULL)
    {
        delete ftpPreView;
        ftpPreView=NULL;
    }
    ftpPreView = new QImage((unsigned char*)data.data()+headsize,1280,720,header.pitch,QImage::Format_Grayscale8);
    *ftpPreView=ftpPreView->copy();
    ui->diagnostic_previewWidget->setImage(ftpPreView->scaledToWidth(ui->diagnostic_previewWidget->width()),0);
    //
    //    if(ftpfile.FilePath==""||ftpfile.ftpFileName==""||ftpfile.ftpFileSize==0||ftpfile.ftpFileValid==0)
    //    {
    //        qDebug()<<"ftp str error";
    //        return;
    //    }
    //    QString dir = ftpfile.FilePath+"/"+ftpfile.ftpFileName+".yuv";
    //    QString dir0 = ftpfile.FilePath+"/"+ftpfile.ftpFileName;
    //    ftpfile.ftpFile = fopen(dir.toLatin1().data(),"wb");
    //    //get img info (result size)
    //    int resultsize;
    //    memcpy(&resultsize,data.data()+sizeof(int)*2,sizeof(int));
    //    int headsize = sizeof(EzImgFileHeader)+resultsize;
    //    //
    //    fwrite(data.data()+headsize,1,ftpfile.ftpFileSize-headsize,ftpfile.ftpFile);
    //    unsigned char* uv=new unsigned char[(ftpfile.ftpFileSize-headsize)/2];

    //    memset(uv,128,(ftpfile.ftpFileSize-headsize)/2);
    //    fwrite(uv,1,(ftpfile.ftpFileSize-headsize)/2,ftpfile.ftpFile);
    //    fclose(ftpfile.ftpFile);
    //    qDebug()<<"save ftp file finished";
    //    delete(uv);

    //    ftpfile.ftpFile = fopen(dir0.toLatin1().data(),"wb");
    //    fwrite(data.data(),1,ftpfile.ftpFileSize,ftpfile.ftpFile);
    //    fclose(ftpfile.ftpFile);

    //    ftpfile.ftpFile=NULL;
}

void Widget::LoadFullConfig()
{
    LoadRunConfig();
    LoadCalibrateConfig();
    LoadConnectionConfig();
    LoadInfomationConfig();
    LoadAccountConfig();
}

void Widget::LoadCalibrateConfig()
{
    CalibrateStr tempconfig = config->getCalibrate();
    ui->light_led1enablecomboBox->setCurrentIndex(tempconfig.value.light_config[0].enable);
    ui->light_led2enablecomboBox->setCurrentIndex(tempconfig.value.light_config[1].enable);
    ui->light_led1pwmlineEdit->editValue(tempconfig.value.light_config[0].pwmduty);
    ui->light_led2pwmlineEdit->editValue(tempconfig.value.light_config[1].pwmduty);
    ui->camera_contrastlineEdit->editValue(tempconfig.value.camera_Contrast);
    ui->camera_saturationlineEdit->editValue(tempconfig.value.camera_Saturation);
    ui->camera_sharpnesslineEdit->editValue(tempconfig.value.camera_Sharpness);
    ui->camera_exposurelineEdit->editValue(tempconfig.value.camera_Exposure);
    ui->camera_sensorgainlineEdit->editValue(tempconfig.value.camera_SensorGain);
    ui->camera_pipegainlineEdit->editValue(tempconfig.value.camera_PipeGain);
    ui->camera_brightnesslineEdit->editValue(tempconfig.value.camera_Brightness);
    ui->camera_whitebalancemodecomboBox->setCurrentIndex(tempconfig.value.camera_WhiteBalanceMode);
    ui->camera_daynightmodecomboBox->setCurrentIndex(tempconfig.value.camera_DayNightMode);
    ui->camera_binningcomboBox->setCurrentIndex(tempconfig.value.camera_Binning);
    ui->camera_mirrorcomboBox->setCurrentIndex(tempconfig.value.camera_Mirror);
    ui->camera_expprioritycomboBox->setCurrentIndex(tempconfig.value.camera_ExpPriority);
    ui->camera_2AmodecomboBox->setCurrentIndex(tempconfig.value.camera_2AMode);
    ui->camera_vsenablecomboBox->setCurrentIndex(tempconfig.value.camera_VsEnable);
    ui->camera_ldcenablecomboBox->setCurrentIndex(tempconfig.value.camera_LdcEnable);
    ui->camera_vnfenablecomboBox->setCurrentIndex(tempconfig.value.camera_VnfEnable);
    ui->camera_vnfmodecomboBox->setCurrentIndex(tempconfig.value.camera_VnfMode);

    if(ui->camera_2AmodecomboBox->currentIndex()==0||ui->camera_2AmodecomboBox->currentIndex()==2)
    {
        ui->camera_exposurelineEdit->setEnabled(true);
        ui->camera_exposureSlider->setEnabled(true);
        ui->camera_pipegainlineEdit->setEnabled(true);
        ui->camera_pipegainSlider->setEnabled(true);
        ui->camera_sensorgainlineEdit->setEnabled(true);
        ui->camera_sensorgainSlider->setEnabled(true);
    }
    else
    {
        ui->camera_exposurelineEdit->setEnabled(false);
        ui->camera_exposureSlider->setEnabled(false);
        ui->camera_pipegainlineEdit->setEnabled(false);
        ui->camera_pipegainSlider->setEnabled(false);
        ui->camera_sensorgainlineEdit->setEnabled(false);
        ui->camera_sensorgainSlider->setEnabled(false);
    }

    ui->camera_2A_TargetBrightnesslineEdit->editValue(tempconfig.value.camera_2A_TargetBrightness);
    ui->camera_2A_MaxBrightnesslineEdit->editValue(tempconfig.value.camera_2A_MaxBrightness);
    ui->camera_2A_MinBrightnesslineEdit->editValue(tempconfig.value.camera_2A_MinBrightness);
    ui->camera_2A_ThresholdlineEdit->editValue(tempconfig.value.camera_2A_Threshold);
    ui->camera_2A_widthValueLabel->setText(QString::number(tempconfig.value.camera_2A_width,10));
    ui->camera_2A_heightValueLabel->setText(QString::number(tempconfig.value.camera_2A_height,10));
    ui->camera_2A_HCountValueLabel->setText(QString::number(tempconfig.value.camera_2A_HCount,10));
    ui->camera_2A_VCountValueLabel->setText(QString::number(tempconfig.value.camera_2A_VCount,10));
    ui->camera_2A_HStartValueLabel->setText(QString::number(tempconfig.value.camera_2A_HStart,10));
    ui->camera_2A_VStartValueLabel->setText(QString::number(tempconfig.value.camera_2A_VStart,10));
    ui->camera_2A_horzIncrValueLabel->setText(QString::number(tempconfig.value.camera_2A_horzIncr,10));
    ui->camera_2A_vertIncrValueLabel->setText(QString::number(tempconfig.value.camera_2A_vertIncr,10));
    ui->camera_2A_AEWeight_width1lineEdit->setText(QString::number(tempconfig.value.camera_2A_weight_width1));
    ui->camera_2A_AEWeight_height1lineEdit->setText(QString::number(tempconfig.value.camera_2A_weight_height1));
    ui->camera_2A_AEWeight_h_start2lineEdit->setText(QString::number(tempconfig.value.camera_2A_weight_h_start2));
    ui->camera_2A_AEWeight_v_start2lineEdit->setText(QString::number(tempconfig.value.camera_2A_weight_v_start2));
    ui->camera_2A_AEWeight_width2lineEdit->setText(QString::number(tempconfig.value.camera_2A_weight_width2));
    ui->camera_2A_AEWeight_height2lineEdit->setText(QString::number(tempconfig.value.camera_2A_weight_height2));
    ui->camera_2A_AEWeight_weightlineEdit->setText(QString::number(tempconfig.value.camera_2A_weight_weight));
    ui->camera_2A_AEWeight_preWidget->weightResize(tempconfig.value.camera_2A_HCount,tempconfig.value.camera_2A_VCount);

    ui->video_streamtypecomboBox->setCurrentIndex(tempconfig.value.camera_video_StreamType);
    ui->video_videocodecmodecomboBox->setCurrentIndex(tempconfig.value.camera_video_VideoCodecMode);
    ui->video_JPEGqualitylineEdit->editValue(tempconfig.value.camera_video_JPEGQuality);

    //    ui->algorithm_ROI_startXlineEdit->setText(QString::number(tempconfig.value.algorithm_ROI_startX));
    //    ui->algorithm_ROI_endXlineEdit->setText(QString::number(tempconfig.value.algorithm_ROI_endX));
    //    ui->algorithm_ROI_startYlineEdit->setText(QString::number(tempconfig.value.algorithm_ROI_startY));
    //    ui->algorithm_ROI_endYlineEdit->setText(QString::number(tempconfig.value.algorithm_ROI_endY));
    //    ui->algorithm_f_rdifsidecirlineEdit->setText(QString::number(tempconfig.value.algorithm_f_RDifSideCir));
    //    ui->algorithm_fcirgood1lineEdit->setText(QString::number(tempconfig.value.algorithm_fCirGood_1));
    //    ui->algorithm_fcirgood2lineEdit->setText(QString::number(tempconfig.value.algorithm_fCirGood_2));
    //    ui->algorithm_fcirgood3lineEdit->setText(QString::number(tempconfig.value.algorithm_fCirGood_3));
    //    ui->algorithm_fcirwarning1lineEdit->setText(QString::number(tempconfig.value.algorithm_fCirWarning_1));
    //    ui->algorithm_fcirwarning2lineEdit->setText(QString::number(tempconfig.value.algorithm_fCirWarning_2));
    //    ui->algorithm_fcirwarning3lineEdit->setText(QString::number(tempconfig.value.algorithm_fCirWarning_3));
    //    ui->algorithm_fcentercirgoodoffsetlineEdit->setText(QString::number(tempconfig.value.algorithm_fCenterCirGoodOffset));
    //    ui->algorithm_fcentercirwarningoffsetlineEdit->setText(QString::number(tempconfig.value.algorithm_fCenterCirWarningOffset));
    //    ui->algorithm_nbiggestraduislineEdit->setText(QString::number(tempconfig.value.algorithm_nBiggestRaduis,10));
    //    ui->algorithm_nsmallestraduislineEdit->setText(QString::number(tempconfig.value.algorithm_nSmallestRaduis,10));
    //    ui->algorithm_bolt_biggestarealineEdit->setText(QString::number(tempconfig.value.algorithm_Blot_BiggestArea,10));
    //    ui->algorithm_bolt_smallestarealineEdit->setText(QString::number(tempconfig.value.algorithm_Blot_SmallestArea,10));
    //    ui->algorithm_bolt_xyoffsetlineEdit->setText(QString::number(tempconfig.value.algorithm_Blot_xyOffset,10));
    //    ui->algorithm_cir_smallestarealineEdit->setText(QString::number(tempconfig.value.algorithm_Cir_SmallestArea,10));
    //    ui->algorithm_cir_xyoffsetlineEdit->setText(QString::number(tempconfig.value.algorithm_Cir_xyOffset,10));
    //    ui->algorithm_cir_errflineEdit->setText(QString::number(tempconfig.value.algorithm_Cir_errf));
    //    ui->algorithm_judge_xyoffsetlineEdit->setText(QString::number(tempconfig.value.algorithm_Judge_xyOffset,10));
    //    ui->algorithm_speed_k1lineEdit->setText(QString::number(tempconfig.value.algorithm_Speed_k1));
    //    ui->algorithm_speed_k2lineEdit->setText(QString::number(tempconfig.value.algorithm_Speed_k2));
    //    ui->algorithm_speed_expandpixel1lineEdit->setText(QString::number(tempconfig.value.algorithm_Speed_ExpandPixel1,10));
    //    ui->algorithm_speed_expandpixel2lineEdit->setText(QString::number(tempconfig.value.algorithm_Speed_ExpandPixel2,10));

}

void Widget::LoadRunConfig()
{
    //    RunStr tempconfig = config->getRun();
    //    ui->run_processmodeAlgBox->setChecked(tempconfig.value.LoadAlg);
    ServerState tempstate = config->getState();
    ui->run_processmodeSearchAreaBox->setChecked(false);
    //for debug.
    ui->run_algrunmodeBox->setCurrentIndex(tempstate.algTriggle);
    ui->run_algsourceBox->setCurrentIndex(tempstate.algImgsrc);
}

void Widget::LoadConnectionConfig()
{
    NetworkStr tempconfig = network->getNetworkConfig();
    ui->ports_ipaddressLineEdit->setText(tempconfig.value.ports_ipaddress);
    ui->ports_netmaskLineEdit->setText(tempconfig.value.ports_netmask);
    ui->ports_gatewayLineEdit->setText(tempconfig.value.ports_gateway);
    ui->ports_primarynameserverLineEdit->setText(tempconfig.value.ports_dns);
    ui->ports_ftpserverLineEdit->setText(tempconfig.value.ports_ftpserverip);
    ui->ports_ftpserverportLineEdit->setText(tempconfig.value.ports_ftpserverport);
    ui->ports_usernameLineEdit->setText(tempconfig.value.ports_ftpusername);
    ui->ports_passwordLineEdit->setText(tempconfig.value.ports_ftppassword);
    ui->ports_fileuploadpathLineEdit->setText(tempconfig.value.ports_ftpfoldername);
    //    ui->ports_rtspport1LineEdit->setText(QString::number(tempconfig.value.ports_rtspport,10));
    ui->ports_multicastcomboBox->setCurrentIndex(tempconfig.value.ports_rtspmulticast);
}

void Widget::LoadInfomationConfig()
{
    InformationStr tempconfig = config->getInformation();
    NetworkStr tempnetwork = network->getNetworkConfig();

    ui->resource_totalmemoryvalueLabel->setText(QString::number(tempconfig.value.resource_totalmem)+"\tMB");
    ui->resource_freememoryvalueLabel->setText(QString::number(tempconfig.value.resource_freemem)+"\tMB");
    ui->resource_usedmemoryvalueLabel->setText(QString::number(tempconfig.value.resource_totalmem-tempconfig.value.resource_freemem)+"\tMB");
    ui->resource_sharedmemoryvalueLabel->setText(QString::number(tempconfig.value.resource_sharedmem)+"\tMB");
    ui->resource_armvalueLabel->setText(QString::number(tempconfig.value.resource_corearmmem)+"\tMB");
    ui->resource_dspvalueLabel->setText(QString::number(tempconfig.value.resource_coredspmem)+"\tMB");
    ui->resource_m3videovalueLabel->setText(QString::number(tempconfig.value.resource_corem3videomem)+"\tMB");
    ui->resource_m3vpssvalueLabel->setText(QString::number(tempconfig.value.resource_corem3vpssmem)+"\tMB");
    ui->resource_totalstoragevalueLabel->setText(QString::number(tempconfig.value.resource_storagemem)+"\tMB");
    ui->resource_usedstoragevalueLabel->setText(QString::number(tempconfig.value.resource_storagemem-tempconfig.value.resource_storagefreemem)+"\tMB");

    ui->device_versionvalueLabel->setText(QString::number(tempconfig.value.device_version));
    ui->device_algvalueLabel->setText(QString::number(config->getState().algType,10));
    ui->device_macvalueLabel->setText(tempnetwork.value.ports_mac);
    ui->device_ipvalueLabel->setText(tempnetwork.value.ports_ipaddress);
    ui->device_armfrqvalueLabel->setText(QString::number(tempconfig.value.resource_corearmfreq)+"\tMhz");
    ui->device_dspfrqvalueLabel->setText(QString::number(tempconfig.value.resource_coredspfreq)+"\tMhz");
    ui->device_m3videofrqvalueLabel->setText(QString::number(tempconfig.value.resource_corem3videofreq)+"\tMhz");
    ui->device_m3vpssfrqvalueLabel->setText(QString::number(tempconfig.value.resource_corem3vpssfreq)+"\tMhz");

    memoryseries->append("M3 VPSS", tempconfig.value.resource_corem3vpssmem);
    memoryseries->append("M3 VIDEO", tempconfig.value.resource_corem3videomem);
    memoryseries->append("ARM Cortex-A8", tempconfig.value.resource_corearmmem);
    memoryseries->append("DSP 674x", tempconfig.value.resource_coredspmem);
    memoryseries->setLabelsVisible(true);
    foreach(QPieSlice *slice,memoryseries->slices())
    {
        slice->setLabelColor(Qt::white);
        slice->setLabelFont(QFont("Arial",10,QFont::Bold));
        //        slice->setBorderWidth(5);
        slice->setBorderColor(QColor(0,0,0,0));
    }

    storageseries->append("Used", tempconfig.value.resource_storagemem-tempconfig.value.resource_storagefreemem);
    storageseries->append("Unused", tempconfig.value.resource_storagefreemem);
    storageseries->setLabelsVisible(true);
    foreach(QPieSlice *slice,storageseries->slices())
    {
        slice->setLabelColor(Qt::white);
        slice->setLabelFont(QFont("Arial",10,QFont::Bold));
        //        slice->setBorderWidth(5);
        slice->setBorderColor(QColor(0,0,0,0));
    }
}

void Widget::LoadAccountConfig()
{
    ui->account_usernamevalueLabel->setText(network->getLogUserName());
    ui->account_authorityvalueLabel->setText(QString::number(network->getLogAuthority(),10));
}

void Widget::ResetOptions()
{
    ui->security_userNameLineEdit->clear();
    ui->security_passwordLineEdit->clear();
    ui->security_confirmPasswordLineEdit->clear();
    ui->security_authorityadminButton->setChecked(false);
    ui->security_authorityviewerButton->setChecked(false);

    ui->diagnostic_ftpbrowsertablewidget->clearContents();
    ui->diagnostic_ftpbrowsertablewidget->setRowCount(0);
}

void Widget::ResetInformation()
{
    cpuloadTimer->stop();
    memoryseries->clear();
    storageseries->clear();
    cpuloadSeries->clear();
    cpuloadSeries2->clear();
}

void Widget::ResetAccount()
{
    ui->LoginSpecifiedIPlineEdit->clear();
    ui->Account_cameralist->clear();
}

void Widget::SetupVideo()
{
    videothread =new QThread;
    h264video = new H264Video;
    h264video->moveToThread(videothread);
    connect(this,SIGNAL(videocontrol(int)),h264video,SLOT(H264VideoOpen(int)));
    connect(h264video,SIGNAL(getImage_Camera(QImage)),this,SLOT(setVideoImage_Camera(QImage)));
    connect(h264video,SIGNAL(getImage_Run(QImage)),this,SLOT(setVideoImage_Run(QImage)));
    connect(h264video,SIGNAL(clearImage()),this,SLOT(clearVideoImage()));
    //    connect(h264video,SIGNAL(getVideoInfo(int,int,int)),this,SLOT(setVideoInfo(int,int,int)));
    connect(h264video,SIGNAL(sendToLog(QString)),this,SLOT(LogDebug(QString)));
    videothread->start();
}

void Widget::SetupRun()
{
    algresultTimer = new QTimer;
    algresultTimer->setInterval(1000);
    algresultTimer->setSingleShot(false);
    connect(algresultTimer,SIGNAL(timeout()),this,SLOT(algresultUpdate()));

    SourceVideoWidget = new SourceVideo();
    connect(this,SIGNAL(getImage_Source(QImage,int)),SourceVideoWidget,SLOT(setImage(QImage,int)));
    connect(SourceVideoWidget,SIGNAL(SourceVideoClose()),this,SLOT(SourceVideoWidgetClose()));
    connect(ui->run_processmodeAlgBox,SIGNAL(stateChanged(int)),config,SLOT(setLoadAlg(int)));

    RunVideoImage=QImage(1280,720,QImage::Format_RGB888);

    ui->run_videoinputwidget->resizeImageCount(2);
    QDir savedir;
    if(savedir.mkdir("./save/"))
    {
        LogDebug("did not find save dir, creat save dir.");
        qDebug()<<"did not find save dir, creat save dir.";
    }
    else
    {
        LogDebug("find save dir.");
        qDebug()<<"find save dir.";
    }
}

void Widget::SetupLog()
{
    logbar=new QStatusBar(this);
    ui->sublistLayout->addWidget(logbar);
    logbar->setStyleSheet("QStatusBar{color:black;background-color:rgba(255,242,204,200);}");
    connect(logbar,SIGNAL(messageChanged(QString)),this,SLOT(logtimeout(QString)));
    effect=new QGraphicsOpacityEffect();
    effect->setOpacity(0.7);
    logbar->setGraphicsEffect(effect);
}

void Widget::SetupInformations()
{
    memoryseries = new QPieSeries();
    memoryseries->setHoleSize(0.3);

    QChart *chart = new QChart();
    chart->addSeries(memoryseries);
    chart->setBackgroundVisible(false);
    chart->legend()->hide();
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("background-color:rgba(0,0,0,0);");
    ui->resource_memorychartlayout->addWidget(chartView);

    storageseries = new QPieSeries();
    storageseries->setHoleSize(0.3);

    QChart *storagechart = new QChart();
    storagechart->addSeries(storageseries);
    storagechart->setBackgroundVisible(false);
    storagechart->legend()->hide();
    QChartView *storageView = new QChartView(storagechart);
    storageView->setRenderHint(QPainter::Antialiasing);
    storageView->setStyleSheet("background-color:rgba(0,0,0,0);");
    ui->resource_storagechartlayout->addWidget(storageView);

    cpuloadTimer = new QTimer;
    cpuloadTimer->setInterval(1000);
    cpuloadTimer->setSingleShot(false);
    connect(ui->sublist,SIGNAL(currentChanged(int)),this,SLOT(sublistService(int)));
    connect(cpuloadTimer,SIGNAL(timeout()),this,SLOT(cpuloadUpdate()));

    cpuloadSeries = new QLineSeries();
    cpuloadSeries->append(QDateTime::currentMSecsSinceEpoch(),5.0);
    cpuloadSeries2 = new QLineSeries();
    cpuloadSeries2->append(QDateTime::currentMSecsSinceEpoch(),3.0);
    //    cpuloadSeries2->append(0.0,1.0);
    cpuloadChart = new QChart();
    cpuloadChart->addSeries(cpuloadSeries);
    cpuloadChart->addSeries(cpuloadSeries2);
    cpuloadChart->createDefaultAxes();
    axisX = new QDateTimeAxis;
    //    QValueAxis* axisX = new QValueAxis;
    axisX->setTickCount(12);
    axisX->setFormat("h:mm:ss");
    QDateTime mint=QDateTime::currentDateTime().addSecs(-119);
    QDateTime maxt=QDateTime::currentDateTime().addSecs(1);
    axisX->setRange(mint,maxt);
    cpuloadChart->setAxisX(axisX,cpuloadSeries);
    cpuloadSeries2->attachAxis(axisX);
    cpuloadChart->axisY()->setRange(0,10);
    cpuloadChart->legend()->hide();
    cpuloadChart->setAnimationOptions(QChart::SeriesAnimations);
    QChartView *cpuloadView = new QChartView(cpuloadChart);
    cpuloadView->setRenderHint(QPainter::Antialiasing);
    cpuloadView->setStyleSheet("background-color:rgba(0,0,0,0);");
    ui->resource_cpuloadlayout->addWidget(cpuloadView);
    qsrand((uint) QTime::currentTime().msec());
    //    testtime.setDate(QDate::currentDate());
}

void Widget::SetupConnection()
{
    connect(this,SIGNAL(NetworkConfigtoServer()),network,SLOT(sendNetworkConfig()));
}

void Widget::SetupNetwork()
{
    network = new NetWork;
    connect(network,SIGNAL(udpRcvDataInfo(uchar*,int,int,int)),SLOT(frameFromSensor(uchar*,int,int,int)));
    connect(network,SIGNAL(udpRcvDataInfo(QByteArray)),SerialPortWidget,SLOT(udpDataRcv(QByteArray)));
    connect(ui->run_videosavesensorButton,SIGNAL(clicked()),network,SLOT(GetFrameFromSensor()));
    connect(SerialPortWidget,SIGNAL(setDebugMode(int)),this,SLOT(setDebugMode(int)));
    connect(ui->LogoutButton,SIGNAL(clicked()),network,SLOT(disconnectConnection()));
    connect(network,SIGNAL(AccountUpdate()),this,SLOT(UpdateUserAccountList()));
    connect(network,SIGNAL(sendToLog(QString)),this,SLOT(LogDebug(QString)));
    connect(network,SIGNAL(loginFailed()),this,SLOT(recoverLoginState()));
    connect(network,SIGNAL(loginSuccess(uint8_t)),this,SLOT(LoginSuccess(uint8_t)));
    connect(network,SIGNAL(disconnectfromServer()),this,SLOT(ConnectionLost()));
    //    connect(config,SIGNAL(sendToServer(_NET_MSG,QString)),network,SLOT(sendConfigToServer(_NET_MSG,QString)));
    connect(config,SIGNAL(sendToServer(_NET_MSG,unsigned char)),network,SLOT(sendConfigToServer(_NET_MSG,unsigned char)));
    connect(config,SIGNAL(sendToServer(_NET_MSG,unsigned char,unsigned char)),network,SLOT(sendConfigToServer(_NET_MSG,unsigned char,unsigned char)));
    connect(config,SIGNAL(sendToServerEntire(QVariant)),network,SLOT(sendConfigToServerEntire(QVariant)));
    connect(config,SIGNAL(sendToServerEntireForBoot(QVariant)),network,SLOT(sendConfigToServerEntireForBoot(QVariant)));
    connect(config,SIGNAL(sendToServerALG(QVariant)),network,SLOT(sendConfigToServerALG(QVariant)));
    connect(config,SIGNAL(sendToServerH3A(QVariant)),network,SLOT(sendConfigToServerH3A(QVariant)));

}

void Widget::SetupAccount()
{
    connect(network,SIGNAL(cameraDevice(int,QString)),this,SLOT(UpdateCameraList(int,QString)));
    ui->LoginSpecifiedIPlineEdit->setVisible(false);
    ui->LoginSpecifiedIPLabel->setVisible(false);
    emit network->cameraScan();
    ui->Accountlist->setCurrentIndex(0);
}

void Widget::VideoCMD(H264Video* video,int cmd)
{
    video->changeVideoStatus();
    emit videocontrol(cmd);
}

void Widget::LogDebug(QString msg)
{
    logbar->showMessage(msg,2000);
}

void Widget::paintEvent(QPaintEvent *event)
{
    //    QPainterPath path;
    //    path.setFillRule(Qt::WindingFill);
    //    path.addRect(0, 0, this->width()+20, this->height()+20);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    //    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for(int i=0; i<5; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(i, i, this->width()+(-i)*2, this->height()+(-i)*2);
        color.setAlpha(i*40);
        painter.setPen(color);
        painter.drawPath(path);
    }
}

void Widget::closeEvent(QCloseEvent *event)
{
    SerialPortWidget->close();
    SourceVideoWidget->close();

}

int Widget::countFlag(QPoint p,int row)
{
    if(p.y()<MARGIN_L)
        return 10+row;
    else if(p.y()>this->height()-MARGIN_S)
        return 30+row;
    else
        return 20+row;
}
int Widget::countRow(QPoint p)
{
    return p.x()>(this->width()-MARGIN_S)?2:1;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        this->isLeftPressed=true;
        QPoint temp=event->globalPos();
        pLast=temp;
        curPos=countFlag(event->pos(),countRow(event->pos()));
        event->ignore();
    }
}
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if(isLeftPressed)
        isLeftPressed=false;
    QApplication::restoreOverrideCursor();
    event->ignore();
}
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    int poss=countFlag(event->pos(),countRow(event->pos()));
    if(poss==22)
        setCursor(Qt::SizeHorCursor);
    else if(poss==31)
        setCursor(Qt::SizeVerCursor);
    //    else if(poss==32)
    //        setCursor(Qt::SizeFDiagCursor);
    else
        setCursor(Qt::ArrowCursor);
    if(isLeftPressed)
    {
        QPoint ptemp=event->globalPos();
        ptemp=ptemp-pLast;
        if(curPos==11)
        {
            ptemp=ptemp+pos();
            move(ptemp);
        }
        else
        {
            QRect wid=geometry();
            switch (curPos)
            {
            case 22:wid.setRight(wid.right()+ptemp.x());
                break;
            case 31:wid.setBottom(wid.bottom()+ptemp.y());
                break;
                //            case 32:wid.setBottomRight(wid.bottomRight()+ptemp);
                //                break;
            }
            setGeometry(wid);
        }
        pLast=event->globalPos();
    }
    event->ignore();
}
void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton&&countFlag(event->pos(),countRow(event->pos()))==11)
    {
        if(windowState()!=Qt::WindowFullScreen)
        {
            ui->mainwidgetLayout->setContentsMargins(0,0,0,0);
            setWindowState(Qt::WindowFullScreen);
        }
        else
        {
            ui->mainwidgetLayout->setContentsMargins(4,4,4,4);
            this->setGeometry(QApplication::desktop()->screenGeometry().width()/6,QApplication::desktop()->screenGeometry().height()/6,1280,720);
        }

    }
    event->ignore();
}

void Widget::SetupList()
{

    item_Run = addListItem(QString("  Run"),ui->mainlist,QIcon(":/image/source/Run.png"));
    item_Calibrate = addListItem(QString("  Calibrate"),ui->mainlist,QIcon(":/image/source/Calibrate.png"));
    item_Connection = addListItem(QString("  Connection"),ui->mainlist,QIcon(":/image/source/Connection.png"));
    item_Options = addListItem(QString("  Options"),ui->mainlist,QIcon(":/image/source/options.png"));
    item_Information = addListItem(QString("  Information"),ui->mainlist,QIcon(":/image/source/Information.png"));

    ui->mainlist->setFocusPolicy(Qt::NoFocus);
    setStyleSheet("#mainlist {background-color: rgba(0,0,0,0);border:0px}");
    ui->mainlist->setStyleSheet("QListWidget::Item {color:rgba(150,150,150,255);border-style:outset;"
                                "border-left-width:3px;border-color:rgba(0,0,0,0);}"
                                "QListWidget::Item:hover {color:rgba(255,255,255,255);"
                                "background-color: rgba(0,0,0,0);}"
                                "QListWidget::Item:selected {color:rgba(255,255,255,255);border-style:outset;"
                                "border-left-width: 3px;border-color: rgba(192,0,0,255);"
                                "background-color: rgba(50,50,50,200);}");
    connect(ui->mainlist,SIGNAL(currentRowChanged(int)),ui->sublist,SLOT(setCurrentIndex(int)));
}
QListWidgetItem* Widget::addListItem(QString name, QListWidget* list, QIcon icon=QIcon())
{
    QListWidgetItem* newitem = new QListWidgetItem(name,list);
    newitem->setSizeHint(QSize(newitem->sizeHint().rwidth(),50));
    newitem->setFont(QFont("Arial",15,QFont::Bold));
    newitem->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    newitem->setIcon(icon);
    return newitem;
}
void Widget::freeList()
{
    freeListItem(item_Calibrate);
    freeListItem(item_Connection);
    freeListItem(item_Run);
    freeListItem(item_Options);
    freeListItem(item_Information);
}

void Widget::freeListItem(QListWidgetItem* item)
{
    delete item;
}

void Widget::on_button_Account_clicked()
{
    ui->sublist->setCurrentIndex(5);
}

void Widget::on_button_maxsize_clicked()
{
    if(windowState()!=Qt::WindowFullScreen)
    {
        ui->mainwidgetLayout->setContentsMargins(0,0,0,0);
        setWindowState(Qt::WindowFullScreen);
    }
    else
    {
        ui->mainwidgetLayout->setContentsMargins(4,4,4,4);
        setWindowState(Qt::WindowNoState);
    }

}

void Widget::on_button_minsize_clicked()
{
    this->showMinimized();
}

void Widget::on_LoginButton_clicked()
{
    //
    //连接账户服务器
    //
    if(ui->LoginSpecifiedIPlineEdit->text()!="")
    {
        NetworkStr config = network->getNetworkConfig();
        config.value.ports_ipaddress = ui->LoginSpecifiedIPlineEdit->text();
        network->setNetworkConfig(config);
        LogDebug(QString("use specified IP address:%1").arg(ui->LoginSpecifiedIPlineEdit->text()));
        qDebug()<<QString("use specified IP address:%1").arg(ui->LoginSpecifiedIPlineEdit->text());
    }
    network->Login(ui->usernameLineEdit->text(),ui->passwordLineEdit->text());
    ui->LoginButton->setEnabled(false);
}

void Widget::on_button_Save_clicked()
{
    config->UpdateConfig(network->getUserName());
    UpdateConfigFile();

}

void Widget::on_button_SaveAs_clicked()
{
    SaveasMenu->exec(ui->button_SaveAs->mapToGlobal(QPoint(-15,ui->button_SaveAs->height())));
}

void Widget::on_system_configlist_clicked(const QModelIndex &index)
{
    ui->system_configcurrentlabel->setText(ui->system_configlist->item(index.row(),0)->text());
}

void Widget::on_system_configloadButton_clicked()
{
    if(ui->system_configcurrentlabel->text()!="")
    {
        config->OpenConfig(ui->system_configcurrentlabel->text());
        config->SetConfig();
        LoadFullConfig();
        UpdateConfigFile();
    }
    else
    {
        LogDebug("load failed, select config file first");
        qDebug()<<"load failed, select config file first";
    }
}

void Widget::ConfigSaveAs()
{
    SaveasMenu->hide();
    config->SaveAsConfig(SaveasComboBox->currentText(),network->getUserName());
    UpdateConfigFile();
    UpdateConfigFile();
}

void Widget::ConfigDelete()
{
    LogDebug("ready to delete");
    qDebug()<<"ready to delete";
    QPushButton* pbutton = qobject_cast<QPushButton*>(sender());
    int row=ui->system_configlist->indexAt(QPoint(pbutton->frameGeometry().x(),pbutton->frameGeometry().y())).row();
    if(ui->system_configlist->item(row,0)->text()==config->getCurrentConfigName())
    {
        LogDebug("could not delete loaded config");
        qDebug()<<"could not delete loaded config";
    }
    else
    {
        QFile f;
        f.remove(config->getConfigDir()+ui->system_configlist->item(row,0)->text()+".ini");
        UpdateConfigFile();
    }
}

void Widget::UserAdd()
{
    if(ui->security_passwordLineEdit->text()==ui->security_confirmPasswordLineEdit->text())
    {
        if(ui->security_authorityadminButton->isChecked()&&(!ui->security_authorityviewerButton->isChecked()))
            network->AddUser(ui->security_userNameLineEdit->text(),ui->security_passwordLineEdit->text(),0);
        else if((!ui->security_authorityadminButton->isChecked())&&ui->security_authorityviewerButton->isChecked())
            network->AddUser(ui->security_userNameLineEdit->text(),ui->security_passwordLineEdit->text(),1);
        else
        {
            LogDebug("choose authority first.");
            qDebug()<<"choose authority first.";
            return;
        }
    }
    else
    {
        LogDebug("Can not confirm password, adduser failed.");
        qDebug()<<"Can not confirm password, adduser failed.";
        return;
    }
}

void Widget::setVideoImage_Run(QImage image)
{
    if (image.height()>0)
    {
        RunVideoImage=image;
        if(SourceFlag==false)
            ui->run_videoinputwidget->setImage(image.scaledToHeight(ui->run_videoinputwidget->height()),1);
        else
            emit getImage_Source(image,1);
    }
}

void Widget::logtimeout(QString msg)
{
    if(msg=="")
    {
        effect->setOpacity(0.1);
        logbar->setGraphicsEffect(effect);
    }
    else
    {
        effect->setOpacity(0.7);
        logbar->setGraphicsEffect(effect);
    }
}

void Widget::LoginSuccess(uint8_t authority)
{
    //    h264video->setH264VideoUrl("rtsp://192.168.1.88:8554/h264ESVideoTest");
    config->OpenConfig(QString("fromCamera"));
    ConfigStr configFromServer;
    if(!network->GetSysinfo(&configFromServer))
    {
        emit LogDebug("GetSysinfo failed.");
        qDebug()<<"GetSysinfo failed.";
        recoverLoginState();
        return;
    }
    //set rtsp for h264
    LogDebug(QString("get rtsp url: %1").arg(network->getRTSPurl()));
    qDebug()<<"get rtsp url: "<<network->getRTSPurl();
    h264video->setH264VideoUrl(network->getRTSPurl());
    //set config with specified config from server and load ui params
    config->SetConfig(configFromServer);//reload
    config->initAlgService();
    if(config->getAlgConfigSize()!=0)
    {
        void* algparam = (void*)malloc(config->getAlgConfigSize());
        if(network->GetParams(NET_MSG_IMGALG_GET_PARAM,algparam,config->getAlgConfigSize()))
            config->setAlgConfig(algparam);
        free(algparam);
    }
    else
    {
        LogDebug("invalid ALG config size");
        qDebug()<<"invalid ALG config size";
    }
    LoadFullConfig();

    //load menu
    if(AUTHORITY_ADMIN==authority)
    {
        ui->mainlist->item(0)->setHidden(false);
        ui->mainlist->item(1)->setHidden(false);
        ui->mainlist->item(2)->setHidden(false);
        ui->mainlist->item(3)->setHidden(false);
        ui->mainlist->item(4)->setHidden(false);
        //update account list for administer
        UpdateUserAccountList();
    }
    else
    {
        ui->mainlist->item(0)->setHidden(false);
        ui->mainlist->item(1)->setHidden(true);
        ui->mainlist->item(2)->setHidden(true);
        ui->mainlist->item(3)->setHidden(true);
        ui->mainlist->item(4)->setHidden(false);
    }
    ui->mainlist->setVisible(true);
    ui->Accountlist->setCurrentIndex(1);
    ui->button_Save->setEnabled(true);
    ui->button_SaveAs->setEnabled(true);
    ui->LoginButton->setEnabled(true);
    //update config list file browser
    UpdateConfigFile();

}

void Widget::ConnectionLost()
{
    if(ui->Accountlist->currentIndex()==1)
    {
        //
        //登出处理内容:关闭视频流
        //
        VideoCMD(h264video,VIDEO_TERMINATE);
        emit network->closeFtp();

        ui->sublist->setCurrentIndex(5);
        ui->mainlist->setVisible(false);
        ui->Accountlist->setCurrentIndex(0);
        ui->button_Save->setEnabled(false);
        ui->button_SaveAs->setEnabled(false);
        if(ui->mainlist->currentRow()!=-1)
        {
            ui->mainlist->selectedItems()[0]->setSelected(false);
            ui->mainlist->setCurrentRow(-1);
        }
        algresultTimer->stop();
        ResetOptions();
        ResetInformation();
        ResetAccount();
    }
}


void Widget::recoverLoginState()
{
    ui->LoginButton->setEnabled(true);
}

void Widget::setVideoImage_Camera(QImage image)
{
    if (image.height()>0)
    {
        QImage temp;
        if(camera_videoinputwidget->ratio()>=1.778)
            temp = image.scaledToHeight(camera_videoinputwidget->height());
        else
            temp =  image.scaledToWidth(camera_videoinputwidget->width());
        camera_videoinputwidget->setImage(temp,0);
        ui->camera_2A_AEWeight_preWidget->setActiveRegion(temp.size());
    }
}

void Widget::on_camera_videocontrolButton_clicked()
{
    VideoCMD(h264video,VIDEO_SHOW_CAMERA);
}

void Widget::clearVideoImage()
{
    LogDebug("clear image labels.");
    qDebug()<<"clear image labels.";
    //    ui->camera_videoinputlabel->clear();
    //    ui->run_videoinputlabel->clear();
    ui->run_videoinputwidget->clearImage(1);
    //ui->camera_videoinputwidget->clearImage();
    camera_videoinputwidget->clearImage(0);
    //    ui->summary_statecodelabel->setText("State Code: ");
    //    ui->summary_warningcodelabel->setText("Warning Code: ");
}

void Widget::on_run_videocontrolButton_clicked()
{
    VideoCMD(h264video,VIDEO_SHOW_RUN);
}

void Widget::on_run_videosourceButton_clicked()
{
    SourceFlag=true;
    SourceVideoWidget->clearImage();
    SourceVideoWidget->show();
}

void Widget::SourceVideoWidgetClose()
{
    SourceFlag=false;
}

void Widget::on_run_processmodeLoadButton_clicked()
{
    config->OpenConfig(ui->run_processmodeConfigBox->currentText());
    config->SetConfig();
    LoadFullConfig();
    UpdateConfigFile();
}

void Widget::deleteUser()
{
    QPushButton* delbutton = qobject_cast<QPushButton*>(sender());
    int row=ui->security_tablewidget->indexAt(QPoint(delbutton->frameGeometry().x(),delbutton->frameGeometry().y())).row();
    network->DeleteUser(ui->security_tablewidget->item(row,0)->text());
}

void Widget::on_system_configbootdefaultButton_clicked()
{
    QString cfgname = config->getCurrentConfigName();
    if(ui->system_configcurrentlabel->text()=="")
    {
        LogDebug("set default boot config failed, select config file first");
        qDebug()<<"set default boot config failed, select config file first";
        return;
    }
    config->OpenConfig(ui->system_configcurrentlabel->text());
    config->setConigAsBoot();
    config->OpenConfig(cfgname);
}


void Widget::on_ports_setButton_clicked()
{
    NetworkStr tempconfig=network->getNetworkConfig();
    tempconfig.value.ports_ipaddress=ui->ports_ipaddressLineEdit->text();
    tempconfig.value.ports_netmask=ui->ports_netmaskLineEdit->text();
    tempconfig.value.ports_gateway=ui->ports_gatewayLineEdit->text();
    tempconfig.value.ports_dns=ui->ports_primarynameserverLineEdit->text();
    tempconfig.value.ports_ftpserverip=ui->ports_ftpserverLineEdit->text();
    tempconfig.value.ports_ftpserverport=ui->ports_ftpserverportLineEdit->text();
    tempconfig.value.ports_ftpusername=ui->ports_usernameLineEdit->text();
    tempconfig.value.ports_ftppassword=ui->ports_passwordLineEdit->text();
    tempconfig.value.ports_ftpfoldername=ui->ports_fileuploadpathLineEdit->text();
    //    tempconfig.value.ports_rtspport=ui->ports_rtspport1LineEdit->text().toInt();
    tempconfig.value.ports_rtspmulticast=ui->ports_multicastcomboBox->currentIndex();
    network->setNetworkConfig(tempconfig);
    emit NetworkConfigtoServer();
}

void Widget::on_security_addusercontrolcancelButton_clicked()
{
    ui->security_userNameLineEdit->clear();
    ui->security_passwordLineEdit->clear();
    ui->security_confirmPasswordLineEdit->clear();
    ui->security_authorityadminButton->setChecked(false);
    ui->security_authorityviewerButton->setChecked(false);
}

void Widget::cpuloadUpdate()
{
    EzcpuDynamicParam cpuload;
    if(network->GetCpuloadresult(&cpuload))
    {
        //        qDebug()<<"arm: "<<cpuload.prf_a8<<"dsp: "<<cpuload.prf_dsp;
        int minload=0;
        int maxload=0;
        if(cpuload.prf_a8>cpuload.prf_dsp)
        {
            maxload=cpuload.prf_a8;
            minload=cpuload.prf_dsp;
        }
        else
        {
            maxload=cpuload.prf_dsp;
            minload=cpuload.prf_a8;
        }
        maxload=maxload>95?100:(maxload+5);
        minload=minload<5?0:(minload-5);
        ui->resource_armcortexa8loadlabel->setText("ARM cortex-A8 load:\t"+QString::number(cpuload.prf_a8)+"%");
        ui->resource_dsp674xloadlabel->setText("DSP674x load:\t"+QString::number(cpuload.prf_dsp)+"%");
        QDateTime currenttime(QDate::currentDate(),QTime::currentTime());
        cpuloadSeries->append(currenttime.toMSecsSinceEpoch(),cpuload.prf_a8);
        cpuloadSeries2->append(currenttime.toMSecsSinceEpoch(),cpuload.prf_dsp);
        cpuloadChart->axisY()->setRange(minload,maxload);
        axisX->setRange(currenttime.addSecs(-119),currenttime.addSecs(1));
    }
    else
    {
        LogDebug("CPU load failed");
        qDebug()<<"CPU load failed";
        cpuloadTimer->stop();
    }
}

void Widget::algresultUpdate()
{

    if(config->getAlgResultSize()!=0)
    {
        void* tempresult = (void*)malloc(config->getAlgResultSize());
//        qDebug()<<config->getAlgResultSize();
        if(network->GetParams(NET_MSG_GET_ALG_RESULT,tempresult,config->getAlgResultSize()))
        {
            config->reflashAlgResult(tempresult);
//            QImage temp =config->refreshAlgImage().scaledToHeight(ui->run_videoinputwidget->height());
            if(SourceFlag==false)
            ui->run_videoinputwidget->setImage(config->refreshAlgImage(),0,SCALE_HEIGHT);
            else
                emit getImage_Source(config->refreshAlgImage(),0);
        }
        else
        {
            algresultTimer->stop();
            LogDebug("get ALG result failed from network");
            qDebug()<<"get ALG result failed from network";
        }
//        int test[9]={0,0,1270,710,1,2,3,4,99};
//        config->reflashAlgResult(test);
//        ui->run_videoinputwidget->setImage(config->refreshAlgImage(),1,SCALE_WIDTH);
        free(tempresult);
    }
    else
    {
        LogDebug("invalid ALG result size");
        qDebug()<<"invalid ALG result size";
        algresultTimer->stop();
    }
}

void Widget::camera_2AmodecomboBox_Service(int index)
{
    if(index==0||index==2)
    {
        ui->camera_exposurelineEdit->setEnabled(true);
        ui->camera_exposureSlider->setEnabled(true);
        ui->camera_pipegainlineEdit->setEnabled(true);
        ui->camera_pipegainSlider->setEnabled(true);
        ui->camera_sensorgainlineEdit->setEnabled(true);
        ui->camera_sensorgainSlider->setEnabled(true);

    }
    else
    {
        ui->camera_exposurelineEdit->setEnabled(false);
        ui->camera_exposureSlider->setEnabled(false);
        ui->camera_pipegainlineEdit->setEnabled(false);
        ui->camera_pipegainSlider->setEnabled(false);
        ui->camera_sensorgainlineEdit->setEnabled(false);
        ui->camera_sensorgainSlider->setEnabled(false);
    }
    unsigned char tempexposure;
    unsigned short tempgain;
    network->getConfigFromSever(NET_MSG_GET_EXPOSURE,&tempexposure);
    network->getConfigFromSever(NET_MSG_GET_GAIN,&tempgain,2);
    //tempgain=tempgain&0xffff;
    ui->camera_exposurelineEdit->editValue(tempexposure);
    ui->camera_sensorgainlineEdit->editValue(tempgain>>8);
    ui->camera_pipegainlineEdit->editValue(tempgain&0xff);
    ConfigStr tempconfig;
    tempconfig = config->getConfig();
    tempconfig.calibrate.value.camera_Exposure=tempexposure;
    tempconfig.calibrate.value.camera_SensorGain=tempgain>>8;
    tempconfig.calibrate.value.camera_PipeGain=tempgain&0xff;
    qDebug()<<"sensor gain is: "<<tempconfig.calibrate.value.camera_SensorGain<<" pipe gain is: "<<tempconfig.calibrate.value.camera_PipeGain;
    config->SetConfig(tempconfig,CONFIG_SAVEONLY);
}

void Widget::sublistService(int index)
{
    if(index==4)
    {
        //        LogDebug("enable cpu load");
        //qDebug()<<"enable cpu load";
        cpuloadTimer->start();
    }
    else
    {
        //        LogDebug("disable cpu load");
        //qDebug()<<"disable cpu load";
        cpuloadTimer->stop();
    }
    if(index==0)
    {
        //qDebug()<<"enable alg result";
        algresultTimer->start();
    }
    else
    {
        //qDebug()<<"disable alg result";
        algresultTimer->stop();
    }
}
void Widget::on_run_algsourceBox_activated(int index)
{
    if(index==0)
    {
        network->sendConfigToServer(NET_MSG_IMGALG_STATIC_IMG,1);

    }
    else
    {
        network->sendConfigToServer(NET_MSG_IMGAL_SENSOR_IMG,1);
    }
}

void Widget::frameFromSensor(uchar *data, int width,int height,int pitch)
{
    QString time =QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    //    FILE fp;
    //    QString filename = "D:/srcimg"+data+""
    //    fp=fopen("D:/srcimg"+time+".h","wb");
    //    uchar* pdata=data;
    //    for(int i=0;i<height;i++)
    //    {
    //        pdata =data+i*pitch;
    //        fwrite(pdata,1,width,fp);
    //    }
    //    fclose(fp);
    QImage frame(data,width,height,pitch,QImage::Format_Grayscale8);
    if(frame.save("./save/"+time+"_sensor.jpg"))
    {
        LogDebug("save image from sensor successfully.");
        qDebug()<<"save image from sensor successfully.";
    }
    else
    {
        LogDebug("failed to save image from sensor.");
        qDebug()<<"failed to save image from sensor.";
    }
}

void Widget::setDebugMode(int index)
{
    if(index==0)
    {
        network->sendConfigToServer(NET_MSG_DEBUG_NONE,1);
    }
    else if(index==1)
    {
        network->sendConfigToServer(NET_MSG_DEBUG_SERIAL,1);
    }
    else if(index==2)
    {
        network->setSocketDebugMode();
    }
}

void Widget::firmwareUpdateService(int cmd)
{
    QString localFilePath = "./update/";
    QString targetPath;
    switch(cmd)
    {
    case 0://FIRMWARE_C6DSP to xe674
    {
        targetPath=FIRMWARE_C6DSP;
        localFilePath += "ipnc_rdk_fw_c6xdsp.xe674";
        break;
    }
    case 1:
    {
        targetPath=FIRMWARE_VPSSM3;
        localFilePath += "ipnc_rdk_fw_m3vpss.xem3";
        break;
    }
    case 2:
    {
        targetPath=FIRMWARE_VIDEOM3;
        localFilePath += "ipnc_rdk_fw_m3video.xem3";
        break;
    }
    case 3:
    {
        targetPath=SDS_EZFTP_PATH;
        localFilePath += "Ezftp";
        break;
    }
    case 4:
    {
        targetPath=SDS_SERIAL_PATH;
        localFilePath += "EzApp";
        break;
    }
    case 5:
    {
        targetPath=SDS_MCFW_PATH;
        localFilePath += "ipnc_rdk_mcfw.out";
        break;
    }
    case 6:
    {
        targetPath=SDS_APP_SERVER;
        localFilePath += "system_server";
        break;
    }
    case 7:
    {
        targetPath=SDS_TEST_PATH;
        localFilePath += "test";
        break;
    }
    default:
    {
        qDebug()<<"unknown update cmd!";
        return;
    }
    }
    QFileInfo localFile(localFilePath);
    if(localFile.isFile())
    {
        FILE* fp=NULL;
        fp = fopen(localFilePath.toLatin1().data(),"rb");
        if(fp==NULL)
        {
            qDebug()<<"open failed";
            return;
        }
        int size = localFile.size();
        QByteArray* ba = new QByteArray;
        ba->resize(size);
        fread(ba->data(),1,size,fp);
        qDebug()<<ba->length();
        emit network->putFtp(targetPath,ba);
        fclose(fp);
    }
    else
    {
        qDebug()<<"could not find specified update file";
    }
}

void Widget::loadALGConfigUi(QVBoxLayout *layout, QPoint pos)
{

    ui->algorithm_settinglayout->addWidget(layout->parentWidget(),pos.x(),pos.y());
}

void Widget::loadALGResultUi(QVBoxLayout *layout, QPoint pos)
{
    QVBoxLayout* temp = (QVBoxLayout*)ui->run_resulttabWidget->widget(pos.x())->layout();
    temp->insertWidget(pos.y(),layout->parentWidget());
}

void Widget::on_algorithm_uploadButton_clicked()
{
    if(config->getAlgConfigSize()!=0)
    {
        void* param = (void*)malloc(config->getAlgConfigSize());
        config->getAlgConfig(param);
        network->sendConfigToServer(NET_MSG_IMGALG_SET_PARAM,param,config->getAlgConfigSize());
        free(param);
    }
    else
    {
        LogDebug("invalid ALG config size");
        qDebug()<<"invalid ALG config size";
    }
}

void Widget::on_run_algrunmodeBox_activated(int index)
{
    if(index==0)
    {
        network->sendConfigToServer(NET_MSG_IMGALG_NORMAL_MODE,1);
    }
    else
    {
        network->sendConfigToServer(NET_MSG_IMGALG_DEBUG_MODE,1);
    }
}



void Widget::on_button_Reboot_clicked()
{
    network->sendConfigToServer(NET_MSG_RESET_SYSTEM,1);
}

void Widget::on_diagnostic_downloadButton_clicked()
{
    ftpfile.FilePath=ui->diagnostic_dirLineEdit->text();
    if(ftpfile.FilePath=="")
    {
        qDebug()<<"invalid file path";
        return;
    }
    //   emit network->getFtp(ftpfile.ftpFileName);
    QString dir = ftpfile.FilePath+"/"+ftpfile.ftpFileName+".jpg";
    //    qDebug()<<dir<<ftpPreView->byteCount();
    ftpPreView->save(dir);

}

void Widget::on_diagnostic_ftpbrowsertablewidget_clicked(const QModelIndex &index)
{
    if(ui->diagnostic_ftpbrowsertablewidget->item(index.row(),0)->text()!="...")
    {
        QString name = ui->diagnostic_ftpbrowsertablewidget->item(index.row(),0)->text();
        ui->diagnostic_itemnamecurrentLabel->setText(name);
        ftpfile.ftpFileSize=ui->diagnostic_ftpbrowsertablewidget->item(index.row(),1)->text().toInt();
        ftpfile.ftpFileValid=ui->diagnostic_ftpbrowsertablewidget->item(index.row(),2)->text().toInt();
        ftpfile.ftpFileName=ui->diagnostic_ftpbrowsertablewidget->item(index.row(),0)->text();
        if(ui->diagnostic_ftpbrowsertablewidget->item(index.row(),2)->text().toInt()==1)
        {
            ui->diagnostic_previewWidget->clearImage(0);
            emit network->getFtp(name);
        }
    }
}

void Widget::on_diagnostic_dirButton_clicked()
{
    QString dstName = QFileDialog::getExistingDirectory (this,tr("Open Directory"),"/home",QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks ) ;
    if(dstName!="")
        ui->diagnostic_dirLineEdit->setText(dstName);

}

void Widget::on_diagnostic_ftpbrowsertablewidget_doubleClicked(const QModelIndex &index)
{
    QString dir = network->getFtpCurrentDir();
    if("..."==ui->diagnostic_ftpbrowsertablewidget->item(index.row(),0)->text())
    {

        if(dir==DEFAULT_PATH)
        {
            qDebug()<<"root";
            return;
        }
        else
        {
            while("/"!=dir.at(dir.length()-1))
            {
                dir.chop(1);
            }
            dir.chop(1);
            ui->diagnostic_ftpbrowsertablewidget->clearContents();
            ui->diagnostic_ftpbrowsertablewidget->setRowCount(0);
            emit network->listFtp(dir);
        }
    }
    else if(0==ui->diagnostic_ftpbrowsertablewidget->item(index.row(),2)->text().toInt())
    {
        dir.append("/");
        dir.append(ui->diagnostic_ftpbrowsertablewidget->item(index.row(),0)->text());
        ui->diagnostic_ftpbrowsertablewidget->clearContents();
        ui->diagnostic_ftpbrowsertablewidget->setRowCount(0);
        emit network->listFtp(dir);
    }
}

void Widget::on_system_firmwareupdateButton_clicked()
{
    if(!network->isFtpLogin())
    {
        NetworkStr networkconfigFromServer = network->getNetworkConfig();
        networkconfigFromServer.value.ports_ftpserverip="192.168.1.224";
        networkconfigFromServer.value.ports_ftpusername="root";
        networkconfigFromServer.value.ports_ftppassword="ftpcam";
        networkconfigFromServer.value.ports_ftpserverport="8010";
        emit network->connectFtp(networkconfigFromServer.value.ports_ftpserverip,networkconfigFromServer.value.ports_ftpserverport.toInt(),
                                 networkconfigFromServer.value.ports_ftpusername,networkconfigFromServer.value.ports_ftppassword);
    }
    updateWidget->show();
    updateWidget->move((QApplication::desktop()->width() - updateWidget->width())/2,(QApplication::desktop()->height() - updateWidget->height())/2);
}

void Widget::on_Account_camerascanButton_clicked()
{
    ui->Account_cameralist->clear();
    emit network->cameraScan();
}

void Widget::on_camera_2A_ApplyButton_clicked()
{
    EzCamH3AParam temp;
    temp.targetBrightness = ui->camera_2A_TargetBrightnesslineEdit->text().toInt();
    temp.targetBrightnessMax=ui->camera_2A_MaxBrightnesslineEdit->text().toInt();
    temp.targetBrightnessMin=ui->camera_2A_MinBrightnesslineEdit->text().toInt();
    temp.threshold = ui->camera_2A_ThresholdlineEdit->text().toInt();
    config->saveH3AParams(temp);
    QVariant h3aparampack;
    h3aparampack.setValue(temp);
    emit config->sendToServerH3A(h3aparampack);



}

void Widget::on_Account_cameralist_itemClicked(QListWidgetItem *item)
{
    QString currentip=item->text();
    currentip=currentip.remove(0,currentip.indexOf("\t")+1);
    //    qDebug()<<currentip;
    //    NetworkStr config = network->getNetworkConfig();
    //    config.value.ports_ipaddress = currentip;
    //    network->setNetworkConfig(config);
    ui->LoginSpecifiedIPlineEdit->setText(currentip);
}

void Widget::on_camera_2A_AEWeight_getButton_clicked()
{
    EzCamH3AWeight weight;
    network->GetParams(NET_MSG_GET_2A_WEIGHT,&weight,sizeof(EzCamH3AWeight));
    ui->camera_2A_AEWeight_preWidget->weightReflash(weight);
    ui->camera_2A_AEWeight_width1lineEdit->setText(QString::number(weight.width1,10));
    ui->camera_2A_AEWeight_height1lineEdit->setText(QString::number(weight.height1,10));
    ui->camera_2A_AEWeight_h_start2lineEdit->setText(QString::number(weight.h_start2,10));
    ui->camera_2A_AEWeight_v_start2lineEdit->setText(QString::number(weight.v_start2,10));
    ui->camera_2A_AEWeight_width2lineEdit->setText(QString::number(weight.width2,10));
    ui->camera_2A_AEWeight_height2lineEdit->setText(QString::number(weight.height2,10));
    ui->camera_2A_AEWeight_weightlineEdit->setText(QString::number(weight.weight,10));

}

void Widget::on_camera_2A_AEWeight_uploadButton_clicked()
{
    EzCamH3AWeight weight;
    weight.width1=ui->camera_2A_AEWeight_width1lineEdit->text().toInt();
    weight.height1=ui->camera_2A_AEWeight_height1lineEdit->text().toInt();
    weight.h_start2=ui->camera_2A_AEWeight_h_start2lineEdit->text().toInt();
    weight.v_start2=ui->camera_2A_AEWeight_v_start2lineEdit->text().toInt();
    weight.width2=ui->camera_2A_AEWeight_width2lineEdit->text().toInt();
    weight.height2=ui->camera_2A_AEWeight_height2lineEdit->text().toInt();
    weight.weight=ui->camera_2A_AEWeight_weightlineEdit->text().toInt();
    ConfigStr tempconfig = config->getConfig();
    tempconfig.calibrate.value.camera_2A_weight_width1 = ui->camera_2A_AEWeight_width1lineEdit->text().toInt();
    tempconfig.calibrate.value.camera_2A_weight_height1 = ui->camera_2A_AEWeight_height1lineEdit->text().toInt();
    tempconfig.calibrate.value.camera_2A_weight_h_start2 = ui->camera_2A_AEWeight_h_start2lineEdit->text().toInt();
    tempconfig.calibrate.value.camera_2A_weight_v_start2 = ui->camera_2A_AEWeight_v_start2lineEdit->text().toInt();
    tempconfig.calibrate.value.camera_2A_weight_width2 = ui->camera_2A_AEWeight_width2lineEdit->text().toInt();
    tempconfig.calibrate.value.camera_2A_weight_height2 = ui->camera_2A_AEWeight_height2lineEdit->text().toInt();
    tempconfig.calibrate.value.camera_2A_weight_weight = ui->camera_2A_AEWeight_weightlineEdit->text().toInt();
    config->SetConfig(tempconfig,CONFIG_SAVEONLY);
    network->sendConfigToServer(NET_MSG_SET_2A_WEIGHT,&weight,sizeof(EzCamH3AWeight));
}

void Widget::on_camera_2A_AEWeight_previewButton_clicked()
{
    EzCamH3AWeight weight;
    weight.width1=ui->camera_2A_AEWeight_width1lineEdit->text().toInt();
    weight.height1=ui->camera_2A_AEWeight_height1lineEdit->text().toInt();
    weight.h_start2=ui->camera_2A_AEWeight_h_start2lineEdit->text().toInt();
    weight.v_start2=ui->camera_2A_AEWeight_v_start2lineEdit->text().toInt();
    weight.width2=ui->camera_2A_AEWeight_width2lineEdit->text().toInt();
    weight.height2=ui->camera_2A_AEWeight_height2lineEdit->text().toInt();
    weight.weight=ui->camera_2A_AEWeight_weightlineEdit->text().toInt();
    ui->camera_2A_AEWeight_preWidget->weightReflash(weight);
}

void Widget::on_LoginSpecifiedIPButton_clicked()
{

    if(ui->LoginSpecifiedIPlineEdit->isHidden())
    {
        ui->LoginSpecifiedIPlineEdit->setVisible(true);
        ui->LoginSpecifiedIPLabel->setVisible(true);
        ui->LoginSpecifiedIPButton->setStyleSheet("QPushButton#LoginSpecifiedIPButton{border-image: url(:/image/source/uparrow.png);}"
                                                  "QPushButton#LoginSpecifiedIPButton:hover{border-image: url(:/image/source/uparrow_active.png);}"
                                                  "QPushButton#LoginSpecifiedIPButton:pressed{border-image: url(:/image/source/uparrow_active.png);}");
    }
    else
    {
        ui->LoginSpecifiedIPlineEdit->setVisible(false);
        ui->LoginSpecifiedIPLabel->setVisible(false);
        ui->LoginSpecifiedIPButton->setStyleSheet("QPushButton#LoginSpecifiedIPButton{border-image: url(:/image/source/downarrow.png);}"
                                                  "QPushButton#LoginSpecifiedIPButton:hover{border-image: url(:/image/source/downarrow_active.png);}"
                                                  "QPushButton#LoginSpecifiedIPButton:pressed{border-image: url(:/image/source/downarrow_active.png);}");
    }
}

void Widget::on_camera_2A_AEWeight_paintButton_clicked()
{
    if(ui->camera_2A_AEWeight_preWidget->currentMode()==MODE_CUSTOM)
    {
        ui->camera_2A_AEWeight_preWidget->editRecover();
        ui->camera_2A_AEWeight_preWidget->changeMode(MODE_DEFAULT);
    }
    else
        ui->camera_2A_AEWeight_preWidget->changeMode(MODE_CUSTOM);
}

void Widget::getH3AWeight(EzCamH3AWeight cfg)
{
    ui->camera_2A_AEWeight_width1lineEdit->editValue(cfg.width1);
    ui->camera_2A_AEWeight_height1lineEdit->editValue(cfg.height1);
    ui->camera_2A_AEWeight_h_start2lineEdit->editValue(cfg.h_start2);
    ui->camera_2A_AEWeight_v_start2lineEdit->editValue(cfg.v_start2);
    ui->camera_2A_AEWeight_width2lineEdit->editValue(cfg.width2);
    ui->camera_2A_AEWeight_height2lineEdit->editValue(cfg.height2);
    ui->camera_2A_AEWeight_weightlineEdit->editValue(cfg.weight);
}

void Widget::on_algorithm_setdefaultButton_clicked()
{
    void *temp = malloc(config->getAlgConfigSize());
    config->getAlgConfig(temp);
    network->sendConfigToServer(NET_MSG_IMGALG_DEF_PARAM,temp,config->getAlgConfigSize());
    free(temp);
}

