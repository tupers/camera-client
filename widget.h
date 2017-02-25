#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QListWidget>
#include <QFontDatabase>
#include <QDebug>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QBitmap>
#include <QPainter>
#include <QtMath>
#include <QString>
#include <QDateTime>
#include <QTime>
#include <QStatusBar>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QHBoxLayout>
//#include <QtCharts/QChartView>
//#include <QtCharts/QPieSeries>
//#include <QtCharts/QPieSlice>
#include <QtCharts>
#include <QMenu>
#include <QWidgetAction>

#include "network.h"
#include "sourcevideo.h"
#include "h264video.h"
#include "ui_widget.h"
#include "configfile.h"
#include "subwidget/serialport.h"
#include "firmwareupdate.h"


#define MARGIN_L 50
#define MARGIN_S 5
//#define OFFLINE_DEBUG
QT_CHARTS_USE_NAMESPACE
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void InitGUI();
    void SetupAttachedWindow();
    void SetupList();//初始化菜单
    void SetupMouseEvent();//初始化鼠标事件
    void SetupRun();
    void SetupOptions();//初始化options界面
    void SetupCalibrate();//初始化Calibrate界面
    void SetupInformations();
    void SetupConnection();
    void UpdateConfigFile();//更新配置文件目录
    void LoadFullConfig();
    void LoadCalibrateConfig();
    void LoadRunConfig();
    void LoadConnectionConfig();
    void LoadInfomationConfig();
    void LoadAccountConfig();
    void SetupVideo();
    void SetupLog();
    void SetupNetwork();
    void SetupAccount();
    void VideoCMD(H264Video*,int);

private:
    Ui::Widget *ui;
    int countFlag(QPoint,int);//鼠标区域
    int countRow(QPoint);//鼠标区域
    QListWidgetItem* addListItem(QString,QListWidget*,QIcon icon);
    void freeListItem(QListWidgetItem*);
    QListWidgetItem* item_Calibrate;
    QListWidgetItem* item_Connection;
    QListWidgetItem* item_Run;
    QListWidgetItem* item_Options;
    QListWidgetItem* item_Information;
    bool isLeftPressed;
    QPoint pLast;
    int curPos;
    SerialPort *SerialPortWidget=NULL;
    ConfigFile* config=NULL;
    QThread *videothread;
    QThread *ftpthread;
    H264Video *h264video;
    SourceVideo *SourceVideoWidget=NULL;
    bool SourceFlag=false;
    QImage RunVideoImage;
    QStatusBar *logbar;
    QGraphicsOpacityEffect* effect;
    NetWork* network;
    QMenu* SaveasMenu=NULL;
    QComboBox* SaveasComboBox=NULL;
    QTimer* cpuloadTimer=NULL;
    QPieSeries *memoryseries=NULL;
    QPieSeries *storageseries=NULL;
    QLineSeries* cpuloadSeries=NULL;
    QLineSeries* cpuloadSeries2=NULL;
    QChart* cpuloadChart=NULL;
    QDateTimeAxis *axisX=NULL;
    QTimer* algresultTimer=NULL;
    EzAlgResult algResult;
    ftpSaveInfo ftpfile;
    FirmwareUpdate* updateWidget=NULL;
    void freeList();
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
private slots:
    void UpdateUserAccountList();
    void UpdateCameraList(int, QString);
    void resetFtpList();
    void UpdateFtpList(FileList*);
    void saveFtpData(QByteArray);
    void on_button_Account_clicked();
    void on_button_maxsize_clicked();
    void on_button_minsize_clicked();
    void on_LoginButton_clicked();
#ifdef OFFLINE_DEBUG
    void on_LogoutButton_clicked();
#endif
    void OpenSubWindow_SerialPort();
    void on_button_Save_clicked();
    void on_button_SaveAs_clicked();
    void on_system_configlist_clicked(const QModelIndex &index);
    void on_system_configloadButton_clicked();
    void ConfigSaveAs();
    void ConfigDelete();
    void UserAdd();
    void setVideoImage_Camera(QImage);
    void setVideoImage_Run(QImage);
    void setVideoInfo(int,int,int);
    void on_camera_videocontrolButton_clicked();
    void clearVideoImage();
    void LogDebug(QString);
    void logtimeout(QString);
    void LoginSuccess(uint8_t);
    void ConnectionLost();
    void recoverLoginState();
    void on_run_videocontrolButton_clicked();
    void on_run_videosourceButton_clicked();
    void SourceVideoWidgetClose();
    void on_run_processmodeLoadButton_clicked();
    void deleteUser();
    void on_system_configbootdefaultButton_clicked();
    void on_ports_setButton_clicked();
    void on_security_addusercontrolcancelButton_clicked();
    void sublistService(int);
    void cpuloadUpdate();
    void algresultUpdate();
    void camera_2AmodecomboBox_Service(int index);
    void on_run_algjogBox_activated(int index);
    void frameFromSensor(uchar*data,int width,int height,int pitch);
    void setDebugMode(int);
    void firmwareUpdateService(int);
    void loadALGConfigUi(QVBoxLayout*,QPoint);
    void loadALGResultUi(QVBoxLayout*,QPoint);

    void on_algorithm_uploadButton_clicked();

    void on_run_algrunmodeBox_activated(int index);

    void on_button_Reboot_clicked();

    void on_diagnostic_downloadButton_clicked();

    void on_diagnostic_ftpbrowsertablewidget_clicked(const QModelIndex &index);

    void on_diagnostic_dirButton_clicked();


    void on_diagnostic_ftpbrowsertablewidget_doubleClicked(const QModelIndex &index);

    void on_diagnostic_logftpButton_clicked();

    void on_system_firmwareupdateButton_clicked();
    void on_Account_camerascanButton_clicked();

    void on_camera_2A_ApplyButton_clicked();

    void on_Account_cameralist_itemClicked(QListWidgetItem *item);

    void on_camera_2A_AEWeight_getButton_clicked();

    void on_camera_2A_AEWeight_uploadButton_clicked();

    void on_camera_2A_AEWeight_previewButton_clicked();

    void on_LoginSpecifiedIPButton_clicked();

    void on_run_controlButton_clicked();

signals:
    void SubWindow_Init();
    void testsignal();
    void videocontrol(int);
    void getImage_Source(QImage);
    void getImage_Source(QImage,QRect,QRect,QRect);
    void NetworkConfigtoServer();
    void previewFtp(QString);
    void openFtp(NetworkStruct);
};

#endif // WIDGET_H
