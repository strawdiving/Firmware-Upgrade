#ifndef SETUPWIDGET_H
#define SETUPWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QListWidget>
#include <QPushButton>

#include "TitleLabel.h"

class Vehicle;

class FirmwarePage;

namespace Ui {
class SetupWidget;
}

class SetupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SetupWidget(QWidget *parent = 0);
    ~SetupWidget();

signals:

public slots:

private slots:
     void on_pushButton_Firmware_clicked();

private:
      Ui::SetupWidget *ui;

      FirmwarePage* _firmwarePage;

     const QString _armedText = "This operation cannot be performed while vehicle is armed.";
};

#endif // SETUPWIDGET_H
