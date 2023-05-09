#include "simplechart.h"

SimpleChart::SimpleChart(QWidget *parent)
    : QChartView{parent},
      TPMinMax(QThreadPool::globalInstance()),
      TPInsert(QThreadPool::globalInstance()),
      TPAdd(QThreadPool::globalInstance()),
      m_timer(new QTimer(this)) {
  Init();
}

SimpleChart::~SimpleChart() { chart()->removeAllSeries(); }

void SimpleChart::keyPressEvent(QKeyEvent *event) {
  static const std::map<int, std::function<void()>> keyHandlers = {
      {Qt::Key_Plus, [this]() { chart()->zoomIn(); }},
      {Qt::Key_Minus, [this]() { chart()->zoomOut(); }},
      {Qt::Key_Up, [this]() { chart()->scroll(0, 50); }},
      {Qt::Key_Down, [this]() { chart()->scroll(0, -50); }},
      {Qt::Key_Right, [this]() { chart()->scroll(50, 0); }},
      {Qt::Key_Left, [this]() { chart()->scroll(-50, 0); }},
  };

  auto handler = keyHandlers.find(event->key());
  if (handler != keyHandlers.end()) {
    handler->second();
    event->accept();
  }
}

void SimpleChart::wheelEvent(QWheelEvent *event) {
  int angleDelta = event->angleDelta().y();
  QChart *chart = this->chart();

  if (angleDelta > 0) {
    chart->zoomIn();
  } else {
    chart->zoomOut();
  }

  int xPosition = event->position().rx();
  int yPosition = event->position().ry();
  int chartWidth = chart->rect().width();
  int chartHeight = chart->rect().height();

  chart->scroll(xPosition - (chartWidth / 2), (chartHeight / 2) - yPosition);
  event->accept();
}

void SimpleChart::mousePressEvent(QMouseEvent *event) {
  if (event->buttons() == Qt::RightButton) {
    m_last_mouse_pos = event->position();
  }
}

void SimpleChart::mouseMoveEvent(QMouseEvent *event) {
  if (event->buttons() == Qt::RightButton && !m_last_mouse_pos.isNull()) {
    const QPointF delta = m_last_mouse_pos - event->position();
    chart()->scroll(delta.x(), -delta.y());
    m_last_mouse_pos = event->position();
  }
}

void SimpleChart::mouseReleaseEvent(QMouseEvent *event) {
  if (event->buttons() == Qt::RightButton) {
    m_last_mouse_pos = {};
  }
}

void SimpleChart::mouseDoubleClickEvent(QMouseEvent *event) {
  chart()->zoomReset();
  SetMinMax();
  event->accept();
}

void SimpleChart::SetMinMax() {
  chart()
      ->axes(Qt::Horizontal)
      .first()
      ->setMax(QDateTime::fromMSecsSinceEpoch(m_max_x));
  chart()
      ->axes(Qt::Horizontal)
      .first()
      ->setMin(QDateTime::fromMSecsSinceEpoch(m_min_x));
  chart()->axes(Qt::Vertical).first()->setMax(m_max_y);
  chart()->axes(Qt::Vertical).first()->setMin(m_min_y);
}

void SimpleChart::AllClear() {
  for (auto *it : m_all_series) {
    it->clear();
  }
}

void SimpleChart::Sort(QList<QPointF> list, QLineSeries *series) {
  std::sort(list.begin(), list.end(),
            [](const QPointF &a, const QPointF &b) { return a.x() < b.x(); });
  series->replace(list);
}

void SimpleChart::AddData(int position, QPointF data) {
  TPMinMax.start([=]() { UpdateX(data); });
  TPMinMax.start([=]() { UpdateY(data); });
  TPAdd.start([=]() { m_temp_list[position].append(data); });
}

void SimpleChart::AddAndSortData() {
  for (int i = 0; i < m_temp_list.size(); ++i) {
    if (!m_temp_list[i].isEmpty()) {
      TPAdd.start([=]() {
        TPInsert.waitForDone();
        QList<QPointF> new_list = m_all_series[i]->points() + m_temp_list[i];
        TPInsert.start([=]() { Sort(new_list, m_all_series[i]); });
        m_temp_list[i].clear();
      });
    }
  }
}

void SimpleChart::SetThread() {
  TPMinMax.setMaxThreadCount(1);
  TPInsert.setMaxThreadCount(1);
  TPInsert.setThreadPriority(QThread::HighPriority);
  TPAdd.setMaxThreadCount(1);
}

void SimpleChart::Init() {
  SetThread();
  SetXAxis();
  SetYAxis();
  m_temp_list.resize(2);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(AddAndSortData()));
  m_timer->start(10);

  setMouseTracking(true);
  setRenderHint(QPainter::Antialiasing);
  setRubberBand(QChartView::HorizontalRubberBand);

  m_all_series.append(new QLineSeries);
  m_all_series[0]->setName("Cube Spline time");

  m_all_series.append(new QLineSeries);
  m_all_series[1]->setName("Newton polinom time");
  for (auto *it : m_all_series) {
    chart()->addSeries(it);
    it->attachAxis(m_axisX);
    it->attachAxis(m_axisY);
    it->setPointsVisible(true);
  }
}

void SimpleChart::UpdateMinMax(QPointF data) {
  UpdateX(data);
  UpdateY(data);
}

void SimpleChart::UpdateX(const QPointF &data) {
  if (m_min_x == 0) {
    m_min_x = data.x();
    chart()
        ->axes(Qt::Horizontal)
        .first()
        ->setMin(QDateTime::fromMSecsSinceEpoch(m_min_x));
  } else {
    if (m_min_x > data.x()) {
      m_min_x = data.x();
      chart()
          ->axes(Qt::Horizontal)
          .first()
          ->setMin(QDateTime::fromMSecsSinceEpoch(m_min_x));
    }
  }
  if (m_max_x == INFINITY) {
    m_max_x = data.x();
    chart()
        ->axes(Qt::Horizontal)
        .first()
        ->setMax(QDateTime::fromMSecsSinceEpoch(m_max_x));
  } else {
    if (m_max_x < data.x()) {
      m_max_x = data.x();
      chart()
          ->axes(Qt::Horizontal)
          .first()
          ->setMax(QDateTime::fromMSecsSinceEpoch(m_max_x));
    }
  }
}

void SimpleChart::UpdateY(const QPointF &data) {
  if (m_min_y == 0) {
    m_min_y = data.y();
    chart()->axes(Qt::Vertical).first()->setMin(m_min_y);
  } else {
    if (m_min_y > data.y()) {
      m_min_y = data.y();
      chart()->axes(Qt::Vertical).first()->setMin(m_min_y);
    }
  }
  if (m_max_y == INFINITY) {
    m_max_y = data.y();
    chart()->axes(Qt::Vertical).first()->setMax(m_max_y);
  } else {
    if (m_max_y < data.y()) {
      m_max_y = data.y();
      chart()->axes(Qt::Vertical).first()->setMax(m_max_y);
    }
  }
}

void SimpleChart::FindMinMax() {
  for (auto *series : m_all_series) {
    TPMinMax.start([=]() { UpdateX(series->at(0)); });
    TPMinMax.start([=]() {
      UpdateX(series->at((series->count() == 0) ? 0 : (series->count() - 1)));
    });

    for (int i = 0; i < series->count(); ++i) {
      TPMinMax.start([=]() { UpdateY(series->at(i)); });
    }
  }
}

void SimpleChart::DropMinMax() {
  m_min_x = 0;
  m_max_x = INFINITY;
  m_min_y = 0;
  m_max_y = INFINITY;
}

void SimpleChart::SetXAxis() {
  m_axisX = new QDateTimeAxis;
  m_axisX->setFormat("mm:ss.zzz");
  m_axisX->setTitleText("Calculate time");
  chart()->addAxis(m_axisX, Qt::AlignBottom);
}

void SimpleChart::SetYAxis() {
  m_axisY = new QValueAxis;
  m_axisY->setLabelFormat("%.0f");
  m_axisY->setTitleText("Points count");
  chart()->addAxis(m_axisY, Qt::AlignLeft);
}
