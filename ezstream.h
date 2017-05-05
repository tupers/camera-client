#ifndef EZSTREAM_H
#define EZSTREAM_H
#include <QtNetwork>
#include <qnetwork.h>
#define EZ_SERVER_STREAM_PORT		3000
#define EZ_SERVERPORT               7000
#define EZ_SERVERRESULTPORT         9800
#define EZ_SERVERIP                 "192.168.1.224"



/***************** **************************
 server information
***********************************************/
typedef struct
{
    unsigned short x;
    unsigned short y;
}pair;

typedef struct
{
    unsigned short flag[4];
    unsigned short size;
    unsigned short num;
    unsigned short area[40];
    unsigned short pad;
    pair xyPair[40];
    float position;
    float distance;
} testResult;
typedef struct
{
    unsigned int area;
    unsigned short x;
    unsigned short y;
}coord;

typedef struct
{
    unsigned int blockNum;
    coord block[40];
    float position;
    float distance;
}uiAlgResult;

enum _NET_MSG
{
    /**System Page*/
    NET_MSG_LOGIN = 0,
    NET_MSG_GETUSER,
    NET_MSG_ADDUSER,
    NET_MSG_DELUSER,
    NET_MSG_GETSYSINFO,
    NET_MSG_SETSYSINFO,
    NET_MSG_UPDATE_DEFAULTPARAM,

    /**Network Page*/
    NET_MSG_SET_DHCP,
    NET_MSG_GET_DHCP,
    NET_MSG_SET_NETIP ,
    NET_MSG_GET_NETIP ,
    NET_MSG_SET_NETMASK ,
    NET_MSG_GET_NETMASK ,
    NET_MSG_SET_GTWAY ,
    NET_MSG_GET_GTWAY ,
    NET_MSG_NET_UPDATE,

    /**Camera Page */
    NET_MSG_SET_BRIGHTNESS ,
    NET_MSG_GET_BRIGHTNESS ,
    NET_MSG_SET_CONTRAST ,
    NET_MSG_GET_CONTRAST ,
    NET_MSG_SET_SATURATION ,
    NET_MSG_GET_SATURATION ,
    NET_MSG_SET_SHARPNESS ,
    NET_MSG_GET_SHARPNESS ,
    NET_MSG_SET_EXPOSURE,
    NET_MSG_GET_EXPOSURE,
    NET_MSG_SET_GAIN,
    NET_MSG_GET_GAIN,
    NET_MSG_SET_BLC ,
    NET_MSG_GET_BLC ,
    NET_MSG_SET_DYRANGE ,
    NET_MSG_GET_DYRANGE ,
    NET_MSG_SET_DYRANGESTRENGTH ,
    NET_MSG_GET_DYRANGESTRENGTH ,
    NET_MSG_SET_IMAGESOURCE ,
    NET_MSG_GET_IMAGESOURCE ,
    NET_MSG_SET_WB ,
    NET_MSG_GET_WB ,
    NET_MSG_SET_DAYNIGHTMODE,
    NET_MSG_GET_DAYNIGHTMODE,
    NET_MSG_SET_BINNING ,
    NET_MSG_GET_BINNING ,
    NET_MSG_SET_IMG2A ,
    NET_MSG_GET_IMG2A ,
    NET_MSG_SET_IMG2ATYPE ,
    NET_MSG_GET_IMG2ATYPE ,
    NET_MSG_SET_EXPRIORITY ,
    NET_MSG_GET_EXPRIORITY ,
    NET_MSG_SET_2A_PARAM,
    NET_MSG_GET_2A_PARAM,
    NET_MSG_SET_2A_WEIGHT,
    NET_MSG_GET_2A_WEIGHT,
    NET_MSG_SET_VIDSTAB ,
    NET_MSG_GET_VIDSTAB ,
    NET_MSG_SET_LDC ,
    NET_MSG_GET_LDC ,
    NET_MSG_SET_MIRROR,
    NET_MSG_GET_MIRROR,
    NET_MSG_SET_VNF,
    NET_MSG_GET_VNF,
    NET_MSG_SET_VNF_MODE ,
    NET_MSG_GET_VNF_MODE ,
    NET_MSG_SET_VNFSTRENGTH ,
    NET_MSG_GET_VNFSTRENGTH ,
    NET_MSG_SET_FRAMECTR ,
    NET_MSG_GET_FRAMECTR,
    NET_MSG_RESET_PAYLOAD,
    NET_MSG_START_PAYLOAD,
    NET_MSG_PRINT_PAYLOAD,
    NET_MSG_GET_PAYLOAD,

    NET_MSG_SET_LED,
    NET_MSG_GET_LED,

    /****IMG ALG*****/
    NET_MSG_GET_ALG_RESULT,
    NET_MSG_IMGALG_ENABLE,
    NET_MSG_IMGALG_DISABLE,
    NET_MSG_IMGALG_STATIC_IMG,
    NET_MSG_IMGAL_SENSOR_IMG,
    NET_MSG_IMGALG_SET_PARAM,
    NET_MSG_IMGALG_GET_PARAM,
    NET_MSG_IMGALG_DEF_PARAM,
    NET_MSG_IMGALG_NORMAL_MODE,
    NET_MSG_IMGALG_DEBUG_MODE,

    NET_MSG_GET_FRAME_IMG,
    NET_MSG_GET_ENABLE_ENCODE,
    NET_MSG_GET_DISABLE_ENCODE,

    NET_MSG_DEBUG_NONE,
    NET_MSG_DEBUG_SERIAL,
    NET_MSG_DEBUG_SOCKET,
    NET_MSG_DEBUG_QUIT,
    NET_MSG_RESET_SYSTEM

};


typedef struct __DebugParams
{
    unsigned port;
    char ip[20];
}DebugParams;

typedef struct __EzFrameInfo
{
    unsigned int udpPort;
    unsigned char udpIp[20];
    unsigned int width;
    unsigned int height;
    unsigned int pitch;
}EzFrameInfo;

#define ACOUNT_NUM				16 		///< How many acounts which are stored in system.
#define USER_LEN				32 		///< Maximum of acount username length.
#define MIN_USER_LEN			4 		///< Maximum of acount username length.
#define PASSWORD_LEN			16 		///< Maximum of acount password length.
#define MIN_PASSWORD_LEN		4 		///< Maximum of acount password length.
#define IP_STR_LEN				20		///< IP string length
#define CIRNUM_MAX              5       ///< ALG cir num


typedef enum USER_AUTHORITY{
    AUTHORITY_ADMIN = 0,
    AUTHORITY_VIEWER
} AUTHORITY;

typedef struct _AdasBlobT
{
    int id;
    int left;
    int right;
    int up;
    int low;
    int area;
    int Total255;
}AdasBlobT;

typedef struct
{
    ////    unsigned int cirnum;
    ////    EzAlgInfo info[CIRNUM_MAX];
    //    unsigned char iImgAlgDone;
    //    unsigned char nStatusCode;
    //    unsigned char nWarningCode;
    //    //圆心用于计算
    //    float   Cir1_PixelX;//将下面6个变量类型Uint改为float cjs20120911
    //    float   Cir1_PixelY;
    //    float   Cir2_PixelX;
    //    float   Cir2_PixelY;
    //    float   Cir3_PixelX;
    //    float   Cir3_PixelY;
    //    int     ErrorIndex;
    unsigned char nStatusCode;
    unsigned char nWarningCode;

    //圆心用于计算
    float   Cir1_PixelX;//将下面6个变量类型Uint改为float cjs20120911
    float   Cir1_PixelY;
    float   Cir2_PixelX;
    float   Cir2_PixelY;
    float   Cir3_PixelX;
    float   Cir3_PixelY;
    float   Cir4_PixelX;
    float   Cir4_PixelY;
    float   Cir5_PixelX;
    float   Cir5_PixelY;

    //重心用于显示
    float   GCir1_PixelX;
    float   GCir1_PixelY;
    float   GCir2_PixelX;
    float   GCir2_PixelY;
    float   GCir3_PixelX;
    float   GCir3_PixelY;
    float   GCir4_PixelX;
    float   GCir4_PixelY;
    float   GCir5_PixelX;
    float   GCir5_PixelY;

    //圆心用于显示
    float   CCir1_PixelX;
    float   CCir1_PixelY;
    float   CCir2_PixelX;
    float   CCir2_PixelY;
    float   CCir3_PixelX;
    float   CCir3_PixelY;
    float   CCir4_PixelX;
    float   CCir4_PixelY;
    float   CCir5_PixelX;
    float   CCir5_PixelY;
    //半径用于显示
    float   Cir1_r;
    float   Cir2_r;
    float   Cir3_r;
    float   Cir4_r;
    float   Cir5_r;
    //半径误差用于显示
    float   Cir1_f;
    float   Cir2_f;
    float   Cir3_f;
    float   Cir4_f;
    float   Cir5_f;

    //Block搜多框大小
    AdasBlobT Cir1_bt;
    AdasBlobT Cir2_bt;
    AdasBlobT Cir3_bt;
    AdasBlobT Cir4_bt;
    AdasBlobT Cir5_bt;

    //Uint    nTimeStamp;
    //FILETIME nTimeStamp;
    unsigned int nTimeStamp;
    int     ErrorIndex;
    unsigned char 	threshold[5];
    unsigned char   iImgAlgDone;
}EzAlgResult;

typedef struct
{
    unsigned int prf_a8;
    unsigned int prf_dsp;
}EzcpuDynamicParam;

/********************************/



#define SYS_ENV_SIZE		sizeof(SysInfo)
#define LOG_ENTRY_SIZE		sizeof(LogEntry_t)
#define MAX_DOMAIN_NAME			40 		///< Maximum length of domain name. Ex: www.xxx.com
#define MAX_LANCAM_TITLE_LEN	11 		///< Maximum length of LANCAM title.
#define MAC_LENGTH				6 		///< Length of MAC address.
#define MAX_FILE_NAME			128		///< Maximum length of file name.
#define ACOUNT_NUM			16 		///< How many acounts which are stored in system.
#define USER_LEN			32 		///< Maximum of acount username length.
#define MIN_USER_LEN			4 		///< Maximum of acount username length.
#define PASSWORD_LEN			16 		///< Maximum of acount password length.
#define MIN_PASSWORD_LEN		4 		///< Maximum of acount password length.
#define PASSWORD_ZERO           	0               ///< minimum length of password>
#define IP_STR_LEN			20		///< IP string length
#define MAX_IP_ADDR     		30
#define RTSPURL_MAX_LEN			100
#define MAX_CORE                4
#define MAX_CODEC_STREAMS		3 		///< Maximum length of normal string.

#define __u8    unsigned char
#define __u16   unsigned short
#define __u32   unsigned int

/**
* @brief IPCAM user account data.
*/
typedef struct
{
    char	user[USER_LEN+1];			///< username
    char	password[PASSWORD_LEN+1];	///< password
    __u8	authority;				///< user authority
}Acount_t;

typedef struct
{
    float ROI_startX;
    float ROI_endX;
    float ROI_startY;
    float ROI_endY;
    float f_RDifSideCir;
    float fCirRadius_1;
    float fCirRadius_2;
    float fCirGood_1;
    float fCirGood_2;
    float fCirGood_3;
    float fCirWarning_1;
    float fCirWarning_2;
    float fCirWarning_3;
    float fCenterCirGoodOffset;
    float fCenterCirWarningOffset;
    unsigned int nBiggestRaduis;
    unsigned int nSmallestRaduis;
    unsigned int Blot_BiggestArea;
    unsigned int Blot_SmallestArea;
    unsigned int Blot_xyOffset;
    unsigned int Cir_SmallestArea;
    unsigned int Cir_xyOffset;
    float Cir_errf;
    unsigned int Judge_xyOffset;
    float Speed_k1;
    float Speed_k2;
    unsigned int Speed_ExpandPixel1;
    unsigned int Speed_ExpandPixel2;
}EzsdsAlgParam;


typedef struct
{
        long mtype;
        unsigned short rtspServerPort_H264_MPEG4_MJPEG[5];
        int size_Port;
        int RTSP_Enable;
        int change;
        __u8 multicast_enable; ///< multicast enable
        char rtspUrl[RTSPURL_MAX_LEN+1];
} rtspServerPort;


/**
* @brief Infomation about ftp configuration.
*/
typedef struct
{
    char		servier_ip[MAX_DOMAIN_NAME+1]; ///< FTP server address
    char		username[USER_LEN+1]; ///< FTP login username
    char		password[PASSWORD_LEN+1]; ///< FTP login password
    char		foldername[MAX_FILE_NAME]; ///< FTP upload folder
//    int			image_count; ///< Image count
    int			pid; ///< PID
    __u16		port; ///< FTP port
    __u8		rftpenable; ///< RFTP enable
    __u8        ftpfileformat; ///< file format
} Ftp_Config_Data;

/* End-Onvif Structures */

#define IP_STR_MAX 16

/**
* @brief Infomation of network status and user settings.
*/
typedef struct
{
//    struct in_addr	ip; ///< IP address in static IP mode
    char ip[IP_STR_MAX];
    char netmask[IP_STR_MAX];
    char gateway[IP_STR_MAX];
    char dns[IP_STR_MAX];
//    struct in_addr	netmask; ///< netmask in static IP mode
//    struct in_addr	gateway; ///< gateway in static IP mode
//    struct in_addr	dns; ///< DNS IP in static IP mode
    __u16		http_port; ///< HTTP port in web site.
    __u16		https_port; ///< HTTP port in web site.
    int			dhcp_enable; ///< current DHCP status.
    int			dhcp_config; ///< DHCP config
    char		ntp_server[MAX_DOMAIN_NAME+1]; ///< NTP server FQDN
    __u8		ntp_timezone; ///< current time zone
    __u8		MAC[MAC_LENGTH]; ///< hardware MAC address

} Network_Config_Data;

/**
* @brief Infomation of network status and user settings.
*/
typedef struct
{
    Network_Config_Data	net_config;
    Ftp_Config_Data		ftp_config;
    rtspServerPort		rtsp_config;
}Network_Config;

typedef struct
{
    int width1;
    int height1;
    int h_start2;
    int v_start2;
    int width2;
    int height2;
    int weight;
}EzCamH3AWeight;

typedef struct
{
    unsigned int width;
    unsigned int height;
    unsigned int pitch;
    unsigned int imgInfoSize;
    unsigned char imgInfo[0];
}EzImgFileHeader;

typedef struct
{
    unsigned char Alg_saveImgdirName[10];
    unsigned char Alg_saveImgIndex;
    unsigned int errNo;
}EzImgFileInfo;

/**
* @brief event log data structure.
*/
typedef struct LogEntry_t{
    char event[50];		///< event description
}LogEntry_t;

typedef struct {
    unsigned int minExposure;                                       /* Mininum Sensor
                                                            * exposure */
    unsigned int maxExposure;                                       /* Maximum Sensor
                                                            * exposure */
    unsigned int stepSize;                                          /* step size for
                                                            * the exposure
                                                            * variation */
    unsigned int aGainMin;                                          /* Minimum analog
                                                            * Gain */
    unsigned int aGainMax;                                          /* Maximum Analog
                                                            * gain */
    unsigned int dGainMin;                                          /* Minimum digital
                                                            * gain */
    unsigned int dGainMax;                                          /* Maximum Digital
                                                            * gain */
    unsigned int targetBrightnessMin;                               /* Minimum target
                                                            * bright ness */
    unsigned int targetBrightnessMax;                               /* Maximum target
                                                            * bright ness */
    unsigned int targetBrightness;                                  /* target
                                                            * brightness to
                                                            * be achieved */
    unsigned int threshold;

    /** weight **/
    unsigned int width;
    unsigned int height;
    unsigned int HCount;
    unsigned int VCount;
    unsigned int HStart;
    unsigned int VStart;
    unsigned int horzIncr;
    unsigned int vertIncr;
}EzCamH3AParam;



typedef struct
{
    __u8			vsEnable;
    __u8			ldcEnable;
    __u8			vnfEnable;
    __u8			vnfMode;
    __u8			vnfStrength;
    __u8			dynRange;						///< Dynamic Range switch ON/OFF
    __u8			dynRangeStrength;				///< Dynamic Range Strength
} Advanced_Mode;

typedef struct
{

    unsigned int 	sensorGain;
    unsigned int 	pipeGain;
    unsigned int 	exposure;
    unsigned char	nWhiteBalance;					///< white balance mode
    unsigned char	nDayNight;						///< Daynight mode
    unsigned char	nBinning;						///< binning/skipping setting
    unsigned char	nBacklightControl;				///< backlight control setting
    unsigned char	nBackLight;						///< backlight value
    unsigned char	nBrightness;					///< brightness value
    unsigned char	nContrast;						///< contrast value
    unsigned char	nSaturation;					///< saturation value
    unsigned char	nSharpness;						///< sharpness value
    unsigned char	mirror; 						///< mirroring
    unsigned char	nAEWswitch;						///< 2A engine selection
    unsigned char	nAEWtype;						///< 2A engine selection
    unsigned char	expPriority;
    unsigned char	histogram;						///< Histogram switch ON/OFF
    unsigned char	framectrl;
    int				frameRateVal1;
    int				frameRateVal2;
    int				frameRateVal3;
    EzCamH3AParam H3AParams;
    EzCamH3AWeight H3AWeight;
    Advanced_Mode	advanceMode;
}Cam_Config_Data;



typedef struct
{
    __u32 ipRatio;
    __u8 fIframe;
    __u8 qpInit;
    __u8 qpMin;
    __u8 qpMax;
    __u8 meConfig;
    __u8 packetSize;
    __u8 unkown1;
    __u8 unkown2;
}CodecAdvParam;
typedef struct
{
    __u8 			nStreamType;
    __u8			nVideocodecmode;				///< selected codec mode
    __u8			nVideocombo;
    __u8			nVideocodecres;					///< selected codec resolution
    __u8			nFrameRate1;					///< MPEG4_1 frame rate
    __u8			nFrameRate2;					///< MPEG4_2 frame rate
    __u8			nFrameRate3;					///< JPEG frame rate
    __u8			njpegquality; 					///< jpeg quality
    int				nMpeg41bitrate;					///< MPEG4_1 bitrate
    int				nMpeg42bitrate;					///< MPEG4_2 bitrate
    __u8			nRateControl1;					///< stream1 bitrate control setting
    __u8			nRateControl2;					///< stream2 bitrate control setting
    __u8			codectype1;
    __u8			codectype2;
    __u8			codectype3;
    CodecAdvParam codec_advconfig[MAX_CODEC_STREAMS];
}Codec_Config_Data;

typedef struct
{
    __u8 version;
    unsigned int total_mem;						//MB
    unsigned int free_mem;						//MB
    unsigned int share_mem;						//MB
    unsigned int core_mem[MAX_CORE];			//MB
    unsigned int storage_mem;					//MB
    unsigned int storage_free_mem;				//MB
    unsigned int core_freq[MAX_CORE];			//MHZ
}CamSystemInfo;

#define EZCAM_LED_NUM 2

typedef struct
{
    unsigned int enable;
    unsigned int pwmduty;
}FillLightConfig;
typedef struct
{
    FillLightConfig led[EZCAM_LED_NUM];
}CamLight_Config;

typedef struct
{
    unsigned int algType;
    unsigned int algTriggle;
    unsigned int algImgsrc;
    unsigned int enableEncode;
}CamState;

/**
* @brief system info main data structure.
*/
typedef struct SysInfo{
    Network_Config	net;
    Cam_Config_Data lan_config;
    Codec_Config_Data codec_config;
    CamSystemInfo	camInfo;
    CamLight_Config	light_config;
    CamState	 state;
}EzSysInfo;

/************************************/

typedef struct
{
    int userCount;
    Acount_t user[ACOUNT_NUM];
}get_userdata_t;

#define NET_MSG_MAX     2048
#define ACKCMD_SOK      0
#define ACKCMD_SFAIL    -1
typedef struct _NetMsg
{
    char cmd;
    int  ackCmd;
    int  cmdValueLen;
    char cmdValue[0];
}NetMsg;

#define MSG_SOK         0
#define MSG_SFAIL       -1
#define MSG_OVERTIME    -2
#define MSG_NETERROR    -3
#define MSG_BUFOVERFLOW -4

#define SCAN_SERVER_PORT    8002

#define SCAN_MAGIC1         0x14
#define SCAN_MAGIC2         0x23

#define IDENTICAL_STRING    "ezcamera"

typedef struct NetCamScanData
{
    unsigned char magic[2];
    char identical[9];
    unsigned int CamId;
    unsigned int crc;
}NetCamScanData;

#endif // EZSTREAM_H
