#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  InitAction();
  DhInitialize();

//  DhInitState init_state =
//      static_cast<DhInitState>(static_cast<quint16>(1));
//  DhGripperStatus gripper_status =
//      static_cast<DhGripperStatus>(static_cast<quint16>(1));
//  DhRotationStatus rotation_status =
//      static_cast<DhRotationStatus>(static_cast<quint16>(1));
//  qDebug() << dhr::EnumConvert(init_state);
//  qDebug() << dhr::EnumConvert(gripper_status);
//  qDebug() << dhr::EnumConvert(rotation_status);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::InitAction() {
  connect(ui->btn_connect, &QPushButton::clicked,
          this, &MainWindow::on_button_click_serial_connect);

  connect(ui->btn_test_read, &QPushButton::clicked, this, [this] () {
    ModbusFunc send_func;
    send_func.func_code = FuncCode::kFuncReadHoldingRegs;
    send_func.slave_address = 1;
    send_func.start_address = static_cast<int>(0x200);
    send_func.amount = static_cast<int>(0x18);
    m_dh_controller->DH_AddFuncToQueue(send_func);
  });
}

void MainWindow::DhInitialize() {
  m_dh_controller = new DHController(QThread::NormalPriority, this);

  connect(m_dh_controller, &DHController::DHSignal_Connected, this, [this] () {
    ui->statusbar->showMessage("Device connected.", 5000);
    ui->btn_connect->setText("Disconnect");
  });

  connect(m_dh_controller, &DHController::DHSignal_Disconnected, this, [this] () {
    ui->statusbar->showMessage("Device disconnected.", 5000);
    ui->btn_connect->setText("Connect");
  });

  connect(m_dh_controller, &DHController::DHSignal_Connecting, this, [this] () {
    ui->statusbar->showMessage("Device connecting...");
    ui->btn_connect->setText("Connecting");
  });

  connect(m_dh_controller, &DHController::DHSignal_ConnectFail, this,
          [this] (QString msg) {
    ui->statusbar->showMessage(msg, 5000);
    ui->btn_connect->setText("Connect");
  });

  connect(m_dh_controller, &DHController::DHSignal_ErrorOccured, this,
          [this] (QString msg) {
    ui->statusbar->showMessage(msg, 5000);
  });

  connect(m_dh_controller, &DHController::DHSignal_PollingTriggered,
          this, &MainWindow::DhDisplayRgiInfo);

  connect(ui->widgetRgiDevice, &DhRgiWidget::SignalsGripper_PositionEdited,
          this, [this] (int value) {
    m_dh_controller->RGI_SetGripperPosition(value);
  });

  connect(ui->widgetRgiDevice, &DhRgiWidget::SignalsGripper_ForceEdited,
          this, [this] (int value) {
    m_dh_controller->RGI_SetGripperForce(value);
  });

  connect(ui->widgetRgiDevice, &DhRgiWidget::SignalsGripper_SpeedEdited,
          this, [this] (int value) {
    m_dh_controller->RGI_SetGripperSpeed(value);
  });

  connect(ui->widgetRgiDevice, &DhRgiWidget::SignalsRotation_AngleEdited,
          this, [this] (int value) {
    m_dh_controller->RGI_SetRotationAngle(value);
  });

  connect(ui->widgetRgiDevice, &DhRgiWidget::SignalsRotation_TorqueEdited,
          this, [this] (int value) {
    m_dh_controller->RGI_SetRotationTorque(value);
  });

  connect(ui->widgetRgiDevice, &DhRgiWidget::SignalsRotation_SpeedEdited,
          this, [this] (int value) {
    m_dh_controller->RGI_SetRotationSpeed(value);
  });
}

void MainWindow::DhDisplayRgiInfo(RGIData device_info) {
  ui->widgetRgiDevice->ShowRgiDeviceInfo(device_info.feedback);
}

void MainWindow::on_button_click_serial_connect() {
  if(!m_dh_controller->DH_IsConnected()) {
    SerialSetting setting = ui->WidgetSerialSetting->GetSerialSetting();
    m_dh_controller->DH_Connect(setting);
  } else {
    m_dh_controller->DH_Disconnect();
  }
}
