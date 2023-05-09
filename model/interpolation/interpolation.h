#ifndef ALGORITHM_TRADING_SRC_INTERPOLATION_INTERPOLATION_H_
#define ALGORITHM_TRADING_SRC_INTERPOLATION_INTERPOLATION_H_

#include "abstractmodel.h"

class Interpolation : public AbstractModel {
  Q_OBJECT
 public:
  explicit Interpolation(QObject *parent = nullptr);
  void Newton(const int, const int);
  double Newton(const QDateTime);
  void Spline(const int);
  double Spline(const QDateTime);
  void Research(const int, const int);

 private:
  struct m_spline_tuple {
    double a, b, c, d, x;
  };
  QVector<m_spline_tuple> m_splines;
  QVector<QVector<double>> m_newton_diff;
  QVector<QVector<QPair<double, double>>> m_newton_data;
  int m_newton_iterations{};
  void ConfigureNewton(const int);
  void FillData(const int, const int, const int);
  void FillDifferences(const int);
  double GetNewtonValue(const double, const int);
  void SplineBuild();
  void ThomasAlgo();
  double GetSpline(const double);
  QPair<double, double> Info(const int);

 signals:
  void ResearchData(int, QPointF);
  void ResearchDone();
};

#endif  // ALGORITHM_TRADING_SRC_INTERPOLATION_INTERPOLATION_H_
