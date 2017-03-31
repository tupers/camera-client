#include "widget.h"
#include <QApplication>
void logMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();
    QDir savedir;
    savedir.mkdir("./log/");
    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("[Debug]");
        break;

    case QtWarningMsg:
        text = QString("[Warning]");
        break;

    case QtCriticalMsg:
        text = QString("[Critical]");
        break;

    case QtFatalMsg:
        text = QString("[Fatal]");
        break;
    default:
        text = QString("[Others]");
    }
    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString current_date = QString("[%1]").arg(current_date_time);
    QFile f("./log/log.txt");
    f.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&f);
    text_stream <<text<<"\t"<<context_info<<"\t"<< msg <<current_date<< "\r\n";
    f.flush();
    f.close();

    mutex.unlock();
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile f("./log/log.txt");
    if(f.exists())
        f.remove();
//    qInstallMessageHandler(logMessage);
    Widget w;
    w.show();

    return a.exec();
}
