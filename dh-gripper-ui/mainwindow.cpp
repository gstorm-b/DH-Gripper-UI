#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  InitAction();
  RgiInitialize();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::InitAction() {
  connect(ui->btn_connect, &QPushButton::clicked,
          this, &MainWindow::on_button_click_serial_connect);

  gripstatic = true;
  connect(ui->btn_test_read, &QPushButton::clicked,
          this, [this] () {
    if (gripstatic) {
      m_rgi_device->RGI_CloseGrip();
    } else {
      m_rgi_device->RGI_OpenGrip();
    }
    gripstatic = !gripstatic;
  });
}

void MainWindow::RgiInitialize() {
  m_rgi_device = new DHR_RGI(this);
  m_rgi_device->RGI_SetSlaveAddress(1);

  connect(m_rgi_device, &DHR_RGI::RGI_SignalConnected, this, [this] () {
    ui->statusbar->showMessage("Device connected.", 5000);
    ui->btn_connect->setText("Disconnect");
  });

  connect(m_rgi_device, &DHR_RGI::RGI_SignalDisconnected, this, [this] () {
    ui->statusbar->showMessage("Device disconnected.", 5000);
    ui->btn_connect->setText("Connect");
  });

  connect(m_rgi_device, &DHR_RGI::RGI_SignalConnecting, this, [this] () {
    ui->statusbar->showMessage("Device connecting...");
    ui->btn_connect->setText("Connecting");
  });

  connect(m_rgi_device, &DHR_RGI::RGI_SignalConnectInitFail,
          this, [this] (QString msg) {
    ui->statusbar->showMessage(msg, 5000);
    ui->btn_connect->setText("Connect");
  });

  connect(m_rgi_device, &DHR_RGI::RGI_SignalConnectFail, this, [this] () {
    ui->statusbar->showMessage("Device connect fail.", 5000);
    ui->btn_connect->setText("Connect");
  });

  connect(m_rgi_device, &DHR_RGI::RGI_SignalError,
          this, [this] (QString msg) {
    ui->statusbar->showMessage(msg, 5000);
  });
}

void MainWindow::on_button_click_serial_connect() {
  if(!m_rgi_device->RGI_IsConnected()) {
    SerialSetting setting = ui->WidgetSerialSetting->GetSerialSetting();
    m_rgi_device->RGI_Connect(setting);
  } else {
    m_rgi_device->RGI_Disconnect();
  }
}
