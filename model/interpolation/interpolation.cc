#include "interpolation.h"

Interpolation::Interpolation(QObject *parent) : AbstractModel{parent} {}

void Interpolation::Newton(const int degree, const int point_count) {
  int line = m_line_count++;
  m_newton_iterations = static_cast<int>(m_data.size() / degree);
  ConfigureNewton(degree);

  for (int i = 0; i < m_newton_iterations; ++i) {
    for (int j = 0; j < m_newton_data[i].size() - 1; ++j) {
      double start = m_newton_data[i][j].first;
      double finish = m_newton_data[i][j + 1].first;
      double step = (finish - start) / point_count;

      auto task = [=]() {
        for (double x = start; x < finish; x += step) {
          AddData(line, {DataToDate(x), GetNewtonValue(x, i)});
        }
        AddData(line, {DataToDate(finish), GetNewtonValue(finish, i)});
      };
      m_threads.start(task);
    }
    m_threads.waitForDone();
  }
}

double Interpolation::Newton(const QDateTime value) {
  double result = NAN;
  if (!m_newton_diff.isEmpty() && !m_newton_data.isEmpty()) {
    result = (value.toMSecsSinceEpoch() - m_diff) / 3600000 / 24;
    int index = 0;
    for (int i = 1; i < m_newton_iterations; ++i) {
      if (result >= m_newton_data[i][0].first &&
          result <= m_newton_data[i][m_newton_data[i].size() - 1].first) {
        index = i;
        break;
      }
    }
    result = GetNewtonValue(result, index);
  }
  return result;
}

void Interpolation::ConfigureNewton(const int degree) {
  m_newton_diff.clear();
  m_newton_data.clear();
  m_newton_diff.resize(m_newton_iterations);
  m_newton_data.resize(m_newton_iterations);
  for (int i = 0; i < m_newton_iterations; ++i) {
    int begin = i * degree;
    int end = (i != m_newton_iterations - 1) ? (begin + degree) : m_data.size();
    m_newton_data[i].resize(end - begin);
    FillData(i, begin, end);
    m_newton_diff[i].resize(end - begin);
    FillDifferences(i);
  }
}

void Interpolation::FillData(const int index, const int begin, const int end) {
  for (int i = begin, k = 0; i < end; ++i, k++) {
    m_newton_data[index][k] = m_data[i];
  }
}

void Interpolation::FillDifferences(const int index) {
  for (int i = 0; i < m_newton_diff[index].size(); ++i) {
    m_newton_diff[index][i] = m_newton_data[index][i].second;
  }
  for (int j = 1; j < m_newton_diff[index].size(); ++j) {
    for (int i = m_newton_diff[index].size() - 1; i >= j; --i) {
      m_newton_diff[index][i] =
          (m_newton_diff[index][i] - m_newton_diff[index][i - 1]) /
          (m_newton_data[index][i].first - m_newton_data[index][i - j].first);
    }
  }
}

double Interpolation::GetNewtonValue(const double x, const int index) {
  double result = m_newton_data[index][0].second;
  double term = 1.0;
  for (int i = 0; i < m_newton_diff[index].size() - 1; ++i) {
    term *= (x - m_newton_data[index][i].first);
    result += m_newton_diff[index][i + 1] * term;
  }
  return result;
}

void Interpolation::Spline(const int point_count) {
  int line = m_line_count++;
  m_splines.resize(m_data.size());
  SplineBuild();

  for (int i = 0; i < m_data.size() - 1; ++i) {
    double start = m_data[i].first;
    double finish = m_data[i + 1].first;
    double step = (finish - start) / point_count;

    auto task = [=]() {
      for (double x = start; x < finish; x += step) {
        AddData(line, {DataToDate(x), GetSpline(x)});
      }
      AddData(line, {DataToDate(finish), GetSpline(finish)});
    };
    m_threads.start(task);
  }
  m_threads.waitForDone();
}

double Interpolation::Spline(const QDateTime value) {
  double result = NAN;
  if (!m_splines.isEmpty()) {
    result = (value.toMSecsSinceEpoch() - m_diff) / 3600000 / 24;
    result = GetSpline(result);
  }
  return result;
}

void Interpolation::SplineBuild() {
  for (int i = 0; i < m_splines.size(); ++i) {
    m_splines[i].x = m_data[i].first;
    m_splines[i].a = m_data[i].second;
  }
  ThomasAlgo();
  for (int i = m_splines.size() - 1; i > 0; --i) {
    double h_i = m_splines[i].x - m_splines[i - 1].x;
    m_splines[i].d = (m_splines[i].c - m_splines[i - 1].c) / h_i;
    m_splines[i].b = h_i * (2. * m_splines[i].c + m_splines[i - 1].c) / 6. +
                     (m_splines[i].a - m_splines[i - 1].a) / h_i;
  }
}

void Interpolation::ThomasAlgo() {
  QVector<double> alpha(m_splines.size() - 1, 0.0),
      beta(m_splines.size() - 1, 0.0);

  for (int i = 1; i < m_splines.size() - 1; ++i) {
    double h_i = m_splines[i].x - m_splines[i - 1].x;
    double h_i1 = m_splines[i + 1].x - m_splines[i].x;
    double F = 6. * ((m_splines[i + 1].a - m_splines[i].a) / h_i1 -
                     (m_splines[i].a - m_splines[i - 1].a) / h_i);
    double z = h_i * alpha[i - 1] + 2. * (h_i + h_i1);
    alpha[i] = -h_i1 / z;
    beta[i] = (F - h_i * beta[i - 1]) / z;
  }

  for (int i = m_splines.size() - 2; i > 0; --i) {
    m_splines[i].c = alpha[i] * m_splines[i + 1].c + beta[i];
  }
}

double Interpolation::GetSpline(const double x) {
  m_spline_tuple result;
  if (x <= m_splines[0].x) {
    result = m_splines[0];
  } else if (x >= m_splines[m_splines.size() - 1].x) {
    result = m_splines[m_splines.size() - 1];
  } else {
    int i = 0, j = m_splines.size() - 1;
    while (i + 1 < j) {
      double k = i + (j - i) / 2;
      if (x <= m_splines[k].x) {
        j = k;
      } else {
        i = k;
      }
    }
    result = m_splines[j];
  }
  double dx = x - result.x;
  return result.a + (result.b + (result.c / 2. + result.d * dx / 6.) * dx) * dx;
}

void Interpolation::Research(const int k, const int h) {
  int start = m_data.size();
  int step = (k - m_data.size()) / (h - 1);

  for (int i = 0; i < h - 1; start += step, ++i) {
    auto pair = Info(start);
    emit ResearchData(0, QPointF(pair.first, start));
    emit ResearchData(1, QPointF(pair.second, start));
  }
  auto pair = Info(k);
  emit ResearchData(0, QPointF(pair.first, k));
  emit ResearchData(1, QPointF(pair.second, k));
  emit ResearchDone();
}

QPair<double, double> Interpolation::Info(const int value) {
  int point_count = value / m_data.size();

  double t_spline = 0, t_newton = 0;
  for (int i = 0; i < 10; ++i) {
    double temp = QDateTime::currentMSecsSinceEpoch();
    Spline(point_count);
    t_spline += QDateTime::currentMSecsSinceEpoch() - temp;
    temp = QDateTime::currentMSecsSinceEpoch();
    Newton(3, point_count);
    t_newton += QDateTime::currentMSecsSinceEpoch() - temp;
  }
  t_spline /= 10;
  t_newton /= 10;

  return {t_spline, t_newton};
}
