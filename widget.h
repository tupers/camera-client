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
#include "multiimagewidget.h"
#include "utils/algdebug.h"
#include "utils/video_core.h"

#define MARGIN_L 50
#define MARGIN_S 5

#define DEFAULT_IMG_WIDTH   1280
#define DEFAULT_IMG_HEIGHT  720

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
    void SetupList();//??�???????
    void SetupMouseEvent();//??�???�???�?�?
    void SetupNetwork();
    void SetupLog();
    void SetupVideo();

    void SetupRun();
    void SetupOptions();//??�???options????
    void SetupCalibrate();//??�???Calibrate????
    void SetupInformations();
    void SetupConnection();
    void SetupAccount();

    void UpdateConfigFile();//?��?��??置�??件�?��?
    void LoadFullConfig();
    void LoadCalibrateConfig();
    void LoadRunConfig();
    void LoadConnectionConfig();
    void LoadInfomationConfig();
    void LoadAccountConfig();
    void ResetRun();
    void ResetOptions();
    void ResetInformation();
    void ResetAccount();
    //ftp
    void ResetFtpBrowser();
    void VideoCMD(H264Video*,int);
private:
    Ui::Widget *ui;
    int countFlag(QPoint,int);//�????��??
    int countRow(QPoint);//�????��??
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
    video_core *m_hVideo;
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
    RTChart* cpuChart;
    EzAlgResult algResult;
//    ftpSaveInfo ftpfile;
    FirmwareUpdate* updateWidget=NULL;
    MultiImageWidget* camera_videoinputwidget;
    QImage* ftpPreView=NULL;
    void freeList();
    bool isRunOnScreen;
    FILE* resultFile=NULL;
    FILE* resultLogFile = NULL;
    algDebug* m_hLocalAlgDebug;
    video_bufQueue* m_hBufQueue;
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *event);
private slots:
    void UpdateUserAccountList();
    void UpdateCameraList(int, QString);
    void UpdateFtpList(QByteArray);
    void saveFtpData(QByteArray);
    void on_button_Account_clicked();
    void on_button_maxsize_clicked();
    void on_button_minsize_clicked();
    void on_LoginButton_clicked();
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
    void algresultUpdate(QByteArray);
    void camera_2AmodecomboBox_Service(int index);
    void on_run_algsourceBox_activated(int index);
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

    void on_system_firmwareupdateButton_clicked();
    void on_Account_camerascanButton_clicked();

    void on_camera_2A_ApplyButton_clicked();

    void on_Account_cameralist_itemClicked(QListWidgetItem *item);

    void on_camera_2A_AEWeight_getButton_clicked();

    void on_camera_2A_AEWeight_uploadButton_clicked();

    void on_camera_2A_AEWeight_previewButton_clicked();

    void on_LoginSpecifiedIPButton_clicked();

    void on_camera_2A_AEWeight_paintButton_clicked();
    void getH3AWeight(EzCamH3AWeight);
    void on_algorithm_setdefaultButton_clicked();
    void resultLog(int);
    void on_diagnostic_local_loadButton_clicked();

    void on_diagnostic_deleteButton_clicked();

    void on_diagnostic_saveErrImgCheckBox_stateChanged(int arg1);

signals:
    void SubWindow_Init();
    void testsignal();
    void videocontrol(int);
    void getImage_Source(QImage,int);
    void NetworkConfigtoServer();
    void previewFtp(QString);
    void openFtp(NetworkStruct);
};

#endif // WIDGET_H
