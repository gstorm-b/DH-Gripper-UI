#include "dh_rgi.h"

namespace dhr {
DH_RGI::DH_RGI() {
  InitDataMap();
}

DH_RGI::DH_RGI(int slave_address) {
  slave_address_ = slave_address;
  InitDataMap();
}

void DH_RGI::SetSlaveAddress(int address) {
  if(IsValueInRange(247,1,address)) {
    slave_address_ = address;
  }
}

void DH_RGI::UpdateData(const QModbusDataUnit unit) {
  for (qsizetype index=0;index<unit.valueCount();++index) {
    quint16 address = static_cast<quint16>(unit.startAddress()) +
                      static_cast<quint16>(index);
    if (data_map_.contains(address)) {
      data_map_[address] = unit.value(index);
//      continue;
    }
//  qDebug() << "Start address:" << QString::number(unit.startAddress(), 16)
//           << "Index:" << QString::number(index, 10)
//           << "Address" << QString::number(address, 16)
//           << ":" << unit.value(index);
  }
}

RGIData DH_RGI::DeviceInfo() {
  RGIData rgi_info;
  ConvertFeedbackData(rgi_info);
  return rgi_info;
}

void DH_RGI::ConvertFeedbackData(RGIData &rgi_info) {
  rgi_info.feedback.gripper_init_state =
      static_cast<DhInitState>(data_map_[feedback_grip_init_state]);
  rgi_info.feedback.gripper_status =
      static_cast<DhGripperStatus>(data_map_[feedback_grip_status]);
  rgi_info.feedback.gripper_position = data_map_[feedback_grip_position];
  rgi_info.feedback.gripper_speed = data_map_[feedback_grip_speed];
  rgi_info.feedback.gripper_current = data_map_[feedback_grip_current];
  rgi_info.feedback.rotation_init_state =
      static_cast<DhInitState>(data_map_[feedback_rotation_init_state]);
  rgi_info.feedback.rotation_status =
      static_cast<DhRotationStatus>(data_map_[feedback_rotation_status]);
  rgi_info.feedback.rotation_angle =
      static_cast<qint16>(data_map_[feedback_rotation_angle]);
  rgi_info.feedback.rotation_speed = data_map_[feedback_rotation_speed];
  rgi_info.feedback.rotation_current = data_map_[feedback_rotation_current];
  rgi_info.feedback.error_code =
      static_cast<DhError>(data_map_[feedback_error_code]);
}

ModbusFunc DH_RGI::GetDeviceFeedbackInfo() {
  ModbusFunc func(slave_address_, FuncCode::kFuncReadHoldingRegs,
                  feedback_grip_init_state, static_cast<quint16>(0x18));
  return func;
}

ModbusFunc DH_RGI::SetInitDevice() {
  ModbusFunc func(slave_address_, FuncCode::kFuncWriteHoldingRegs,
                  control_init, 1);
  func.value.push_back(165);
  return func;
}

ModbusFunc DH_RGI::SetGripperPosition(int position) {
  ModbusFunc func(slave_address_, FuncCode::kFuncWriteHoldingRegs,
                  control_grip_position, 1);
  func.value.push_back(position);
  if(!IsValueInRange(GRIPPER_POSITION_MAX, GRIPPER_POSITION_MIN, position)) {
    func.slave_address = 255;
  }
  return func;
}

ModbusFunc DH_RGI::SetGripperForce(int force) {
  ModbusFunc func(slave_address_, FuncCode::kFuncWriteHoldingRegs,
                  control_grip_force, 1);
  func.value.push_back(force);
  if(!IsValueInRange(GRIPPER_FORCE_MAX, GRIPPER_FORCE_MIN, force)) {
    func.slave_address = 255;
  }
  return func;
}

ModbusFunc DH_RGI::SetGripperSpeed(int speed) {
  ModbusFunc func(slave_address_, FuncCode::kFuncWriteHoldingRegs,
                  control_grip_speed, 1);
  func.value.push_back(speed);
  if(!IsValueInRange(GRIPPER_SPEED_MAX, GRIPPER_SPEED_MIN, speed)) {
    func.slave_address = 255;
  }
  return func;
}

ModbusFunc DH_RGI::SetRotationAngle(int angle) {
  ModbusFunc func(slave_address_, FuncCode::kFuncWriteHoldingRegs,
                  control_rotation_angle, 1);
  func.value.push_back(angle);
  if(!IsValueInRange(ROTATION_ANGLE_MAX, ROTATION_ANGLE_MIN, angle)) {
    func.slave_address = 255;
  }
  return func;
}

ModbusFunc DH_RGI::SetRotationTorque(int torque) {
  ModbusFunc func(slave_address_, FuncCode::kFuncWriteHoldingRegs,
                  control_rotation_torque, 1);
  func.value.push_back(torque);
  if(!IsValueInRange(ROTATION_TORQUE_MAX, ROTATION_TORQUE_MIN, torque)) {
    func.slave_address = 255;
  }
  return func;
}

ModbusFunc DH_RGI::SetRotationSpeed(int speed) {
  ModbusFunc func(slave_address_, FuncCode::kFuncWriteHoldingRegs,
                  control_rotation_speed, 1);
  func.value.push_back(speed);
  if(!IsValueInRange(ROTATION_SPEED_MAX, ROTATION_SPEED_MIN, speed)) {
    func.slave_address = 255;
  }
  return func;
}

void DH_RGI::InitDataMap() {
  ///// MAPPING FEEDBACK DATA
  // control map
  data_map_[control_init] = 0x00;
  data_map_[control_grip_force] = 0x00;
  data_map_[control_grip_position] = 0x00;
  data_map_[control_grip_speed] = 0x00;
  data_map_[control_rotation_angle] = 0x00;
  data_map_[control_rotation_speed] = 0x00;
  data_map_[control_rotation_torque] = 0x00;
  // feedback map
  data_map_[feedback_grip_init_state] = 0x00;
  data_map_[feedback_grip_status] = 0x00;
  data_map_[feedback_grip_position] = 0x00;
  data_map_[feedback_grip_speed] = 0x00;
  data_map_[feedback_grip_current] = 0x00;
  data_map_[feedback_error_code] = 0x00;
  data_map_[feedback_rotation_angle] = 0x00;
  data_map_[feedback_rotation_init_state] = 0x00;
  data_map_[feedback_rotation_status] = 0x00;
  data_map_[feedback_rotation_speed] = 0x00;
  data_map_[feedback_rotation_current] = 0x00;
}

void DH_RGI::PushSingleValue(QList<quint16> &container, quint16 value) {
  container.clear();
  container.push_back(value);
}
}
