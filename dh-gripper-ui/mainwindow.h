#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "dh-robotic/dhcontroller.h"
#include "dh-robotic/serialsettingdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace dhr;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  void InitAction();

  void DhInitialize();
  void DhDisplayRgiInfo(RGIData device_info);

  void on_button_click_serial_connect();

private:
  Ui::MainWindow *ui;
  DHController *m_dh_controller;
  int m_rgi_address;
};
#endif // MAINWINDOW_H
