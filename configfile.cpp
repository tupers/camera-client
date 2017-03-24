#include "configfile.h"

ConfigFile::ConfigFile()
{
    setConfigDir("./config/");
    qRegisterMetaType<QVariant>("QVariant");
    initDefaultConfig();
    algconfig=new ALGConfigService;
    connect(algconfig,SIGNAL(algConfigTag(QVBoxLayout*,QPoint)),this,SIGNAL(algConfigTag(QVBoxLayout*,QPoint)));
    connect(algconfig,SIGNAL(algResultTag(QVBoxLayout*,QPoint)),this,SIGNAL(algResultTag(QVBoxLayout*,QPoint)));
    connect(algconfig,SIGNAL(sendToLog(QString)),this,SIGNAL(sendToLog(QString)));
    //
//    algconfig->getParamsFromXml();
    //
}
ConfigFile::~ConfigFile()
{
    if(CurrentConfig!=NULL)
    {
        delete CurrentConfig;
        CurrentConfig=NULL;
    }
    if(algconfig!=NULL)
    {
        delete algconfig;
        algconfig=NULL;
    }
}

void ConfigFile::initDefaultConfig()
{
    QFileInfo ftemp(getConfigDir()+"default.ini");
    if(ftemp.isFile())
    {
        emit sendToLog("find default.ini");
        qDebug()<<"find default.ini";
    }
    else
    {
        emit sendToLog("create new default.ini");
        qDebug()<<"create new default.ini";
        CurrentConfig = new QSettings(getConfigDir()+"default.ini",QSettings::IniFormat);
        saveConfig(CurrentConfig);
        delete CurrentConfig;
        CurrentConfig=NULL;
    }
    //setGUI value
    //   delete CurrentConfig;
}
void ConfigFile::saveConfig(QSettings* conf)
{
    int i;
    for(i=0;i<EZCAM_LED_NUM;i++)
    {
        conf->setValue(config.calibrate.name.light_config[i].Enable,config.calibrate.value.light_config[i].enable);
        conf->setValue(config.calibrate.name.light_config[i].pwmDuty,config.calibrate.value.light_config[i].pwmduty);
    }
    conf->setValue(config.calibrate.name.camera_WhiteBalanceMode,config.calibrate.value.camera_WhiteBalanceMode);
    conf->setValue(config.calibrate.name.camera_DayNightMode,config.calibrate.value.camera_DayNightMode);
    conf->setValue(config.calibrate.name.camera_Binning,config.calibrate.value.camera_Binning);
    conf->setValue(config.calibrate.name.camera_BacklightControl,config.calibrate.value.camera_BacklightControl);
    conf->setValue(config.calibrate.name.camera_Backlight,config.calibrate.value.camera_Backlight);
    conf->setValue(config.calibrate.name.camera_Mirror,config.calibrate.value.camera_Mirror);
    conf->setValue(config.calibrate.name.camera_ExpPriority,config.calibrate.value.camera_ExpPriority);
    conf->setValue(config.calibrate.name.camera_Contrast,config.calibrate.value.camera_Contrast);
    conf->setValue(config.calibrate.name.camera_Saturation,config.calibrate.value.camera_Saturation);
    conf->setValue(config.calibrate.name.camera_Sharpness,config.calibrate.value.camera_Sharpness);
    conf->setValue(config.calibrate.name.camera_Exposure,config.calibrate.value.camera_Exposure);
    conf->setValue(config.calibrate.name.camera_SensorGain,config.calibrate.value.camera_SensorGain);
    conf->setValue(config.calibrate.name.camera_PipeGain,config.calibrate.value.camera_PipeGain);
    conf->setValue(config.calibrate.name.camera_Brightness,config.calibrate.value.camera_Brightness);
    conf->setValue(config.calibrate.name.camera_2AMode,config.calibrate.value.camera_2AMode);
    conf->setValue(config.calibrate.name.camera_2AVendor,config.calibrate.value.camera_2AVendor);
    conf->setValue(config.calibrate.name.camera_histogram,config.calibrate.value.camera_histogram);
    conf->setValue(config.calibrate.name.camera_framectrl,config.calibrate.value.camera_framectrl);
    conf->setValue(config.calibrate.name.camera_frameRateVal1,config.calibrate.value.camera_frameRateVal1);
    conf->setValue(config.calibrate.name.camera_frameRateVal2,config.calibrate.value.camera_frameRateVal2);
    conf->setValue(config.calibrate.name.camera_frameRateVal3,config.calibrate.value.camera_frameRateVal3);
    conf->setValue(config.calibrate.name.camera_VsEnable,config.calibrate.value.camera_VsEnable);
    conf->setValue(config.calibrate.name.camera_LdcEnable,config.calibrate.value.camera_LdcEnable);
    conf->setValue(config.calibrate.name.camera_VnfEnable,config.calibrate.value.camera_VnfEnable);
    conf->setValue(config.calibrate.name.camera_VnfMode,config.calibrate.value.camera_VnfMode);
    conf->setValue(config.calibrate.name.camera_VnfStrength,config.calibrate.value.camera_VnfStrength);
    conf->setValue(config.calibrate.name.camera_dynRange,config.calibrate.value.camera_dynRange);
    conf->setValue(config.calibrate.name.camera_dynRangeStrength,config.calibrate.value.camera_dynRangeStrength);
    conf->setValue(config.calibrate.name.camera_2A_minExposure,config.calibrate.value.camera_2A_minExposure);
    conf->setValue(config.calibrate.name.camera_2A_maxExposure,config.calibrate.value.camera_2A_maxExposure);
    conf->setValue(config.calibrate.name.camera_2A_stepSize,config.calibrate.value.camera_2A_stepSize);
    conf->setValue(config.calibrate.name.camera_2A_aGainMin,config.calibrate.value.camera_2A_aGainMin);
    conf->setValue(config.calibrate.name.camera_2A_aGainMax,config.calibrate.value.camera_2A_aGainMax);
    conf->setValue(config.calibrate.name.camera_2A_dGainMin,config.calibrate.value.camera_2A_dGainMin);
    conf->setValue(config.calibrate.name.camera_2A_dGainMax,config.calibrate.value.camera_2A_dGainMax);
    conf->setValue(config.calibrate.name.camera_2A_TargetBrightness,config.calibrate.value.camera_2A_TargetBrightness);
    conf->setValue(config.calibrate.name.camera_2A_MaxBrightness,config.calibrate.value.camera_2A_MaxBrightness);
    conf->setValue(config.calibrate.name.camera_2A_MinBrightness,config.calibrate.value.camera_2A_MinBrightness);
    conf->setValue(config.calibrate.name.camera_2A_Threshold,config.calibrate.value.camera_2A_Threshold);
    conf->setValue(config.calibrate.name.camera_2A_width,config.calibrate.value.camera_2A_width);
    conf->setValue(config.calibrate.name.camera_2A_height,config.calibrate.value.camera_2A_height);
    conf->setValue(config.calibrate.name.camera_2A_HCount,config.calibrate.value.camera_2A_HCount);
    conf->setValue(config.calibrate.name.camera_2A_VCount,config.calibrate.value.camera_2A_VCount);
    conf->setValue(config.calibrate.name.camera_2A_HStart,config.calibrate.value.camera_2A_HStart);
    conf->setValue(config.calibrate.name.camera_2A_VStart,config.calibrate.value.camera_2A_VStart);
    conf->setValue(config.calibrate.name.camera_2A_horzIncr,config.calibrate.value.camera_2A_horzIncr);
    conf->setValue(config.calibrate.name.camera_2A_vertIncr,config.calibrate.value.camera_2A_vertIncr);
    conf->setValue(config.calibrate.name.camera_2A_weight_width1,config.calibrate.value.camera_2A_weight_width1);
    conf->setValue(config.calibrate.name.camera_2A_weight_height1,config.calibrate.value.camera_2A_weight_height1);
    conf->setValue(config.calibrate.name.camera_2A_weight_h_start2,config.calibrate.value.camera_2A_weight_h_start2);
    conf->setValue(config.calibrate.name.camera_2A_weight_v_start2,config.calibrate.value.camera_2A_weight_v_start2);
    conf->setValue(config.calibrate.name.camera_2A_weight_width2,config.calibrate.value.camera_2A_weight_width2);
    conf->setValue(config.calibrate.name.camera_2A_weight_height2,config.calibrate.value.camera_2A_weight_height2);
    conf->setValue(config.calibrate.name.camera_2A_weight_weight,config.calibrate.value.camera_2A_weight_weight);
    conf->setValue(config.calibrate.name.camera_video_StreamType,config.calibrate.value.camera_video_StreamType);
    conf->setValue(config.calibrate.name.camera_video_VideoCodecMode,config.calibrate.value.camera_video_VideoCodecMode);
    conf->setValue(config.calibrate.name.camera_video_JPEGQuality,config.calibrate.value.camera_video_JPEGQuality);
    conf->setValue(config.calibrate.name.camera_video_combo,config.calibrate.value.camera_video_combo);
    conf->setValue(config.calibrate.name.camera_video_codecres,config.calibrate.value.camera_video_codecres);
    conf->setValue(config.calibrate.name.camera_video_frameRate1,config.calibrate.value.camera_video_frameRate1);
    conf->setValue(config.calibrate.name.camera_video_frameRate2,config.calibrate.value.camera_video_frameRate2);
    conf->setValue(config.calibrate.name.camera_video_frameRate3,config.calibrate.value.camera_video_frameRate3);
    conf->setValue(config.calibrate.name.camera_video_Mpeg41Bitrate,config.calibrate.value.camera_video_Mpeg41Bitrate);
    conf->setValue(config.calibrate.name.camera_video_Mpeg42Bitrate,config.calibrate.value.camera_video_Mpeg42Bitrate);
    conf->setValue(config.calibrate.name.camera_video_rateControl1,config.calibrate.value.camera_video_rateControl1);
    conf->setValue(config.calibrate.name.camera_video_rateControl2,config.calibrate.value.camera_video_rateControl2);
    conf->setValue(config.calibrate.name.camera_video_codectype1,config.calibrate.value.camera_video_codectype1);
    conf->setValue(config.calibrate.name.camera_video_codectype2,config.calibrate.value.camera_video_codectype2);
    conf->setValue(config.calibrate.name.camera_video_codectype3,config.calibrate.value.camera_video_codectype3);
    for(i=0;i<3;i++)
    {
        conf->setValue(config.calibrate.name.codec_advconfig[i].ipRatio,config.calibrate.value.codec_advconfig[i].ipRatio);
        conf->setValue(config.calibrate.name.codec_advconfig[i].fIframe,config.calibrate.value.codec_advconfig[i].fIframe);
        conf->setValue(config.calibrate.name.codec_advconfig[i].qpInit,config.calibrate.value.codec_advconfig[i].qpInit);
        conf->setValue(config.calibrate.name.codec_advconfig[i].qpMin,config.calibrate.value.codec_advconfig[i].qpMin);
        conf->setValue(config.calibrate.name.codec_advconfig[i].qpMax,config.calibrate.value.codec_advconfig[i].qpMax);
        conf->setValue(config.calibrate.name.codec_advconfig[i].meConfig,config.calibrate.value.codec_advconfig[i].meConfig);
        conf->setValue(config.calibrate.name.codec_advconfig[i].packetSize,config.calibrate.value.codec_advconfig[i].packetSize);
        conf->setValue(config.calibrate.name.codec_advconfig[i].unkown1,config.calibrate.value.codec_advconfig[i].unkown1);
        conf->setValue(config.calibrate.name.codec_advconfig[i].unkown2,config.calibrate.value.codec_advconfig[i].unkown2);
    }
    conf->setValue(config.connection.name.digitalio_setting,config.connection.value.digitalio_setting);
    conf->setValue(config.options.name.security_authority,config.options.value.security_authority);
    conf->setValue(config.options.name.system_creator,config.options.value.system_creator);
    conf->setValue(config.options.name.system_changedby,config.options.value.system_changedby);
    conf->setValue(config.information.name.device_version,config.information.value.device_version);
}
void ConfigFile::loadConfig(QSettings *conf)
{
    int i;
    for(i=0;i<EZCAM_LED_NUM;i++)
    {
        config.calibrate.value.light_config[i].enable=conf->value(config.calibrate.name.light_config[i].Enable).toUInt();
        config.calibrate.value.light_config[i].pwmduty=conf->value(config.calibrate.name.light_config[i].pwmDuty).toUInt();
    }
    config.calibrate.value.camera_WhiteBalanceMode=conf->value(config.calibrate.name.camera_WhiteBalanceMode).toInt();
    config.calibrate.value.camera_DayNightMode=conf->value(config.calibrate.name.camera_DayNightMode).toInt();
    config.calibrate.value.camera_Binning=conf->value(config.calibrate.name.camera_Binning).toInt();
    config.calibrate.value.camera_BacklightControl=conf->value(config.calibrate.name.camera_BacklightControl).toInt();
    config.calibrate.value.camera_Backlight=conf->value(config.calibrate.name.camera_Backlight).toInt();
    config.calibrate.value.camera_Mirror=conf->value(config.calibrate.name.camera_Mirror).toInt();
    config.calibrate.value.camera_ExpPriority=conf->value(config.calibrate.name.camera_ExpPriority).toInt();
    config.calibrate.value.camera_Contrast=conf->value(config.calibrate.name.camera_Contrast).toInt();
    config.calibrate.value.camera_Saturation=conf->value(config.calibrate.name.camera_Saturation).toInt();
    config.calibrate.value.camera_Sharpness=conf->value(config.calibrate.name.camera_Sharpness).toInt();
    config.calibrate.value.camera_Exposure=conf->value(config.calibrate.name.camera_Exposure).toInt();
    config.calibrate.value.camera_SensorGain=conf->value(config.calibrate.name.camera_SensorGain).toInt();
    config.calibrate.value.camera_PipeGain=conf->value(config.calibrate.name.camera_PipeGain).toInt();
    config.calibrate.value.camera_Brightness=conf->value(config.calibrate.name.camera_Brightness).toInt();
    config.calibrate.value.camera_2AMode=conf->value(config.calibrate.name.camera_2AMode).toInt();
    config.calibrate.value.camera_2AVendor=conf->value(config.calibrate.name.camera_2AVendor).toInt();
    config.calibrate.value.camera_histogram=conf->value(config.calibrate.name.camera_histogram).toInt();
    config.calibrate.value.camera_framectrl=conf->value(config.calibrate.name.camera_framectrl).toInt();
    config.calibrate.value.camera_frameRateVal1=conf->value(config.calibrate.name.camera_frameRateVal1).toInt();
    config.calibrate.value.camera_frameRateVal2=conf->value(config.calibrate.name.camera_frameRateVal2).toInt();
    config.calibrate.value.camera_frameRateVal3=conf->value(config.calibrate.name.camera_frameRateVal3).toInt();
    config.calibrate.value.camera_VsEnable=conf->value(config.calibrate.name.camera_VsEnable).toInt();
    config.calibrate.value.camera_LdcEnable=conf->value(config.calibrate.name.camera_LdcEnable).toInt();
    config.calibrate.value.camera_VnfEnable=conf->value(config.calibrate.name.camera_VnfEnable).toInt();
    config.calibrate.value.camera_VnfMode=conf->value(config.calibrate.name.camera_VnfMode).toInt();
    config.calibrate.value.camera_VnfStrength=conf->value(config.calibrate.name.camera_VnfStrength).toInt();
    config.calibrate.value.camera_dynRange=conf->value(config.calibrate.name.camera_dynRange).toInt();
    config.calibrate.value.camera_dynRangeStrength=conf->value(config.calibrate.name.camera_dynRangeStrength).toInt();
    config.calibrate.value.camera_2A_minExposure=conf->value(config.calibrate.name.camera_2A_minExposure).toInt();
    config.calibrate.value.camera_2A_maxExposure=conf->value(config.calibrate.name.camera_2A_maxExposure).toInt();
    config.calibrate.value.camera_2A_stepSize=conf->value(config.calibrate.name.camera_2A_stepSize).toInt();
    config.calibrate.value.camera_2A_aGainMin=conf->value(config.calibrate.name.camera_2A_aGainMin).toInt();
    config.calibrate.value.camera_2A_aGainMax=conf->value(config.calibrate.name.camera_2A_aGainMax).toInt();
    config.calibrate.value.camera_2A_dGainMin=conf->value(config.calibrate.name.camera_2A_dGainMin).toInt();
    config.calibrate.value.camera_2A_dGainMax=conf->value(config.calibrate.name.camera_2A_dGainMax).toInt();
    config.calibrate.value.camera_2A_TargetBrightness=conf->value(config.calibrate.name.camera_2A_TargetBrightness).toInt();
    config.calibrate.value.camera_2A_MaxBrightness=conf->value(config.calibrate.name.camera_2A_MaxBrightness).toInt();
    config.calibrate.value.camera_2A_MinBrightness=conf->value(config.calibrate.name.camera_2A_MinBrightness).toInt();
    config.calibrate.value.camera_2A_Threshold=conf->value(config.calibrate.name.camera_2A_Threshold).toInt();
    config.calibrate.value.camera_2A_width=conf->value(config.calibrate.name.camera_2A_width).toInt();
    config.calibrate.value.camera_2A_height=conf->value(config.calibrate.name.camera_2A_height).toInt();
    config.calibrate.value.camera_2A_HCount=conf->value(config.calibrate.name.camera_2A_HCount).toInt();
    config.calibrate.value.camera_2A_VCount=conf->value(config.calibrate.name.camera_2A_VCount).toInt();
    config.calibrate.value.camera_2A_HStart=conf->value(config.calibrate.name.camera_2A_HStart).toInt();
    config.calibrate.value.camera_2A_VStart=conf->value(config.calibrate.name.camera_2A_VStart).toInt();
    config.calibrate.value.camera_2A_horzIncr=conf->value(config.calibrate.name.camera_2A_horzIncr).toInt();
    config.calibrate.value.camera_2A_vertIncr=conf->value(config.calibrate.name.camera_2A_vertIncr).toInt();
    config.calibrate.value.camera_2A_weight_width1=conf->value(config.calibrate.name.camera_2A_weight_width1).toInt();
    config.calibrate.value.camera_2A_weight_height1=conf->value(config.calibrate.name.camera_2A_weight_height1).toInt();
    config.calibrate.value.camera_2A_weight_h_start2=conf->value(config.calibrate.name.camera_2A_weight_h_start2).toInt();
    config.calibrate.value.camera_2A_weight_v_start2=conf->value(config.calibrate.name.camera_2A_weight_v_start2).toInt();
    config.calibrate.value.camera_2A_weight_width2=conf->value(config.calibrate.name.camera_2A_weight_width2).toInt();
    config.calibrate.value.camera_2A_weight_height2=conf->value(config.calibrate.name.camera_2A_weight_height2).toInt();
    config.calibrate.value.camera_2A_weight_weight=conf->value(config.calibrate.name.camera_2A_weight_weight).toInt();
    config.calibrate.value.camera_video_StreamType=conf->value(config.calibrate.name.camera_video_StreamType).toInt();
    config.calibrate.value.camera_video_VideoCodecMode=conf->value(config.calibrate.name.camera_video_VideoCodecMode).toInt();
    config.calibrate.value.camera_video_JPEGQuality=conf->value(config.calibrate.name.camera_video_JPEGQuality).toInt();
    config.calibrate.value.camera_video_combo=conf->value(config.calibrate.name.camera_video_combo).toInt();
    config.calibrate.value.camera_video_codecres=conf->value(config.calibrate.name.camera_video_codecres).toInt();
    config.calibrate.value.camera_video_frameRate1=conf->value(config.calibrate.name.camera_video_frameRate1).toInt();
    config.calibrate.value.camera_video_frameRate2=conf->value(config.calibrate.name.camera_video_frameRate2).toInt();
    config.calibrate.value.camera_video_frameRate3=conf->value(config.calibrate.name.camera_video_frameRate3).toInt();
    config.calibrate.value.camera_video_Mpeg41Bitrate=conf->value(config.calibrate.name.camera_video_Mpeg41Bitrate).toInt();
    config.calibrate.value.camera_video_Mpeg42Bitrate=conf->value(config.calibrate.name.camera_video_Mpeg42Bitrate).toInt();
    config.calibrate.value.camera_video_rateControl1=conf->value(config.calibrate.name.camera_video_rateControl1).toInt();
    config.calibrate.value.camera_video_rateControl2=conf->value(config.calibrate.name.camera_video_rateControl2).toInt();
    config.calibrate.value.camera_video_codectype1=conf->value(config.calibrate.name.camera_video_codectype1).toInt();
    config.calibrate.value.camera_video_codectype2=conf->value(config.calibrate.name.camera_video_codectype2).toInt();
    config.calibrate.value.camera_video_codectype3=conf->value(config.calibrate.name.camera_video_codectype3).toInt();
    for(i=0;i<3;i++)
    {
        config.calibrate.value.codec_advconfig[i].ipRatio=conf->value(config.calibrate.name.codec_advconfig[i].ipRatio).toInt();
        config.calibrate.value.codec_advconfig[i].fIframe=conf->value(config.calibrate.name.codec_advconfig[i].fIframe).toInt();
        config.calibrate.value.codec_advconfig[i].qpInit=conf->value(config.calibrate.name.codec_advconfig[i].qpInit).toInt();
        config.calibrate.value.codec_advconfig[i].qpMin=conf->value(config.calibrate.name.codec_advconfig[i].qpMin).toInt();
        config.calibrate.value.codec_advconfig[i].qpMax=conf->value(config.calibrate.name.codec_advconfig[i].qpMax).toInt();
        config.calibrate.value.codec_advconfig[i].meConfig=conf->value(config.calibrate.name.codec_advconfig[i].meConfig).toInt();
        config.calibrate.value.codec_advconfig[i].packetSize=conf->value(config.calibrate.name.codec_advconfig[i].packetSize).toInt();
        config.calibrate.value.codec_advconfig[i].unkown1=conf->value(config.calibrate.name.codec_advconfig[i].unkown1).toInt();
        config.calibrate.value.codec_advconfig[i].unkown2=conf->value(config.calibrate.name.codec_advconfig[i].unkown2).toInt();
    }
    config.connection.value.digitalio_setting=conf->value(config.connection.name.digitalio_setting).toInt();
    config.options.value.security_authority=(USER_AUTHORITY)conf->value(config.options.name.security_authority).toInt();
    config.options.value.system_creator=conf->value(config.options.name.system_creator).toString();
    config.options.value.system_changedby=conf->value(config.options.name.system_changedby).toString();
    config.information.value.device_version=conf->value(config.information.name.device_version).toInt();
}

void ConfigFile::setContrast(int val)
{
    config.calibrate.value.camera_Contrast=val;
    emit sendToServer(NET_MSG_SET_CONTRAST,val);
}

void ConfigFile::setSaturation(int val)
{
    config.calibrate.value.camera_Saturation=val;
    emit sendToServer(NET_MSG_SET_SATURATION,val);
}


void ConfigFile::setSharpness(int val)
{
    config.calibrate.value.camera_Sharpness=val;
    emit sendToServer(NET_MSG_SET_SHARPNESS,val);
}

void ConfigFile::setExposure(int val)
{
    config.calibrate.value.camera_Exposure=val;
    emit sendToServer(NET_MSG_SET_EXPOSURE,val);
}

void ConfigFile::setSensorGain(int val)
{
    config.calibrate.value.camera_SensorGain=val;
    emit sendToServer(NET_MSG_SET_GAIN,val,config.calibrate.value.camera_PipeGain);
}

void ConfigFile::setPipeGain(int val)
{
    config.calibrate.value.camera_PipeGain=val;
    emit sendToServer(NET_MSG_SET_GAIN,config.calibrate.value.camera_SensorGain,val);
}

void ConfigFile::setBrightness(int val)
{
    config.calibrate.value.camera_Brightness=val;
    emit sendToServer(NET_MSG_SET_BRIGHTNESS,val);
}


void ConfigFile::set2AMode(int val)
{
    config.calibrate.value.camera_2AMode=val;
    emit sendToServer(NET_MSG_SET_IMG2ATYPE,val);
}

void ConfigFile::set2AVendor(int val)
{
    config.calibrate.value.camera_2AVendor=val;
    //    emit sendToServer(NET_MSG_SET_IMG2A,val);
}

void ConfigFile::setWhiteBalanceMode(int val)
{
    config.calibrate.value.camera_WhiteBalanceMode=val;
    emit sendToServer(NET_MSG_SET_WB,val);
}

void ConfigFile::setDayNightMode(int val)
{
    config.calibrate.value.camera_DayNightMode=val;
    emit sendToServer(NET_MSG_SET_DAYNIGHTMODE,val);
}

void ConfigFile::setBinning(int val)
{
    config.calibrate.value.camera_Binning=val;
    emit sendToServer(NET_MSG_SET_BINNING,val);
}

void ConfigFile::setMirror(int val)
{
    config.calibrate.value.camera_Mirror=val;
    emit sendToServer(NET_MSG_SET_MIRROR,val);
}

void ConfigFile::setExpPriority(int val)
{
    config.calibrate.value.camera_ExpPriority=val;
    emit sendToServer(NET_MSG_SET_EXPRIORITY,val);
}

void ConfigFile::setStreamType(int val)
{
    config.calibrate.value.camera_video_StreamType=val;
}

void ConfigFile::setVideoCodecMode(int val)
{
    config.calibrate.value.camera_video_VideoCodecMode=val;
}

void ConfigFile::setJPEGQuality(int val)
{
    config.calibrate.value.camera_video_JPEGQuality=val;
}

void ConfigFile::setLoadAlg(int val)
{

    config.run.value.LoadAlg=QVariant(val).toBool();
    if(val==2)
        emit sendToServer(NET_MSG_IMGALG_ENABLE,val);
    else
        emit sendToServer(NET_MSG_IMGALG_DISABLE,val);
}

void ConfigFile::setVsEnable(int val)
{
    config.calibrate.value.camera_VsEnable=val;
    emit sendToServer(NET_MSG_SET_VIDSTAB,val);
}

void ConfigFile::setLDCEnable(int val)
{
    config.calibrate.value.camera_LdcEnable=val;
    emit sendToServer(NET_MSG_SET_LDC,val);
}

void ConfigFile::setVnfEnable(int val)
{
    config.calibrate.value.camera_VnfEnable=val;
    emit sendToServer(NET_MSG_SET_VNF,val);
}

void ConfigFile::setVnfMode(int val)
{
    config.calibrate.value.camera_VnfMode=val;
    emit sendToServer(NET_MSG_SET_VNF_MODE,val);
}

void ConfigFile::setLightConfig(int val)
{
    QString obName=sender()->objectName();
    if(obName=="light_led1pwmlineEdit")
        config.calibrate.value.light_config[0].pwmduty=val;
    else if(obName=="light_led2pwmlineEdit")
        config.calibrate.value.light_config[1].pwmduty=val;
    else if(obName=="light_led1enablecomboBox")
        config.calibrate.value.light_config[0].enable =val;
    else if(obName=="light_led2enablecomboBox")
        config.calibrate.value.light_config[1].enable=val;
    QVariant lightConfig;
    lightConfig.setValue(config.calibrate.value);
    emit sendToServerLightConfig(lightConfig);

}


void ConfigFile::SaveAsConfig(QString name,QString creator)
{
    OpenConfig(name);
    config.options.value.system_creator=creator;
    config.options.value.system_changedby=creator;
    saveConfig(CurrentConfig);
}
void ConfigFile::UpdateConfig(QString changedby)
{
    config.options.value.system_changedby=changedby;
    saveConfig(CurrentConfig);
}
void ConfigFile::OpenConfig(QString name)
{
    if(CurrentConfig!=NULL)
    {
        delete CurrentConfig;
        CurrentConfig=NULL;
    }
    CurrentConfig = new QSettings(getConfigDir()+name+".ini",QSettings::IniFormat);
    setCurrentConfigName(name);
}

void ConfigFile::SetConfig()
{
    if(CurrentConfig!=NULL)
    {
        loadConfig(CurrentConfig);
        QVariant configEntire;
        configEntire.setValue(config);
        emit sendToServerEntire(configEntire);
        emit sendToLog("load config file: "+CurrentConfigName);
        qDebug()<<"load config file: "+CurrentConfigName;
    }
    else
    {
        emit sendToLog("SetConfig failed.");
        qDebug()<<"SetConfig failed.";
    }
}

void ConfigFile::SetConfig(ConfigStr tempconfig, _CONFIG_OPT opt)
{
    if(CurrentConfig!=NULL)
    {
        config.calibrate.value=tempconfig.calibrate.value;
        config.run.value=tempconfig.run.value;
        config.connection.value=tempconfig.connection.value;
        config.information.value=tempconfig.information.value;
        config.state=tempconfig.state;
        if(opt==CONFIG_SAVETOLOCAL)
        {
            saveConfig(CurrentConfig);
        emit sendToLog("Fill config structure with given structure, and save it to "+CurrentConfigName);
        qDebug()<<"Fill config structure with given structure, and save it to "+CurrentConfigName;
        }
        else
        {
            emit sendToLog("Fill config structure with given structure.");
            qDebug()<<"Fill config structure with given structure.";
        }
    }
    else
    {
        emit sendToLog("SetConfig failed.");
        qDebug()<<"SetConfig failed.";
    }
}

QString ConfigFile::ConfigCreator()
{
    if(CurrentConfig!=NULL)
        return CurrentConfig->value(config.options.name.system_creator).toString();
    else
        return "";
}

QString ConfigFile::ConfigChangedby()
{
    if(CurrentConfig!=NULL)
        return CurrentConfig->value(config.options.name.system_changedby).toString();
    else
        return "";
}

void ConfigFile::setConigAsBoot()
{

    QVariant configEntire;
    ConfigStruct temp;
    temp=config;
    loadConfig(CurrentConfig);
    configEntire.setValue(config);
    config=temp;
    emit sendToServerEntireForBoot(configEntire);
}

void ConfigFile::saveH3AParams(EzCamH3AParam src)
{
    config.calibrate.value.camera_2A_TargetBrightness=src.targetBrightness;
    config.calibrate.value.camera_2A_MaxBrightness=src.targetBrightnessMax;
    config.calibrate.value.camera_2A_MinBrightness=src.targetBrightnessMin;
    config.calibrate.value.camera_2A_Threshold=src.threshold;
}

void ConfigFile::initAlgService()
{
    algconfig->initConfigList(ALG_TYPE(config.state.algType));
//    algconfig->initConfigList(ALG_GVSS);
}

void ConfigFile::setAlgConfig(void *params)
{
    algconfig->setConfig(params);
    algconfig->loadConfig();
}

void ConfigFile::getAlgConfig(void *params)
{
    algconfig->saveConfig();
    algconfig->getConfig(params);
}

void ConfigFile::reflashAlgResult(void *params)
{
    algconfig->reflashResult(params);
}

QImage ConfigFile::refreshAlgImage()
{
    return algconfig->resultImage();
}




