#ifndef DHRGIWIDGET_H
#define DHRGIWIDGET_H

#include <QWidget>
#include "dhr_define.h"
#include "dh_rgi.h"

namespace Ui {
class DhRgiWidget;
}

using namespace dhr;

class DhRgiWidget : public QWidget
{
  Q_OBJECT

public:
  explicit DhRgiWidget(QWidget *parent = nullptr);
  ~DhRgiWidget();

  void ShowRgiDeviceSetParameters(RGIData::RGIFeedback feedback_data);
  void ShowRgiDeviceInfo(RGIData::RGIFeedback feedback_data);

private:
  void InitSignals();

signals:
  void SignalsGripper_PositionEdited(int position);
  void SignalsGripper_ForceEdited(int force);
  void SignalsGripper_SpeedEdited(int speed);
  void SignalsRotation_AngleEdited(int position);
  void SignalsRotation_TorqueEdited(int force);
  void SignalsRotation_SpeedEdited(int speed);

private:
  Ui::DhRgiWidget *ui;
};

#endif // DHRGIWIDGET_H
