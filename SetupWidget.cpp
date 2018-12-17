#include "SetupWidget.h"
#include "ui_SetupWidget.h"

#include <QModelIndex>
#include <QGridLayout>

#include "SetupWizard/FirmwarePage.h"
#include "QuadApplication.h"
#include "VehicleManager.h"

SetupWidget::SetupWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetupWidget)
  , _firmwarePage(NULL)
{
    ui->setupUi(this);
    this->setStyleSheet("QPushButton {background: #cfe2f3}"
                        "QPushButton:pressed {background: #6699cc}");

    _firmwarePage = new FirmwarePage();

    ui->stackedWidget->addWidget(_firmwarePage);
}

SetupWidget::~SetupWidget()
{
    delete ui;
}

void SetupWidget::on_pushButton_Firmware_clicked()
{
   ui->stackedWidget->setCurrentWidget(_firmwarePage);
}
