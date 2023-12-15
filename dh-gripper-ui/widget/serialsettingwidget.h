#ifndef SERIALSETTINGWIDGET_H
#define SERIALSETTINGWIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>

static const char BlankString[] = QT_TRANSLATE_NOOP("SerialSettingWidget", "N/A");

struct SerialSetting
{
  QString name;
  qint32 baudrate;
  QString string_baudrate;
  QSerialPort::DataBits data_bits;
  QString string_data_bits;
  QSerialPort::Parity parity;
  QString string_parity;
  QSerialPort::StopBits stop_bits;
  QString string_stop_bits;
  QSerialPort::FlowControl flow_control;
  QString string_flow_control;
  int response_timeout;
  QString string_response_timeout;
  int polls_time;
  QString string_polls_time;
  //  bool local_echo_enabled;
};

namespace Ui {
class SerialSettingWidget;
}

class SerialSettingWidget : public QWidget
{
  Q_OBJECT

public:
  explicit SerialSettingWidget(QWidget *parent = nullptr);
  ~SerialSettingWidget();

  SerialSetting GetSerialSetting() const;

private:
  Ui::SerialSettingWidget *ui;

  void FillPortParameter();
  void FillAvailablePort();
  void DisplayPortInfo(int index);
};

#endif // SERIALSETTINGWIDGET_H
