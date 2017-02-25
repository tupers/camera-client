#ifndef FIRMWAREUPDATE_H
#define FIRMWAREUPDATE_H

#include <QWidget>

namespace Ui {
class FirmwareUpdate;
}

class FirmwareUpdate : public QWidget
{
    Q_OBJECT

public:
    explicit FirmwareUpdate(QWidget *parent = 0);
    ~FirmwareUpdate();

private slots:
    void on_comboBox_activated(int index);

    void on_pushButton_clicked();

private:
    Ui::FirmwareUpdate *ui;
    int updateoption=0;
signals:
    void updateOption(int cmd);
};

#endif // FIRMWAREUPDATE_H
