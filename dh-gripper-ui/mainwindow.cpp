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

  connect(ui->btn_test_read, &QPushButton::clicked, this, [this] () {
    ModbusFunc send_func;
    send_func.code = FuncCode::kFuncWriteHoldingRegs;
    send_func.slave_address = 1;
    send_func.start_address = static_cast<int>(0x100);
    send_func.amount = 3;
    send_func.value.clear();
    send_func.value.push_back(123);
    send_func.value.push_back(278);
    send_func.value.push_back(369);
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
}

void MainWindow::on_button_click_serial_connect() {
  if(!m_dh_controller->DH_IsConnected()) {
    SerialSetting setting = ui->WidgetSerialSetting->GetSerialSetting();
    m_dh_controller->DH_Connect(setting);
  } else {
    m_dh_controller->DH_Disconnect();
  }
}
