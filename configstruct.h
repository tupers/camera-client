#ifndef CONFIGSTRUCT_H
#define CONFIGSTRUCT_H
#include <QString>
#include "ezstream.h"
/*----------------------*/
#define H2AMODE_NONE     0
#define H2AMODE_AE       1
#define H2AMODE_AWB      2
#define H2AMODE_AEWB     3
#define H2AVENDOR_NONE   0
#define H2AVENDOR_APPRO  1
#define H2AVENDOR_TI     2
/*----------------------*/

enum _CONFIG_OPT
{
    CONFIG_SAVETOLOCAL=0,
    CONFIG_SAVEONLY
};

typedef struct RunNameStruct
{
    QString LoadAlg;
}RunNameStr;


typedef struct RunValueStruct
{
    bool LoadAlg;
}RunValueStr;

typedef struct RunStruct
{
    RunNameStr name;
    RunValueStr value;
}RunStr;

typedef struct
{
    QString ipRatio;
    QString fIframe;
    QString qpInit;
    QString qpMin;
    QString qpMax;
    QString meConfig;
    QString packetSize;
    QString unkown1;
    QString unkown2;
}CodecAdvParamName;

typedef struct
{
    QString Enable;
    QString pwmDuty;
}lightConfigName;

typedef struct
{
    unsigned int enable;
    unsigned int pwmduty;
}lightConfigValue;

typedef struct CalibrateNameStruct
{
    lightConfigName light_config[EZCAM_LED_NUM];
    QString camera_WhiteBalanceMode;
    QString camera_DayNightMode;
    QString camera_Binning;
    QString camera_BacklightControl;
    QString camera_Backlight;
    QString camera_Mirror;
    QString camera_ExpPriority;
    QString camera_Contrast;
    QString camera_Saturation;
    QString camera_Sharpness;
    QString camera_Exposure;
    QString camera_SensorGain;
    QString camera_PipeGain;
    QString camera_Brightness;
    QString camera_2AMode;
    QString camera_2AVendor;
    QString camera_histogram;
    QString camera_framectrl;
    QString camera_frameRateVal1;
    QString camera_frameRateVal2;
    QString camera_frameRateVal3;
    QString camera_VsEnable;
    QString camera_LdcEnable;
    QString camera_VnfEnable;
    QString camera_VnfMode;
    QString camera_VnfStrength;
    QString camera_dynRange;
    QString camera_dynRangeStrength;
    QString camera_2A_minExposure;
    QString camera_2A_maxExposure;
    QString camera_2A_stepSize;
    QString camera_2A_aGainMin;
    QString camera_2A_aGainMax;
    QString camera_2A_dGainMin;
    QString camera_2A_dGainMax;
    QString camera_2A_TargetBrightness;
    QString camera_2A_MaxBrightness;
    QString camera_2A_MinBrightness;
    QString camera_2A_Threshold;
    QString camera_2A_width;
    QString camera_2A_height;
    QString camera_2A_HCount;
    QString camera_2A_VCount;
    QString camera_2A_HStart;
    QString camera_2A_VStart;
    QString camera_2A_horzIncr;
    QString camera_2A_vertIncr;
    QString camera_2A_weight_width1;
    QString camera_2A_weight_height1;
    QString camera_2A_weight_h_start2;
    QString camera_2A_weight_v_start2;
    QString camera_2A_weight_width2;
    QString camera_2A_weight_height2;
    QString camera_2A_weight_weight;
    QString camera_video_StreamType;
    QString camera_video_VideoCodecMode;
    QString camera_video_JPEGQuality;
    QString camera_video_combo;
    QString camera_video_codecres;
    QString camera_video_frameRate1;
    QString camera_video_frameRate2;
    QString camera_video_frameRate3;
    QString camera_video_Mpeg41Bitrate;
    QString camera_video_Mpeg42Bitrate;
    QString camera_video_rateControl1;
    QString camera_video_rateControl2;
    QString camera_video_codectype1;
    QString camera_video_codectype2;
    QString camera_video_codectype3;
    CodecAdvParamName codec_advconfig[3];
}CalibrateNameStr;


typedef struct CalibrateValueStruct
{
    lightConfigValue light_config[EZCAM_LED_NUM];
    int camera_WhiteBalanceMode;
    int camera_DayNightMode;
    int camera_Binning;
    int camera_BacklightControl;
    int camera_Backlight;
    int camera_Mirror;
    int camera_ExpPriority;
    int camera_Contrast;
    int camera_Saturation;
    int camera_Sharpness;
    int camera_Exposure;
    int camera_SensorGain;
    int camera_PipeGain;
    int camera_Brightness;
    int camera_2AMode;
    int camera_2AVendor;
    int camera_histogram;
    int camera_framectrl;
    int camera_frameRateVal1;
    int camera_frameRateVal2;
    int camera_frameRateVal3;
    int camera_VsEnable;
    int camera_LdcEnable;
    int camera_VnfEnable;
    int camera_VnfMode;
    int camera_VnfStrength;
    int camera_dynRange;
    int camera_dynRangeStrength;
    int camera_2A_minExposure;
    int camera_2A_maxExposure;
    int camera_2A_stepSize;
    int camera_2A_aGainMin;
    int camera_2A_aGainMax;
    int camera_2A_dGainMin;
    int camera_2A_dGainMax;
    int camera_2A_TargetBrightness;
    int camera_2A_MaxBrightness;
    int camera_2A_MinBrightness;
    int camera_2A_Threshold;
    int camera_2A_width;
    int camera_2A_height;
    int camera_2A_HCount;
    int camera_2A_VCount;
    int camera_2A_HStart;
    int camera_2A_VStart;
    int camera_2A_horzIncr;
    int camera_2A_vertIncr;
    int camera_2A_weight_width1;
    int camera_2A_weight_height1;
    int camera_2A_weight_h_start2;
    int camera_2A_weight_v_start2;
    int camera_2A_weight_width2;
    int camera_2A_weight_height2;
    int camera_2A_weight_weight;
    int camera_video_StreamType;
    int camera_video_VideoCodecMode;
    int camera_video_JPEGQuality;
    int camera_video_combo;
    int camera_video_codecres;
    int camera_video_frameRate1;
    int camera_video_frameRate2;
    int camera_video_frameRate3;
    int camera_video_Mpeg41Bitrate;
    int camera_video_Mpeg42Bitrate;
    int camera_video_rateControl1;
    int camera_video_rateControl2;
    int camera_video_codectype1;
    int camera_video_codectype2;
    int camera_video_codectype3;
    CodecAdvParam codec_advconfig[3];
}CalibrateValueStr;

typedef struct CalibrateStruct
{
    CalibrateNameStr name;
    CalibrateValueStr value;
}CalibrateStr;

typedef struct ConnectionNameStruct
{
    QString digitalio_setting;
}ConnectionNameStr;

typedef struct ConnectionValueStruct
{
    int digitalio_setting;
}ConnectionValueStr;

typedef struct ConnectionStruct
{
    ConnectionNameStr name;
    ConnectionValueStr value;
}ConnectionStr;

typedef struct OptionsNameStruct
{
    QString security_authority;
    QString system_creator;
    QString system_changedby;
}OptionsNameStr;

typedef struct OptionsValueStruct
{
    AUTHORITY security_authority;
    QString system_creator;
    QString system_changedby;
}OptionsValueStr;

typedef struct OptionsStruct
{
    OptionsNameStr name;
    OptionsValueStr value;
}OptionsStr;

typedef struct InformationNameStruct
{
    QString resource_totalmem;
    QString resource_freemem;
    QString resource_sharedmem;
    QString resource_corearmmem;
    QString resource_coredspmem;
    QString resource_corem3vpssmem;
    QString resource_corem3videomem;
    QString resource_storagemem;
    QString resource_storagefreemem;
    QString resource_corearmfreq;
    QString resource_coredspfreq;
    QString resource_corem3vpssfreq;
    QString resource_corem3videofreq;
    QString device_version;
}InformationNameStr;

typedef struct InformationValueStruct
{
    int resource_totalmem;
    int resource_freemem;
    int resource_sharedmem;
    int resource_corearmmem;
    int resource_coredspmem;
    int resource_corem3vpssmem;
    int resource_corem3videomem;
    int resource_storagemem;
    int resource_storagefreemem;
    int resource_corearmfreq;
    int resource_coredspfreq;
    int resource_corem3vpssfreq;
    int resource_corem3videofreq;
    int device_version;
}InformationValueStr;

typedef struct InformationStruct
{
    InformationNameStr name;
    InformationValueStr value;
}InformationStr;

typedef struct NetworkNameStruct
{
    QString ports_ipaddress;
    QString ports_netmask;
    QString ports_gateway;
    QString ports_dns;
    QString ports_httpport;
    QString ports_httpsport;
    QString ports_dhcpenable;
    QString ports_dhcpconfig;
    QString ports_ntpserver;
    QString ports_ntptimezone;
    QString ports_mac;
    QString ports_ftpserverip;
    QString ports_ftpserverport;
    QString ports_ftpusername;
    QString ports_ftppassword;
    QString ports_ftpfoldername;
    QString ports_pid;
    QString ports_rftpenable;
    QString ports_ftpfileformat;
    QString ports_rtspurl;
    QString ports_rtspmulticast;
    QString ports_mtype;
    QString ports_rtspport[5];
    QString ports_sizeport;
    QString ports_rtspenable;
    QString ports_change;
}NetworkNameStr;

typedef struct NetworkValueStruct
{
    QString ports_ipaddress;
    QString ports_netmask;
    QString ports_gateway;
    QString ports_dns;
    unsigned short ports_httpport;
    unsigned short ports_httpsport;
    int ports_dhcpenable;
    int ports_dhcpconfig;
    QString ports_ntpserver;
    unsigned char ports_ntptimezone;
    QString ports_mac;
    QString ports_ftpserverip;
    QString ports_ftpserverport;
    QString ports_ftpusername;
    QString ports_ftppassword;
    QString ports_ftpfoldername;
    int ports_pid;
    unsigned char ports_rftpenable;
    unsigned char ports_ftpfileformat;
    QString ports_rtspurl;
    int ports_rtspmulticast;
    int ports_mtype;
    unsigned short ports_rtspport[5];
    int ports_sizeport;
    int ports_rtspenable;
    int ports_change;
}NetworkValueStr;

typedef struct NetworkStruct
{
    NetworkNameStr name;
    NetworkValueStr value;
}NetworkStr;

typedef struct
{
    unsigned int algType;
    unsigned int algTriggle;
    unsigned int algImgsrc;
    unsigned int enableEncode;
}ServerState;
typedef struct ConfigStruct
{
    RunStr run;
    CalibrateStr calibrate;
    ConnectionStr connection;
    OptionsStr options;
    InformationStr information;
    ServerState state;
}ConfigStr;

Q_DECLARE_METATYPE(NetworkNameStruct);
Q_DECLARE_METATYPE(NetworkValueStruct);
Q_DECLARE_METATYPE(NetworkStruct);
Q_DECLARE_METATYPE(RunNameStruct);
Q_DECLARE_METATYPE(RunValueStruct);
Q_DECLARE_METATYPE(RunStruct);
Q_DECLARE_METATYPE(CalibrateNameStruct);
Q_DECLARE_METATYPE(CalibrateValueStruct);
Q_DECLARE_METATYPE(CalibrateStruct);
Q_DECLARE_METATYPE(ConnectionNameStruct);
Q_DECLARE_METATYPE(ConnectionValueStruct);
Q_DECLARE_METATYPE(ConnectionStruct);
Q_DECLARE_METATYPE(OptionsNameStruct);
Q_DECLARE_METATYPE(OptionsValueStruct);
Q_DECLARE_METATYPE(OptionsStruct);
Q_DECLARE_METATYPE(InformationNameStruct);
Q_DECLARE_METATYPE(InformationValueStruct);
Q_DECLARE_METATYPE(InformationStruct);
Q_DECLARE_METATYPE(ConfigStruct);
Q_DECLARE_METATYPE(EzsdsAlgParam);
Q_DECLARE_METATYPE(EzCamH3AParam);
//Q_DECLARE_METATYPE(QHostAddress);
#endif // CONFIGSTRUCT_H
