#include "dhrgiwidget.h"
#include "ui_dhrgiwidget.h"

DhRgiWidget::DhRgiWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DhRgiWidget)
{
  ui->setupUi(this);
  InitSignals();
}

DhRgiWidget::~DhRgiWidget()
{
  delete ui;
}

void DhRgiWidget::ShowRgiDeviceSetParameters(RGIData::RGIFeedback feedback_data) {
  ui->doubleSpinBox_gipper_position->setValue(feedback_data.gripper_position/10.0);
  ui->spinBox_gripper_force->setValue(100);
  ui->spinBox_gripper_speed->setValue(100);
  ui->spinBox_rotation_angle->setValue(feedback_data.rotation_angle);
  ui->spinBox_rotation_torque->setValue(100);
  ui->spinBox_rotation_speed->setValue(100);
}

void DhRgiWidget::ShowRgiDeviceInfo(RGIData::RGIFeedback feedback_data) {
  // gripper info
  ui->label_gripper_init_state->setText(
      "Initialize state: " + dhr::EnumConvert(feedback_data.gripper_init_state));
  ui->label_gripper_status->setText(
      "Status: " + dhr::EnumConvert(feedback_data.gripper_status));
  ui->label_gripper_position->setText(
      "Current position: " + QString::number(feedback_data.gripper_position / 10.0));

  // rotating info
  ui->label_rotation_init_state->setText(
      "Initialize state: " + dhr::EnumConvert(feedback_data.rotation_init_state));
  ui->label_rotation_status->setText(
      "Status: " + dhr::EnumConvert(feedback_data.rotation_status));
  ui->label_rotation_angle->setText(
      "Current angle: " + QString::number(feedback_data.rotation_angle));
}

void DhRgiWidget::InitSignals() {
  connect(ui->doubleSpinBox_gipper_position, &QDoubleSpinBox::editingFinished,
          this, [this] () {
    int value = ui->doubleSpinBox_gipper_position->value() * 10;
    emit SignalsGripper_PositionEdited(value);
  });

  connect(ui->spinBox_gripper_force, &QSpinBox::editingFinished,
          this, [this] () {
    emit SignalsGripper_ForceEdited(ui->spinBox_gripper_force->value());
  });

  connect(ui->spinBox_gripper_speed, &QSpinBox::editingFinished,
          this, [this] () {
    emit SignalsGripper_SpeedEdited(ui->spinBox_gripper_speed->value());
  });

  connect(ui->spinBox_rotation_angle, &QSpinBox::editingFinished,
          this, [this] () {
    emit SignalsRotation_AngleEdited(ui->spinBox_rotation_angle->value());
  });

  connect(ui->spinBox_rotation_torque, &QSpinBox::editingFinished,
          this, [this] () {
    emit SignalsRotation_TorqueEdited(ui->spinBox_rotation_torque->value());
  });

  connect(ui->spinBox_rotation_speed, &QSpinBox::editingFinished,
          this, [this] () {
    emit SignalsRotation_SpeedEdited(ui->spinBox_rotation_speed->value());
  });
}

