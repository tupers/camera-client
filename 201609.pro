#-------------------------------------------------
#
# Project created by QtCreator 2016-09-18T16:53:05
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += charts
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DM8127Client
RC_ICONS = myappico.ico
TEMPLATE = app
LIBS += $$PWD/ffmpeg/lib/libavcodec.dll.a \
        $$PWD/ffmpeg/lib/libavfilter.dll.a \
        $$PWD/ffmpeg/lib/libavformat.dll.a \
        $$PWD/ffmpeg/lib/libswscale.dll.a \
        $$PWD/ffmpeg/lib/libavutil.dll.a \
#       D:/Qt/Tools/mingw530_32/i686-w64-mingw32/lib/libwsock32.a\

#LIBS += $$PWD/ffmpeg/lib/avcodec.lib\
#        $$PWD/ffmpeg/lib/avfilter.lib\
#        $$PWD/ffmpeg/lib/avformat.lib\
#        $$PWD/ffmpeg/lib/avutil.lib\
#        $$PWD/ffmpeg/lib/swscale.lib\


SOURCES += main.cpp\
        widget.cpp \
    subwidget/serialport.cpp \
    console.cpp \
    subwidget/spconsole.cpp \
    configfile.cpp \
    lineedit.cpp \
    h264video.cpp \
    sourcevideo.cpp \
    network.cpp \
    udpservice.cpp \
    dm8127ftp_service.cpp \
    ftpdata.cpp \
    firmwareupdate.cpp \
    dm8127scan_service.cpp \
    weightwidget.cpp \
    algconfigservice.cpp \
    imagewidget.cpp \
    multiimagewidget.cpp \
    resultservice.cpp \
    algConfig/alg_gvss.cpp \
    utils/fpscnt.cpp \
    utils/rtchart.cpp

HEADERS  += widget.h \
    subwidget/serialport.h \
    console.h \
    subwidget/spconsole.h \
    configfile.h \
    lineedit.h \
    h264video.h \
    ezstream.h \
    network.h \
    configstruct.h \
    udpservice.h \
    dm8127ftp_service.h \
    ftpdata.h \
    ftp_cmdlist.h \
    firmwareupdate.h \
    dm8127scan_service.h \
    weightwidget.h \
    algconfigstr.h \
    algconfigservice.h \
    imagewidget.h \
    multiimagewidget.h \
    resultservice.h \
    algConfig/alg_gvss.h \
    utils/fpscnt.h \
    utils/rtchart.h \
    sourcevideo.h

FORMS    += widget.ui \
    subwidget/serialport.ui \
    sourcevideo.ui \
    firmwareupdate.ui

RESOURCES += \
    source.qrc

INCLUDEPATH += $$PWD/ffmpeg/include


