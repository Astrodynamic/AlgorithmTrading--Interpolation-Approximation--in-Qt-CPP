#ifndef ALGORITHM_TRADING_SRC_CHARTS_SIMPLECHART_H_
#define ALGORITHM_TRADING_SRC_CHARTS_SIMPLECHART_H_

#include <QChartView>
#include <QLineSeries>
#include <QRectF>
#include <QThread>
#include <QThreadPool>
#include <QValueAxis>
#include <QtCharts>

class SimpleChart : public QChartView {
  Q_OBJECT
 public:
  explicit SimpleChart(QWidget *parent = nullptr);
  ~SimpleChart();

 protected:
  QPointF m_last_mouse_pos;

  double m_min_x{0}, m_max_x{INFINITY};
  double m_min_y{0}, m_max_y{INFINITY};

  QThreadPool TPMinMax, TPInsert, TPAdd;
  QVector<QList<QPointF>> m_temp_list;

  QList<QLineSeries *> m_all_series;

  QDateTimeAxis *m_axisX;
  QValueAxis *m_axisY;

  void keyPressEvent(QKeyEvent *event);
  void wheelEvent(QWheelEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void SetMinMax();
  void FindMinMax();

 private:
  QTimer *m_timer;

  void SetXAxis();
  void SetYAxis();
  void SetThread();
  void Init();
  void Sort(QList<QPointF>, QLineSeries *);
  void UpdateMinMax(QPointF);
  void UpdateX(const QPointF &);
  void UpdateY(const QPointF &);

 public slots:
  void AddData(int, QPointF);
  void AllClear();
  void DropMinMax();

 private slots:
  void AddAndSortData();
};

#endif  // ALGORITHM_TRADING_SRC_CHARTS_SIMPLECHART_H_
