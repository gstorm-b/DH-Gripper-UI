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

void MainWindow::closeEvent(QCloseEvent *event) {
  QMessageBox::StandardButton resBtn = QMessageBox::question( this, this->windowTitle(),
                                                             tr("Are you sure?\n"),
                                                             QMessageBox::Yes | QMessageBox::No,
                                                             QMessageBox::No);
  if (resBtn != QMessageBox::Yes) {
    event->ignore();
    return;
  }

  if (m_dh_controller->DH_IsConnected()) {
    m_dh_controller->DH_Disconnect();
  }
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
    if (ui->widgetRgiDevice->IsAutoInit()) {
      m_dh_controller->DH_AddFuncToQueue(DH_RGI::SetInitDevice(m_rgi_address));
    }
    if (ui->widgetPgcDevice->IsAutoInit()) {
      m_dh_controller->DH_AddFuncToQueue(DH_PGC::SetInitDevice(m_pgc_address));
    }
    ui->btn_connect->setEnabled(true);
  });

  connect(m_dh_controller, &DHController::DHSignal_Disconnected, this, [this] () {
    ui->statusbar->showMessage("Device disconnected.", 5000);
    ui->btn_connect->setText("Connect");
    ui->widgetRgiDevice->SetSlaveEditBoxEnable(true);
    ui->widgetPgcDevice->SetSlaveEditBoxEnable(true);
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
    ui->widgetPgcDevice->SetSlaveEditBoxEnable(true);
    ui->btn_connect->setEnabled(true);
  });

  connect(m_dh_controller, &DHController::DHSignal_ErrorOccured, this,
          [this] (QString msg) {
    ui->statusbar->showMessage(msg, 5000);
  });

  connect(m_dh_controller, &DHController::DHSignal_PollingDisplayTriggered,
          this, &MainWindow::DhDisplayDeviceInfo);

  connect(m_dh_controller, &DHController::DHSignal_PgcGripperStateChanged,
          this, [this] (DhGripperStatus state) {
    ui->statusbar->showMessage("PGC Gripper: " + dhr::EnumConvert(state) +
                               " - " + QString::number(test_counter));
    test_counter += 1;
  });

  connect(m_dh_controller, &DHController::DHSignal_RgiGripperStateChanged,
          this, [this] (DhGripperStatus state) {
    ui->statusbar->showMessage("RGI Gripper: " + dhr::EnumConvert(state) +
                               " - " + QString::number(test_counter));
    test_counter += 1;
  });

  connect(m_dh_controller, &DHController::DHSignal_RgiRotateStateChanged,
          this, [this] (DhRotationStatus state) {
    ui->statusbar->showMessage("RGI Rotation: " + dhr::EnumConvert(state) +
                               " - " + QString::number(test_counter));
    test_counter += 1;
  });

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

  connect(ui->widgetPgcDevice, &DhPgcWidget::SignalsRgiInitialize, this, [this] () {
    m_dh_controller->DH_AddFuncToQueue(DH_PGC::SetInitDevice(m_pgc_address));
  });

  connect(ui->widgetPgcDevice, &DhPgcWidget::SignalsGripper_PositionEdited,
          this, [this] (int value) {
    m_dh_controller->DH_AddFuncToQueue(
        DH_PGC::SetGripperPosition(m_pgc_address, value));
  });

  connect(ui->widgetPgcDevice, &DhPgcWidget::SignalsGripper_ForceEdited,
          this, [this] (int value) {
    m_dh_controller->DH_AddFuncToQueue(
        DH_PGC::SetGripperForce(m_pgc_address, value));
  });

  connect(ui->widgetPgcDevice, &DhPgcWidget::SignalsGripper_SpeedEdited,
          this, [this] (int value) {
    m_dh_controller->DH_AddFuncToQueue(
        DH_PGC::SetGripperSpeed(m_pgc_address, value));
  });
}

void MainWindow::DhDisplayDeviceInfo() {
  RGIData rgi_data;
  PGCData pgc_data;
  if (!m_dh_controller->DH_GetRgiData(m_rgi_address, rgi_data)) {
    this->statusBar()->showMessage("Get device info fail");
    return;
  }
  if (!m_dh_controller->DH_GetPgcData(m_pgc_address, pgc_data)) {
    this->statusBar()->showMessage("Get device info fail");
    return;
  }

  ui->widgetRgiDevice->ShowRgiDeviceInfo(rgi_data.feedback);
  ui->widgetPgcDevice->ShowPgcDeviceInfo(pgc_data.feedback);
}

void MainWindow::on_button_click_serial_connect() {
  if(!m_dh_controller->DH_IsConnected()) {
    SerialSettingDialog *serial_dialog = new SerialSettingDialog(this);
    connect(serial_dialog, &SerialSettingDialog::UserAcceptSerialSetting,
            this, [this] (SerialSetting setting) {

      m_rgi_address = ui->widgetRgiDevice->GetSlaveAddress();
      ui->widgetRgiDevice->SetSlaveEditBoxEnable(false);
      m_pgc_address = ui->widgetPgcDevice->GetSlaveAddress();
      ui->widgetPgcDevice->SetSlaveEditBoxEnable(false);
      ui->btn_connect->setEnabled(false);
      m_dh_controller->DH_SetRgiAddress(m_rgi_address);
      m_dh_controller->DH_SetPgcAddress(m_pgc_address);
      m_dh_controller->DH_Connect(setting);
    });
    serial_dialog->ShowDialog();
  } else {
    m_dh_controller->DH_Disconnect();
  }
}
