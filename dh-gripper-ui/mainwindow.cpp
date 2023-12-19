#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  InitAction();
  DhInitialize();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::InitAction() {
  connect(ui->btn_connect, &QPushButton::clicked,
          this, &MainWindow::on_button_click_serial_connect);
}

void MainWindow::DhInitialize() {
  m_dh_controller = new DHController(QThread::NormalPriority, this);

  connect(m_dh_controller, &DHController::DHSignal_Connected, this, [this] () {
    ui->statusbar->showMessage("Device connected.", 5000);
    ui->btn_connect->setText("Disconnect");
    if(ui->widgetRgiDevice->IsAutoInit()) {
      m_dh_controller->DH_AddFuncToQueue(DH_RGI::SetInitDevice(m_rgi_address));
    }
    ui->btn_connect->setEnabled(true);
  });

  connect(m_dh_controller, &DHController::DHSignal_Disconnected, this, [this] () {
    ui->statusbar->showMessage("Device disconnected.", 5000);
    ui->btn_connect->setText("Connect");
    ui->widgetRgiDevice->SetSlaveEditBoxEnable(true);
    ui->btn_connect->setEnabled(true);
  });

  connect(m_dh_controller, &DHController::DHSignal_Connecting, this, [this] () {
    ui->statusbar->showMessage("Device connecting...");
    ui->btn_connect->setText("Connecting");
  });

  connect(m_dh_controller, &DHController::DHSignal_ConnectFail, this,
          [this] (QString msg) {
    ui->statusbar->showMessage(msg, 5000);
    ui->btn_connect->setText("Connect");
    ui->widgetRgiDevice->SetSlaveEditBoxEnable(true);
    ui->btn_connect->setEnabled(true);
  });

  connect(m_dh_controller, &DHController::DHSignal_ErrorOccured, this,
          [this] (QString msg) {
    ui->statusbar->showMessage(msg, 5000);
  });

  connect(m_dh_controller, &DHController::DHSignal_PollingTriggered,
          this, &MainWindow::DhDisplayRgiInfo);

  connect(ui->widgetRgiDevice, &DhRgiWidget::SignalsRgiInitialize, this, [this] () {
    m_dh_controller->DH_AddFuncToQueue(DH_RGI::SetInitDevice(m_rgi_address));
  });

  connect(ui->widgetRgiDevice, &DhRgiWidget::SignalsGripper_PositionEdited,
          this, [this] (int value) {
    m_dh_controller->DH_AddFuncToQueue(
        DH_RGI::SetGripperPosition(m_rgi_address, value));
  });

  connect(ui->widgetRgiDevice, &DhRgiWidget::SignalsGripper_ForceEdited,
          this, [this] (int value) {
    m_dh_controller->DH_AddFuncToQueue(
        DH_RGI::SetGripperForce(m_rgi_address, value));
  });

  connect(ui->widgetRgiDevice, &DhRgiWidget::SignalsGripper_SpeedEdited,
          this, [this] (int value) {
    m_dh_controller->DH_AddFuncToQueue(
        DH_RGI::SetGripperSpeed(m_rgi_address, value));
  });

  connect(ui->widgetRgiDevice, &DhRgiWidget::SignalsRotation_AngleEdited,
          this, [this] (int value) {
    m_dh_controller->DH_AddFuncToQueue(
        DH_RGI::SetRotationAngle(m_rgi_address, value));
  });

  connect(ui->widgetRgiDevice, &DhRgiWidget::SignalsRotation_TorqueEdited,
          this, [this] (int value) {
    m_dh_controller->DH_AddFuncToQueue(
        DH_RGI::SetRotationTorque(m_rgi_address, value));
  });

  connect(ui->widgetRgiDevice, &DhRgiWidget::SignalsRotation_SpeedEdited,
          this, [this] (int value) {
    m_dh_controller->DH_AddFuncToQueue(
        DH_RGI::SetRotationSpeed(m_rgi_address, value));
  });
}

void MainWindow::DhDisplayRgiInfo(RGIData device_info) {
  ui->widgetRgiDevice->ShowRgiDeviceInfo(device_info.feedback);
}

void MainWindow::on_button_click_serial_connect() {
  if(!m_dh_controller->DH_IsConnected()) {
    SerialSetting setting = ui->WidgetSerialSetting->GetSerialSetting();
    m_rgi_address = ui->widgetRgiDevice->GetSlaveAddress();
    ui->widgetRgiDevice->SetSlaveEditBoxEnable(false);
    ui->btn_connect->setEnabled(false);
    m_dh_controller->DH_Connect(setting);
  } else {
    m_dh_controller->DH_Disconnect();
  }
}
