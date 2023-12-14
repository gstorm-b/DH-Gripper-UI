#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dh-robotic/dhr_rgi.h"

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

  void RgiInitialize();

  void on_button_click_serial_connect();

private:
  Ui::MainWindow *ui;
  DHR_RGI *m_rgi_device;
  bool gripstatic;
};
#endif // MAINWINDOW_H
