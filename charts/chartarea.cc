#include "chartarea.h"

ChartArea::ChartArea(QWidget *parent)
    : SimpleChart{parent},
      m_default_min_x{0},
      m_default_max_x{INFINITY},
      m_default_min_y{0},
      m_default_max_y{INFINITY} {
  Init();
}

ChartArea::~ChartArea() {}

void ChartArea::Init() {
  chart()->removeAllSeries();
  chart()->setAnimationOptions(QChart::SeriesAnimations);
  m_all_series.clear();
  m_temp_list.resize(6);
  SetXAxis();
  SetYAxis();
  AddGraph(0);
}

QPair<QDate, QDate> ChartArea::GetMinMax() {
  if (m_default_max_x == INFINITY || m_default_min_x == 0) {
    return {QDate(), QDate()};
  }
  return {QDateTime::fromMSecsSinceEpoch(m_min_x).date(),
          QDateTime::fromMSecsSinceEpoch(m_max_x).date()};
}

void ChartArea::mouseDoubleClickEvent(QMouseEvent *event) {
  chart()->zoomReset();
  if (m_all_series.count() == 1) {
    chart()
        ->axes(Qt::Horizontal)
        .first()
        ->setMin(QDateTime::fromMSecsSinceEpoch(m_default_min_x));
    chart()
        ->axes(Qt::Horizontal)
        .first()
        ->setMax(QDateTime::fromMSecsSinceEpoch(m_default_max_x));
    chart()->axes(Qt::Vertical).first()->setMin(m_default_min_y);
    chart()->axes(Qt::Vertical).first()->setMax(m_default_max_y);
  } else if (m_all_series.count() > 1) {
    SetMinMax();
  }
  event->accept();
}

void ChartArea::SetXAxis() {
  m_axisX->setFormat("dd.MM.yyyy");
  m_axisX->setTitleText("Date");
}

void ChartArea::SetYAxis() {
  m_axisY->setLabelFormat("%f");
  m_axisY->setTitleText("Price");
}

void ChartArea::AddGraph(int position) {
  if (position == m_all_series.size()) {
    m_all_series.append(new QLineSeries);
    chart()->addSeries(m_all_series[position]);
    m_all_series[position]->attachAxis(m_axisX);
    m_all_series[position]->attachAxis(m_axisY);
    if (!position) {
      m_all_series[position]->setName("Row data");
      m_all_series[position]->setPointsVisible();
    }
  }
}

void ChartArea::UpdateDefault(QPointF data) {
  if (m_default_min_x == 0) {
    m_default_min_x = data.x();
  } else {
    if (m_default_min_x > data.x()) m_default_min_x = data.x();
  }
  if (m_default_max_x == INFINITY) {
    m_default_max_x = data.x();
  } else {
    if (m_default_max_x < data.x()) m_default_max_x = data.x();
  }
  if (m_default_min_y == 0) {
    m_default_min_y = data.y();
  } else {
    if (m_default_min_y > data.y()) m_default_min_y = data.y();
  }
  if (m_default_max_y == INFINITY) {
    m_default_max_y = data.y();
  } else {
    if (m_default_max_y < data.y()) m_default_max_y = data.y();
  }
}

void ChartArea::RenameGraph(int position, QString name) {
  AddGraph(++position);
  m_all_series[position]->setName(name);
}

void ChartArea::AddData(int position, QPointF data) {
  ++position;
  if (!position) {
    TPMinMax.start([=]() { UpdateDefault(data); });
  }
  AddGraph(position);
  SimpleChart::AddData(position, data);
}

void ChartArea::AllClear() {
  TPMinMax.clear();
  TPMinMax.waitForDone();
  for (int i = 1; i < m_all_series.size(); ++i) {
    chart()->removeSeries(m_all_series[i]);
  }
  m_all_series.clear();
}

void ChartArea::FullClear() {
  TPMinMax.clear();
  TPMinMax.waitForDone();
  chart()->removeAllSeries();
  m_all_series.clear();
}

void ChartArea::RemoveGraph(int position) {
  TPMinMax.clear();
  TPMinMax.waitForDone();
  chart()->removeSeries(m_all_series[++position]);
  m_all_series.removeAt(position);

  DropMinMax();
  FindMinMax();
}
