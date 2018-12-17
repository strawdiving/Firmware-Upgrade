#-------------------------------------------------
#
# Project created by QtCreator 2016-06-24T09:26:30
#
#-------------------------------------------------

QT       += core \
                  gui \
                  serialport \
                  network


!MobileBuild {
QT += \
    printsupport \
    serialport \
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Drones
TEMPLATE = app

include(./qextserialport/src/qextserialport.pri)
INCLUDEPATH +=./qextserialport/src/qextserialport

include(./MAVLinkLib.pri)
INCLUDEPATH +=./libs/mavlink/include/mavlink/v1.0

SOURCES += main.cpp\
        mainwindow.cpp \
    helper.cpp \
    linkmanager.cpp \
    SerialCommunication.cpp \
    FirmwareUpgrade/Bootloader.cpp \
    FirmwareUpgrade/FirmwareImage.cpp \
    FirmwareUpgrade/PX4FirmUpgradeThread.cpp \
    SetupWizard/FirmwarePage.cpp \
    Vehicle.cpp \
    QuadApplication.cpp \
    Toolbox.cpp \
    VehicleManager.cpp \
    TitleLabel.cpp \
    SetupWidget.cpp \


HEADERS  += mainwindow.h \
    helper.h \
    linkmanager.h \
    SerialCommunication.h \
    FirmwareUpgrade/Bootloader.h \
    FirmwareUpgrade/FirmwareImage.h \
    FirmwareUpgrade/PX4FirmUpgradeThread.h \
    SetupWizard/FirmwarePage.h \
    Vehicle.h \
    QuadApplication.h \
    Toolbox.h \
    VehicleManager.h \
    TitleLabel.h \
    SetupWidget.h \

FORMS    += mainwindow.ui \
    SetupWidget.ui \

UI_DIR  += $$PWD

RESOURCES +=

DISTFILES += \

