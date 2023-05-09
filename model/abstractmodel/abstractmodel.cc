#include "abstractmodel.h"

AbstractModel::AbstractModel(QObject *parent)
    : QObject{parent}, m_line_count{0}, m_threads{this} {
  connect(&m_parser, SIGNAL(RawData(int, QPointF)), this,
          SIGNAL(NewPoint(int, QPointF)));
  connect(&m_parser, SIGNAL(NewData()), this, SLOT(RemoveAll()));
  int thread_count = QThread::idealThreadCount() / 2;
  m_threads.setMaxThreadCount(thread_count ? thread_count : 1);
}

void AbstractModel::InitData(QString file_path) {
  m_parser.readFile(file_path);
  if (m_parser.m_raw_data.size() > 1) {
    m_data.resize(m_parser.m_raw_data.size());
    m_weight.resize(m_parser.m_weight.size());
    m_diff = m_parser.m_raw_data[0].first.toMSecsSinceEpoch();
    for (int i = 0; i < m_parser.m_raw_data.size(); ++i) {
      m_data[i].first =
          (m_parser.m_raw_data[i].first.toMSecsSinceEpoch() - m_diff) /
          3600000 / 24;  // 3600000 msecs to hours
      m_data[i].second = m_parser.m_raw_data[i].second;
      m_weight[i] = m_parser.m_weight[i];
    }
  }
}

void AbstractModel::AddData(int line, QPointF data) {
  emit NewPoint(line, data);
}

void AbstractModel::RemoveGraph([[maybe_unused]] int line) {
  if (m_line_count) {
    --m_line_count;
  }
}

void AbstractModel::RemoveAll() {
  m_line_count = 0;
  m_data.clear();
  m_weight.clear();
}

double AbstractModel::DataToDate(double value) {
  return value * 24 * 3600000 + m_diff;
}
