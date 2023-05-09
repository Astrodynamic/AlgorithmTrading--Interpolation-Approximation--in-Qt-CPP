#ifndef DATETIMEDIALOG_H
#define DATETIMEDIALOG_H

#include <QCalendarWidget>
#include <QDateTime>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTimeEdit>
#include <QVBoxLayout>

class DateSelectDialog : public QDialog {
 public:
  explicit DateSelectDialog(QPair<QDate, QDate>, QWidget *parent = nullptr);
  QDateTime SelectedDate();

 private:
  QCalendarWidget *m_date;
  QTimeEdit *m_time;
 private slots:
};
#endif  // DATETIMEDIALOG_H
