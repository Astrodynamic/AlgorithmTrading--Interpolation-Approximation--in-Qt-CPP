#include "datetimedialog.h"

DateSelectDialog::DateSelectDialog(QPair<QDate, QDate> MinMax, QWidget *parent)
    : QDialog(parent) {
  auto cancel_button = new QPushButton("Cancel", this);
  connect(cancel_button, &QPushButton::clicked, this,
          &DateSelectDialog::reject);
  auto layout = new QVBoxLayout(this);
  if (MinMax.first.isNull() || MinMax.second.isNull()) {
    setWindowTitle("Error, No data");
    layout->addWidget(new QLabel("Load data file", this));
    cancel_button->setText("Close");
  } else {
    auto ok_button = new QPushButton("OK", this);
    connect(ok_button, &QPushButton::clicked, this, &DateSelectDialog::accept);
    m_date = new QCalendarWidget(this);
    m_date->setSelectedDate(MinMax.first);
    m_date->setMinimumDate(MinMax.first);
    m_date->setMaximumDate(MinMax.second);
    layout->addWidget(new QLabel("Time", this));
    m_time = new QTimeEdit(this);

    m_time->setDisplayFormat("HH:mm:ss.zzz");
    layout->addWidget(m_time);
    layout->addWidget(m_date);
    layout->addWidget(ok_button);
    setWindowTitle("Select a date");
  }
  layout->addWidget(cancel_button);
}

QDateTime DateSelectDialog::SelectedDate() {
  return QDateTime(m_date->selectedDate(), m_time->time());
}
