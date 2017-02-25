#ifndef CONFIGFILE_H
#define CONFIGFILE_H
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include <QTableWidget>
#include <QFileInfoList>
#include <QDir>

#include "configstruct.h"
#include "algconfigservice.h"
class ConfigFile:public QObject
{
    Q_OBJECT
public:
    explicit ConfigFile();
    ~ConfigFile();
    void initDefaultConfig();
    void setConfigDir(QString dir){ConfigDir = dir;}
    QString getConfigDir(){return ConfigDir;}
    void SaveAsConfig(QString,QString creator="default");
    void UpdateConfig(QString changedby="default");
    void OpenConfig(QString);
    void SetConfig();
    void SetConfig(ConfigStr, _CONFIG_OPT opt =CONFIG_SAVETOLOCAL);
    QString ConfigCreator();
    QString ConfigChangedby();
    QString getCurrentConfigName(){return CurrentConfigName;}
    ConfigStr getConfig(){return config;}
    CalibrateStr getCalibrate(){return config.calibrate;}
    RunStr getRun(){return config.run;}
    ConnectionStr getConnection(){return config.connection;}
    OptionsStr getOptions(){return config.options;}
    InformationStr getInformation(){return config.information;}
    ServerState getState(){return config.state;}
    void setConigAsBoot();
    void saveH3AParams(EzCamH3AParam src);
    void initAlgService();
    void setAlgConfig(void*params);
    void getAlgConfig(void*params);
    int getAlgConfigSize(){return algconfig->getConfigSize();}
    int getAlgResultSize(){return algconfig->getResultSize();}
    void reflashAlgResult(void*);

private:
    void saveConfig(QSettings*);
    void loadConfig(QSettings*);
    void setCurrentConfigName(QString name){CurrentConfigName=name;}
    QSettings* CurrentConfig=NULL;
    QString ConfigDir;
    QString CurrentConfigName="";
    ALGConfigService* algconfig=NULL;
    ConfigStr config=
    {
        {
            {"Run/LoadALG"},
            {true}
        },
        {
            {
                {
                    "Calibrate/light_led1",
                    "Calibrate/light_led2",
                    "Calibrate/light_led3",
                    "Calibrate/light_led4",
                    "Calibrate/light_led5",
                    "Calibrate/light_led6",
                    "Calibrate/light_led7",
                    "Calibrate/light_led8"
                },
                "Calibrate/camera_WhiteBalanceMode",
                "Calibrate/camera_DayNightMode",
                "Calibrate/camera_Binning",
                "Calibrate/camera_BacklightControl",
                "Calibrate/camera_Backlight",
                "Calibrate/camera_Mirror",
                "Calibrate/camera_ExpPriority",
                "Calibrate/camera_Contrast",
                "Calibrate/camera_Saturation",
                "Calibrate/camera_Sharpness",
                "Calibrate/camera_Exposure",
                "Calibrate/camera_SensorGain",
                "Calibrate/camera_PipeGain",
                "Calibrate/camera_Brightness",
                "Calibrate/camera_2AMode",
                "Calibrate/camera_2AVendor",
                "Calibrate/camera_histogram",
                "Calibrate/camera_framectrl",
                "Calibrate/camera_frameRateVal1",
                "Calibrate/camera_frameRateVal2",
                "Calibrate/camera_frameRateVal3",
                "Calibrate/camera_VsEnable",
                "Calibrate/camera_LdcEnable",
                "Calibrate/camera_VnfEnable",
                "Calibrate/camera_VnfMode",
                "Calibrate/camera_VnfStrength",
                "Calibrate/camera_dynRange",
                "Calibrate/camera_dynRangeStrength",
                "Calibrate/camera_2A_minExposure",
                "Calibrate/camera_2A_maxExposrte",
                "Calibrate/camera_2A_stepSize",
                "Calibrate/camera_2A_aGainMin",
                "Calibrate/camera_2A_aGainMax",
                "Calibrate/camera_2A_dGainMin",
                "Calibrate/camera_2A_dGainMax",
                "Calibrate/camera_2A_TargetBrightness",
                "Calibrate/camera_2A_MaxBrightness",
                "Calibrate/camera_2A_MinBrightness",
                "Calibrate/camera_2A_Threshold",
                "Calibrate/camera_2A_width",
                "Calibrate/camera_2A_height",
                "Calibrate/camera_2A_HCount",
                "Calibrate/camera_2A_VCount",
                "Calibrate/camera_2A_HStart",
                "Calibrate/camera_2A_VStart",
                "Calibrate/camera_2A_horzIncr",
                "Calibrate/camera_2A_vertIncr",
                "Calibrate/camera_2A_weight_width1",
                "Calibrate/camera_2A_weight_height1",
                "Calibrate/camera_2A_weight_h_start2",
                "Calibrate/camera_2A_weight_v_start2",
                "Calibrate/camera_2A_weight_width2",
                "Calibrate/camera_2A_weight_height2",
                "Calibrate/camera_2A_weight_weight",
                "Calibrate/camera_video_StreamType",
                "Calibrate/camera_video_VideoCodecMode",
                "Calibrate/camera_video_JPEGQuality",
                "Calibrate/camera_video_Combo",
                "Calibrate/camera_video_Codecres",
                "Calibrate/camera_video_FrameRate1",
                "Calibrate/camera_video_FrameRate2",
                "Calibrate/camera_video_FrameRate3",
                "Calibrate/camera_video_Mpeg41BitRate",
                "Calibrate/camera_video_Mpeg42BitRate",
                "Calibrate/camera_video_RateControl1",
                "Calibrate/camera_video_RateControl2",
                "Calibrate/camera_video_codecType1",
                "Calibrate/camera_video_codecType2",
                "Calibrate/camera_video_codecType3",
                {
                    {"Calibrate/camera_video_adv1_ipRatio",
                    "Calibrate/camera_video_adv1_fIframe",
                    "Calibrate/camera_video_adv1_qpInit",
                    "Calibrate/camera_video_adv1_qpMin",
                    "Calibrate/camera_video_adv1_qpMax",
                    "Calibrate/camera_video_adv1_meConfig",
                    "Calibrate/camera_video_adv1_packetSize",
                    "Calibrate/camera_video_adv1_unkown1",
                    "Calibrate/camera_video_adv1_unkown2"},
                    {"Calibrate/camera_video_adv2_ipRatio",
                     "Calibrate/camera_video_adv2_fIframe",
                     "Calibrate/camera_video_adv2_qpInit",
                     "Calibrate/camera_video_adv2_qpMin",
                     "Calibrate/camera_video_adv2_qpMax",
                     "Calibrate/camera_video_adv2_meConfig",
                     "Calibrate/camera_video_adv2_packetSize",
                     "Calibrate/camera_video_adv2_unkown1",
                     "Calibrate/camera_video_adv2_unkown2"},
                    {"Calibrate/camera_video_adv3_ipRatio",
                     "Calibrate/camera_video_adv3_fIframe",
                     "Calibrate/camera_video_adv3_qpInit",
                     "Calibrate/camera_video_adv3_qpMin",
                     "Calibrate/camera_video_adv3_qpMax",
                     "Calibrate/camera_video_adv3_meConfig",
                     "Calibrate/camera_video_adv3_packetSize",
                     "Calibrate/camera_video_adv3_unkown1",
                     "Calibrate/camera_video_adv3_unkown2"}
                }
            },
            {
                {
                  0,0,0,0,0,0,0,0
                },
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                127,
                127,
                127,
                50,
                50,
                50,
                127,
                0,
                2,
                0,
                0,
                0,
                0,
                0,
                0,//advance
                0,
                0,
                0,
                0,
                0,
                0,
                0,//2a
                0,
                0,
                0,
                0,
                0,
                0,
                40,//2a target
                50,
                30,
                40,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,//video
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                {
                    {
                        0,0,0,0,0,0,0,0,0
                    },
                    {
                        0,0,0,0,0,0,0,0,0
                    },
                    {
                        0,0,0,0,0,0,0,0,0
                    }
                }
            }

        },
        {
            {
                "Connection/digitalio_setting"},
            {
                255}
        },
        {
            {"Options/security_authority","Options/system_creator","Options/system_changedby"},
            {AUTHORITY_ADMIN,"default","default"}
        },
        {
            {"Information/resource_totalmem",
             "Information/resource_freemem",
             "Information/resource_sharedmem",
             "Information/resource_ARMmem",
             "Information/resource_DSPmem",
             "Information/resource_M3VPSSmem",
             "Information/resource_M3VIDEOmem",
             "Information/resource_storagemem",
             "Information/resource_storagefreemem",
             "Information/resource_ARMfreq",
             "Information/resource_DSPfreq",
             "Information/resource_M3VPSSfreq",
             "Information/resource_M3VIDEOfreq",
             "Information/device_version"},
            {
                2048,
                0,
                0,
                0,
                0,
                1024,
                1024,
                512,
                128,
                750,
                750,
                750,
                750,
                1000
            }
        },
        {
            0,
            0,
            0,
            0
        }
    };
private slots:
    void setContrast(int val);
    void setSaturation(int val);
    void setSharpness(int val);
    void setExposure(int val);
    void setSensorGain(int val);
    void setPipeGain(int val);
    void setBrightness(int val);
    void set2AMode(int val);
    void set2AVendor(int val);
    void setWhiteBalanceMode(int val);
    void setDayNightMode(int val);
    void setBinning(int val);
    void setMirror(int val);
    void setExpPriority(int val);
    void setStreamType(int val);
    void setVideoCodecMode(int val);
    void setJPEGQuality(int val);
    void setLoadAlg(int val);
    void setVsEnable(int val);
    void setLDCEnable(int val);
    void setVnfEnable(int val);
    void setVnfMode(int val);
signals:
    void sendToServer(_NET_MSG, QString);
    void sendToServer(_NET_MSG, unsigned char);
    void sendToServer(_NET_MSG, unsigned char ,unsigned char);
    void sendToServerEntire(QVariant);
    void sendToServerALG(QVariant);
    void sendToServerH3A(QVariant);
    void sendToServerEntireForBoot(QVariant);
    void sendToLog(QString);
    void algConfigTag(QVBoxLayout*,QPoint);
    void algResultTag(QVBoxLayout*,QPoint);

};

#endif // CONFIGFILE_H
