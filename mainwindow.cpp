#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPortInfo>
#include <QtDebug>
#include <QtCore>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include "linkmanager.h"

#include "QuadApplication.h"
#include "VehicleManager.h"
#include "Vehicle.h"

static MainWindow* _instance = NULL;

MainWindow* MainWindow::_create()
{
    Q_ASSERT(_instance == NULL);
    _instance = new MainWindow();
    Q_ASSERT(_instance);
    return _instance;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
  , port_open(false)
  , connectionLost(true)
  , systemId(TargetSystemID)

{
    ui->setupUi(this);
    setFixedSize(1200,700);
    this->setStyleSheet("QPushButton {background: #cfe2f3}"
                        "QPushButton:pressed {background: 6699cc}");

    ui->pushButton_Connect->setFixedWidth(100);

    c_palette.setColor(QPalette::WindowText,Qt::black);
    w_palette.setColor(QPalette::WindowText,Qt::red);

    _calWidget = new SetupWidget(this);
    ui->stackedWidget_Summary->addWidget(_calWidget);
    ui->stackedWidget_Summary->setCurrentWidget(_calWidget);

    connect(&_portListTimer, &QTimer::timeout, this, &MainWindow::init);
    _portListTimer.start(1000);

    _linkMgr = qgcApp()->toolbox()->linkManager();
    connect(_linkMgr, &LinkManager::linkConnected,    this, &MainWindow::_linkStateChange);
    connect(_linkMgr, &LinkManager::linkDisconnected, this, &MainWindow::_linkStateChange);

    show();
}

 MainWindow* MainWindow::instance(void)
 {
     return _instance;
 }

//display config description and baudrate
void MainWindow::init()
{
    _linkMgr->_updateConfigurationList();
    QList<SerialConfiguration*> configs = _linkMgr->getSerialConfigurationList();
    foreach (SerialConfiguration* config, configs)
    {
     if(ui->label_Portname->text()!=config->description())
     {
     ui->label_Portname->setText(config->description());
     ui->label_Baudrate->setText(QString::number(config->baud()));
     qDebug()<<config->description()<<config->baud();
      }
    _portListTimer.stop();
    _config = config;
    ui->pushButton_Connect->setEnabled(true);
     }
}

//create serial link, start thread, open serial port
void MainWindow::on_pushButton_Connect_clicked()
{
     Q_ASSERT(_currentLink);
     if(port_open == false)
     {
     if(_config) {
        _linkMgr->createConnectedLink(_config);
        if(_currentLink=_linkMgr->getLink()) {
          port_open = true;
          ui->pushButton_Connect->setText("断开连接");
          enableAllDataTransmission(_currentLink);
          connect(&_statusTimer,&QTimer::timeout,this,&MainWindow::_updateState);
          _statusTimer.start(500);
         }
        else {
            qDebug()<<"can‘t create link!";
         }
       }
     }
     else{
         if (_currentLink->isConnected()) {
            _linkMgr->disconnectLink(_currentLink);
            port_open = false;
            ui->pushButton_Connect->setText("连接");
         }
      }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event)
    /*QMessageBox::StandardButton button;
    button = QMessageBox::question(this,tr("关闭程序"),
                           tr("确认退出程序？"),
                                   QMessageBox::Yes | QMessageBox::No);
     if(button == QMessageBox::Yes){
         event->accept();
     }
     if(button == QMessageBox::No){
         event->ignore();
     }
     */
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
}

void MainWindow::_updateState()
{
    quint64 curTime =static_cast<quint64>(QDateTime::currentMSecsSinceEpoch());

    quint64 lastHeartbeat = _linkMgr->getHeartbeatTime();
    quint64 heartbeatInterval = (curTime - lastHeartbeat)/1000.0;
    //qDebug()<<"last time"<<lastHeartbeat<<"now time: "<<curTime<<"interval"<<heartbeatInterval;

    // Check if heartbeat timed out
    if(!connectionLost && (heartbeatInterval>timeoutIntervalHeartbeat))
    {
        connectionLost = true;
        ui->label_Message->setPalette(w_palette);
        ui->label_Message->setText("链接丢失!");

        if (_currentLink->isConnected()) {
           _linkMgr->disconnectLink(_currentLink);
           port_open = false;
           ui->pushButton_Connect->setText("连接");
         }
    }

    // Update connection loss time on each iteration
        if (connectionLost && (heartbeatInterval > timeoutIntervalHeartbeat))
        {
            quint64 connectionLossTime = heartbeatInterval;
            ui->label_Message->setPalette(w_palette);
            ui->label_Message->setText("链接丢失: "+QString::number(connectionLossTime));
        }
    // Connection gained
    if(connectionLost && (heartbeatInterval<timeoutIntervalHeartbeat))
    {
        connectionLost = false;
        ui->label_Message->setPalette(c_palette);
        ui->label_Message->setText("连接成功!");
    }
}

MainWindow::~MainWindow()
{
    _instance = NULL;
    delete ui;
    if (_currentLink) {
        _linkMgr->disconnectLink(_currentLink);
      }
}

void MainWindow::enableAllDataTransmission(SerialLink* link)
{
    Q_UNUSED(link);
    mavlink_message_t msg;
    mavlink_param_request_list_t param;
    param.target_component = DEFAULT_COMPONENT_ID;
    param.target_system = TargetSystemID;

    mavlink_msg_param_request_list_encode(param.target_system,param.target_component,&msg,&param);
    _linkMgr->sendMessage(msg);
}

void MainWindow::_linkStateChange(SerialLink* link)
{
    int connectCount=0;
    qDebug()<<"_linkStateChange called";
    if(link->isConnected())    {
        connectCount=1;
    }
    else{
        connectCount=0;
    }
 }
