#include "approximation.h"

Approximation::Approximation(QObject *parent) : AbstractModel{parent} {}

void Approximation::MLQ(int degree, int point_count, int m, bool use_weights) {
  int line = m_line_count++;
  QVector<QVector<double>> matrix(degree + 1, QVector<double>(degree + 1));
  QVector<double> b(degree + 1);
  InitGauss(matrix, b, use_weights);
  m_a = SolveGauss(matrix, b);

  AddM(m);
  for (int i = 0; i < m_data.size() - 1; ++i) {
    double start = m_data[i].first;
    double finish = m_data[i + 1].first;
    double step = (finish - start) / point_count;

    for (double x = start; x <= finish; x += step) {
      AddData(line, {DataToDate(x), GetApproximationValue(x)});
    }
  }
  DelM(m);
}

double Approximation::MQL(const QDateTime value) {
  double result = NAN;
  if (!m_a.isEmpty()) {
    result = (value.toMSecsSinceEpoch() - m_diff) / 3600000 / 24;
    result = GetApproximationValue(result);
  }
  return result;
}

void Approximation::InitGauss(QVector<QVector<double>> &matrix,
                              QVector<double> &b, bool use_weights) {
  for (int i = 0; i < matrix.size(); ++i) {
    for (int j = 0; j < matrix.size(); ++j) {
      matrix[i][j] = 0;
      for (int k = 0; k < m_data.size(); ++k) {
        double x = m_data[k].first;
        if (use_weights) {
          matrix[i][j] += m_weight[k] * qPow(x, i + j);
        } else {
          matrix[i][j] += qPow(x, i + j);
        }
      }
    }
  }

  for (int i = 0; i < b.size(); ++i) {
    b[i] = 0;
    for (int k = 0; k < m_data.size(); ++k) {
      double x = m_data[k].first;
      double y = m_data[k].second;
      if (use_weights) {
        b[i] += m_weight[k] * qPow(x, i) * y;
      } else {
        b[i] += qPow(x, i) * y;
      }
    }
  }
}

QVector<double> Approximation::SolveGauss(QVector<QVector<double>> &matrix,
                                          QVector<double> &b) {
  QVector<double> result(matrix.size());

  for (int k = 0; k < matrix.size(); ++k) {
    for (int i = k + 1; i < matrix.size(); ++i) {
      double koef = matrix[i][k] / matrix[k][k];
      for (int j = k; j < matrix.size(); ++j) {
        matrix[i][j] -= koef * matrix[k][j];
      }
      b[i] -= koef * b[k];
    }
  }

  for (int i = matrix.size() - 1; i >= 0; --i) {
    double s = 0;
    for (int j = i; j < matrix.size(); ++j) {
      s += matrix[i][j] * result[j];
    }
    result[i] = (b[i] - s) / matrix[i][i];
  }

  return result;
}

double Approximation::GetApproximationValue(double x) {
  double value = 0.0;
  for (int j = 0; j < m_a.size(); ++j) {
    value += m_a[j] * qPow(x, j);
  }
  return value;
}

void Approximation::AddM(int m) {
  for (int i = 0; i < m; ++i) {
    double value = m_data[m_data.size() - 1].first + 1;
    m_data.push_back(QPair<double, double>(value, 0.0));
  }
}

void Approximation::DelM(int m) {
  for (int i = 0; i < m; ++i) {
    m_data.pop_back();
  }
}
