
#ifndef ALGORITHM_TRADING_SRC_ABSTRACTMODEL_ABSTRACTMODEL_H_
#define ALGORITHM_TRADING_SRC_ABSTRACTMODEL_ABSTRACTMODEL_H_

#include <QDateTime>
#include <QObject>
#include <QPointF>
#include <QThread>
#include <QThreadPool>
#include <QVector>

#include "parser.h"

class AbstractModel : public QObject {
  Q_OBJECT
 public:
  explicit AbstractModel(QObject* parent = nullptr);
  const Parser* ConnectParser() const { return &m_parser; }

 protected:
  Parser m_parser;
  int m_line_count;
  QVector<QPair<double, double>> m_data;
  QVector<double> m_weight;
  double m_diff;
  QThreadPool m_threads;
  double DataToDate(double);

 public slots:
  void InitData(QString);
  void AddData(int, QPointF);
  void RemoveGraph([[maybe_unused]] int);
  void RemoveAll();

 signals:
  void NewPoint(int, QPointF);
};

#endif  // ALGORITHM_TRADING_SRC_ABSTRACTMODEL_ABSTRACTMODEL_H_
