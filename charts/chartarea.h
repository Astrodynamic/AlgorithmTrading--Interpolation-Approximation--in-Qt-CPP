#ifndef ALGORITHM_TRADING_SRC_CHARTS_CHARTAREA_H_
#define ALGORITHM_TRADING_SRC_CHARTS_CHARTAREA_H_

#include <QDateTime>
#include <QDateTimeAxis>
#include <QPair>
#include <algorithm>

#include "simplechart.h"

class ChartArea : public SimpleChart {
  Q_OBJECT
 public:
  explicit ChartArea(QWidget *parent = nullptr);
  ~ChartArea();
  QPair<QDate, QDate> GetMinMax();

 protected:
  void mouseDoubleClickEvent(QMouseEvent *event);

 private:
  double m_default_min_x, m_default_max_x;
  double m_default_min_y, m_default_max_y;

  void Init();
  void SetXAxis();
  void SetYAxis();
  void AddGraph(int);

  void UpdateDefault(QPointF);

 public slots:
  void RenameGraph(int, QString);
  void RemoveGraph(int);
  void AddData(int, QPointF);
  void AllClear();

  void FullClear();
};

#endif  // ALGORITHM_TRADING_SRC_CHARTS_CHARTAREA_H_
