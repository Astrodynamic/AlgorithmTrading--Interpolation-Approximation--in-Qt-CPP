#ifndef ALGORITHM_TRADING_CONTROLLER_CONTROLLER_H_
#define ALGORITHM_TRADING_CONTROLLER_CONTROLLER_H_

#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QMainWindow>
#include <QPointF>
#include <QThread>

#include "approximation.h"
#include "chartarea.h"
#include "datetimedialog.h"
#include "interpolation.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class View;
}
QT_END_NAMESPACE

class Controller : public QMainWindow {
  Q_OBJECT

 public:
  Controller(QWidget *parent = nullptr);
  ~Controller();

 private:
  Ui::View *ui;
  Interpolation *m_model_int;
  Approximation *m_model_app;
  Interpolation *m_model_research;
  int last_m;

  QThreadPool pool;

  void SetChartArea();
  void SetModels();
  void AddIntLine();
  void SetTable();

 private slots:
  void on_int_add_graph_clicked();
  void on_app_add_graph_clicked();
  void on_int_type_cbox_currentIndexChanged(int index);
  void setIntDegreeMax(int);
  void setAppDegreeMax(int);
  void unlockResearch();
  void on_app_open_btn_clicked();
  void on_int_open_btn_clicked();
  void on_res_open_btn_clicked();
  void on_app_data_to_date_clicked();
  void on_int_data_to_date_clicked();
  void on_res_calc_btn_clicked();
};
#endif  // ALGORITHM_TRADING_CONTROLLER_CONTROLLER_H_
