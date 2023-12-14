#ifndef DHR_RGI_H
#define DHR_RGI_H

#include <QObject>
#include <QDebug>
#include <QModbusRtuSerialClient>
#include <QModbusDataUnit>
#include "widget/serialsettingwidget.h"

#define RTU_RESPONSE_TIME         500
#define RTU_CONNECT_RETRIES_TIME  3
#define RTU_NORMAL_SLAVE_ADDRESS  1

namespace dhr {
class DHR_RGI : public QObject
{
  Q_OBJECT
public:
  explicit DHR_RGI(QObject *parent = nullptr);
  ~DHR_RGI();

  void RGI_Connect(SerialSetting setting);
  void RGI_Disconnect();
  bool RGI_IsConnected();
  QString RGI_GetModbusError();
  void RGI_SetResponseTimeout(int timeout);
  void RGI_SetSlaveAddress(int address);

  void RGI_OpenGrip();
  void RGI_CloseGrip();

private:
  void ModbusChangedState(QModbusDevice::State state);
  void ModbusErrorHandle(QModbusDevice::Error error);
  void ModbusRTU_ReadHoldingRegister(int start_address, int amount);
  void ModbusRTU_OnReadyRead();
  void ModbusRTU_WriteHoldingRegister(int start_address,
                                      QList<quint16> write_list);
  void ModbusRTU_WriteHoldingRegister(QModbusDataUnit write_unit);
  void ModbusRTU_WriteResponseHandle(const QModbusDataUnit unit);

signals:
  void RGI_SignalConnected();
  void RGI_SignalDisconnected();
  void RGI_SignalConnecting();
  void RGI_SignalConnectInitFail(QString msg);
  void RGI_SignalConnectFail();
  void RGI_SignalError(QString msg);

private:
  QModbusRtuSerialClient *modbus_device_;
  bool is_connected_;
  int slave_address_;
  int response_timeout_;
  int retries_connect_time_;
};
}
#endif // DHR_RGI_H
