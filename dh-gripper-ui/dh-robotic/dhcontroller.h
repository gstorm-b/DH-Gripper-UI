#ifndef DHCONTROLLER_H
#define DHCONTROLLER_H

#include <QThread>
#include <QObject>
#include <QDebug>
#include <QModbusRtuSerialClient>
#include <QMutex>

#include "timer/TimeCounter.h"
#include "widget/serialsettingwidget.h"

//#define RTU_RESPONSE_TIME         500
#define RTU_NORMAL_REFRESH_TIME   100
#define RTU_CONNECT_RETRIES_TIME  3
#define RTU_NORMAL_SLAVE_ADDRESS  1
#define MUTEX_LOCK_TIMEOUT        50

namespace dhr {
enum FuncCode : int {
  kFuncReadHoldingRegs = 3,
  kFuncWriteHoldingRegs = 10
};

enum ModbusUnitType : int {
  kUnitCoils = 0,
  kUnitInputCoils,
  kUnitHoldingRegisters,
  kUnitInputRegisters
};

struct ModbusFunc {
  int slave_address = 1;
  FuncCode code = FuncCode::kFuncReadHoldingRegs;
  int start_address = 0;
  int amount = 1;
  QList<quint16> value;
};

struct ModbusRegister {
  int address = 0;
  quint16 value = 0;
};

struct ModbusFuncResponse {
  int slave_address = 1;
  ModbusUnitType type = ModbusUnitType::kUnitHoldingRegisters;
  QList<ModbusRegister> value;
};

class DHController : public QThread
{
  Q_OBJECT
public:
  explicit DHController(QThread::Priority priority = QThread::NormalPriority,
                        QObject *parent = nullptr);
  ~DHController();

  void DH_Connect(SerialSetting setting);
  void DH_Disconnect();
  bool DH_IsConnected();

  void DH_AddFuncToQueue(ModbusFunc func);

private:
  void run() override;
  void ModbusInit();
  void ModbusChangedState(QModbusDevice::State state);
  void ModbusErrorHandle(QModbusDevice::Error error);
  void ModbusReadHodlingRegister(int slave_address, int start_address, int amount);
  void ModbusWriteHoldingRegister(int slave_address, int start_address,
                                  QList<quint16> write_list);
  void ModbusHodlingRegsResponse(const QModbusDataUnit unit);

  void ModbusQueueClear();
  bool ModbusQueueIsEmpty();
  ModbusFunc ModbusQueueGetFront();
  void ModbusQueuePopFront();

  void ModbusQueueHandle();

signals:
  void DHSignal_Connected();
  void DHSignal_Disconnected();
  void DHSignal_ConnectFail(QString msg);
  void DHSignal_Connecting();
  void DHSignal_ErrorOccured(QString msg);
  void DHSignal_PollingTriggered();

private:
  QThread::Priority thread_priority_;
  bool thread_running_;
  QMutex mutex_;
  QModbusRtuSerialClient *modbus_device_;
  SerialSetting setting_connect_;
  TimeCounter *time_counter_;
  bool is_modbus_connected_;
  int refresh_data_time_;
  QList<ModbusFunc> send_func_queue_;
};
}

#endif // DHCONTROLLER_H
