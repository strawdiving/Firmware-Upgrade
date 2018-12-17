#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSerialPort>
#include<QTimer>
#include <QElapsedTimer>
#include "SerialCommunication.h"
#include "common/mavlink.h"
#include "linkManager.h"
#include "helper.h"
#include "SetupWidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    static MainWindow*  _create();
    ~MainWindow();

    MainWindow* MainWindow::instance(void);

    void paintEvent(QPaintEvent* event);
    void closeEvent(QCloseEvent* event);
    void init();
    void enableAllDataTransmission(SerialLink *link);//pack message, and send message to FC to request param list

    int    getSystemId() {return systemId;}
private slots:

public slots:
    void _updateState();
    /// Signals from LinkManager
    void _linkStateChange(SerialLink* link);
    void on_pushButton_Connect_clicked();

protected:
    // TIMEOUT CONSTANTS
    static const unsigned int timeoutIntervalHeartbeat = 3.5; ///< Heartbeat timeout is 3.5 seconds

private:

    Ui::MainWindow *ui;
    QPalette w_palette; //warning
    QPalette c_palette; //common
    bool                   port_open;
    bool                   link_connected;
    bool                   connectionLost;

    SerialConfiguration* _config;
    LinkManager* _linkMgr;//manage serial links
    SerialLink* _currentLink;//currently connected link

    SetupWidget* _calWidget;

    QTimer             _portListTimer;//time to search for valid port
    QTimer             _statusTimer;

    int systemId;
};

#endif // MAINWINDOW_H
