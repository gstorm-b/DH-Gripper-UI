#include "dhr_rgi.h"
namespace dhr {
DHR_RGI::DHR_RGI(QObject *parent)
    : QObject{parent}
{
  modbus_device_ = new QModbusRtuSerialClient(this);
  is_connected_ = false;
  response_timeout_ = RTU_RESPONSE_TIME;
  retries_connect_time_ = RTU_CONNECT_RETRIES_TIME;
  slave_address_ = RTU_NORMAL_SLAVE_ADDRESS;

  connect(modbus_device_, &QModbusRtuSerialClient::stateChanged,
          this, &DHR_RGI::ModbusChangedState);
  connect(modbus_device_, &QModbusRtuSerialClient::errorOccurred,
          this, &DHR_RGI::ModbusErrorHandle);
}

DHR_RGI::~DHR_RGI() {}

void DHR_RGI::RGI_Connect(SerialSetting setting) {
  if (!modbus_device_) {
    return;
  }
  // set serial port parameters
  modbus_device_->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
                                       setting.name);
  modbus_device_->setConnectionParameter(QModbusDevice::SerialParityParameter,
                                       setting.parity);
  modbus_device_->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
                                       setting.baudrate);
  modbus_device_->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
                                       setting.data_bits);
  modbus_device_->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
                                       setting.stop_bits);
  modbus_device_->setTimeout(response_timeout_);
  modbus_device_->setNumberOfRetries(retries_connect_time_);

  if(!modbus_device_->connectDevice()) {
    emit RGI_SignalConnectInitFail(tr("Connect failed: %1").
                                   arg(modbus_device_->errorString()));
  }
}

void DHR_RGI::RGI_Disconnect() {
  modbus_device_->disconnectDevice();
}

bool DHR_RGI::RGI_IsConnected() {
  return is_connected_;
}

QString DHR_RGI::RGI_GetModbusError() {
  return modbus_device_->errorString();
}

void DHR_RGI::RGI_SetSlaveAddress(int address) {
  slave_address_ = address;
}

void DHR_RGI::RGI_OpenGrip() {
  QList<quint16> write_list;
  write_list.push_back(static_cast<quint16>(0));
  ModbusRTU_WriteHoldingRegister(static_cast<int>(0x103), write_list);
}

void DHR_RGI::RGI_CloseGrip() {
  QList<quint16> write_list;
  write_list.push_back(static_cast<quint16>(1000));
  ModbusRTU_WriteHoldingRegister(static_cast<int>(0x103), write_list);
}

//////// PRIVATE FUNCTIONS
void DHR_RGI::ModbusChangedState(QModbusDevice::State state) {
  switch (state) {
    case QModbusDevice::State::ConnectedState:
      is_connected_ = true;
      emit RGI_SignalConnected();
//      readFeederInitValue();
      break;
    case QModbusDevice::State::UnconnectedState:
      if(!is_connected_) {
        emit RGI_SignalConnectFail();
        return;
      }
      is_connected_ = false;
      emit RGI_SignalDisconnected();
      break;
    case QModbusDevice::State::ConnectingState:
      emit RGI_SignalConnecting();
      break;
  }
}

void DHR_RGI::ModbusErrorHandle(QModbusDevice::Error error) {
  emit RGI_SignalError(modbus_device_->errorString());
}

void DHR_RGI::ModbusRTU_ReadHoldingRegister(int start_address, int amount) {
  if(!is_connected_) {
    return;
  }

  QModbusDataUnit read_request(QModbusDataUnit::HoldingRegisters,
                               start_address,
                               static_cast<quint16>(amount));
  if (auto *reply = modbus_device_->sendReadRequest(read_request, slave_address_)) {
    if (!reply->isFinished()) {
      connect(reply, &QModbusReply::finished, this, &DHR_RGI::ModbusRTU_OnReadyRead);
    } else {
      delete reply; // broadcast replies return immediately
    }
  } else {
    emit RGI_SignalError(tr("Read error: %1").arg(modbus_device_->errorString()));
  }
}

void DHR_RGI::ModbusRTU_OnReadyRead() {
  auto reply = qobject_cast<QModbusReply *>(sender());
  if (!reply)
    return;

  if (reply->error() == QModbusDevice::NoError) {
    const QModbusDataUnit unit = reply->result();
    for (qsizetype index=0;index<unit.valueCount();++index) {
      const QString entry = tr("Address: %1, Value: %2").
                            arg(unit.startAddress() + index).
                            arg(QString::number(unit.value(index), 16));
      qDebug() << entry;
    }
  } else if (reply->error() == QModbusDevice::ProtocolError) {
    emit RGI_SignalError(tr("Read response error: %1 (Modbus exception: 0x%2)").
                         arg(reply->errorString()).
                         arg(reply->rawResult().exceptionCode(), -1, 16));
  } else {
    emit RGI_SignalError(tr("Read response error: %1 (code: 0x%2)").
                         arg(reply->errorString()).
                         arg(reply->error(), -1, 16));
  }

  reply->deleteLater();
}

void DHR_RGI::ModbusRTU_WriteHoldingRegister(int start_address,
                                             QList<quint16> write_list) {
  QModbusDataUnit write_unit(QModbusDataUnit::HoldingRegisters,
                            start_address,
                            static_cast<quint16>(write_list.size()));
  for(qsizetype index=0;index<write_list.size();index++) {
    write_unit.setValue(index, write_list.at(index));
  }
  ModbusRTU_WriteHoldingRegister(write_unit);
}

void DHR_RGI::ModbusRTU_WriteHoldingRegister(QModbusDataUnit write_unit) {
  if(!is_connected_) {
    return;
  }

  QModbusReply *reply = modbus_device_->sendWriteRequest(write_unit, slave_address_);
  if(reply) {
    if(!reply->isFinished()) {
      connect(reply, &QModbusReply::finished, this, [this, reply]() {
        const auto error = reply->error();
        if (error == QModbusDevice::ProtocolError) {
          emit RGI_SignalError(tr("Write error: %1 (Modbus exception: 0x%2)").
                               arg(reply->errorString()).
                               arg(reply->rawResult().exceptionCode(), -1, 16));
        } else if (error != QModbusDevice::NoError) {
          emit RGI_SignalError(tr("Write error: %1 (code: 0x%2)").
                               arg(reply->errorString()).
                               arg(error, -1, 16));
        } else if (error == QModbusDevice::NoError) {
          ModbusRTU_WriteResponseHandle(reply->result());
        }
        reply->deleteLater();
      });
    }
    else {
      reply->deleteLater();
    }
  }
  else {
    emit RGI_SignalError(tr("Write error: %1").arg(modbus_device_->errorString()));
  }
}

void DHR_RGI::ModbusRTU_WriteResponseHandle(const QModbusDataUnit unit) {
  for (qsizetype index=0;index<unit.valueCount();++index) {
    const QString entry = tr("Address: %1, Value: %2").
                          arg(unit.startAddress() + index).
                          arg(QString::number(unit.value(index), 16));
    qDebug() << entry;
  }
}
}
