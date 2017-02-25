#ifndef FTP_CMDLIST_H
#define FTP_CMDLIST_H

#define MAX_USERNAME 32
#define MAX_PASSWD 32
#define MIN_USERNAME 4
#define MIN_PASSWD 4
#define MAX_FILE_PATH 160
#define DEFAULT_PATH "sds"

#define MSG_SOK         0
#define MSG_SFAIL       -1
#define MSG_OVERTIME    -2
#define MSG_NETERROR    -3
#define MSG_BUFOVERFLOW -4

#define ACKCMD_SOK      0
#define ACKCMD_SFAIL    -1

#define FIRMWARE_C6DSP  "/opt/ipnc/firmware/ipnc_rdk_fw_c6xdsp/xe674"
#define FIRMWARE_VPSSM3 "/opt/ipnc/firmware/ipnc_rdk_fw_m3vpss.xem3"
#define FIRMWARE_VIDEOM3 "/opt/ipnc/firmware/ipnc_rdk_fw_m3video.xem3"
#define SDS_EZFTP_PATH  "/opt/ipnc/Ezftp"
#define SDS_SERIAL_PATH "/opt/ipnc/SdsSerial"
#define SDS_MCFW_PATH   "/opt/ipnc/bin/ipnc_rdk_mcfw.out"
#define SDS_APP_SERVER  "/opt/ipnc/system_server"
#define SDS_TEST_PATH   "/opt/test"



typedef enum _EzFtpAck_Status
{
    FTP_ACK_SOK=0,
    FTP_ACK_SFAIL
}EzFtpAck_Status;


#define MAGIC_HEADER1 0x159753
#define MAGIC_HEADER2 0x456183
typedef struct _EzFtp_Header
{

    unsigned int magic1;
    unsigned int magic2;
    unsigned int cmd;
    unsigned int len;
    unsigned int pad;
    int status;
}EzFtp_Header;


typedef struct _EzFtp_Ack
{
    int status;
    unsigned int len;
}EzFtp_Ack;

enum _FTP_MSG
{
    FTP_MSG_LOGIN=0,
    FTP_MSG_CLOSE,
    FTP_MSG_LIST,
    FTP_MSG_PUT,
    FTP_MSG_GET
};

typedef struct _EzFtp_File
{
    unsigned int port;
    char ip[20];
    char filepath[MAX_FILE_PATH];
    unsigned int fileSize;
}EzFtp_File;

typedef struct _EzFtp_Account
{
    char UserName[MAX_USERNAME+1];
    char Passwd[MAX_PASSWD+1];

}EZFtp_Account;


typedef struct _EzFtp_NetMsg
{
    char cmd;
    int  ackCmd;
    int  cmdValueLen;
    char cmdValue[0];
}EzFtp_NetMsg;

#define DIR_TYPE 0
#define FILE_TYPE 1
#define FILE_NAME_MAX_SIZE 32

typedef struct __FILE_INFO
{
    unsigned int type;
    char name[FILE_NAME_MAX_SIZE];
    unsigned int size;
}FileInfo;

typedef struct __FILE_LIST
{
    unsigned int dirCount;
    FileInfo finfo[0];
}FileList;

enum Server_STATE
{
    Server_Disconnected,
    Server_Loggedin
};

Q_DECLARE_METATYPE(__FILE_LIST);
Q_DECLARE_METATYPE(__FILE_LIST*);
#endif // FTP_CMDLIST_H
