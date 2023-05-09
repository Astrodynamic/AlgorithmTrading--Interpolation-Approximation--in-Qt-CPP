#ifndef ALGORITHM_TRADING_SRC_TABLECONTROLLER_TABLECONTROLLER_H
#define ALGORITHM_TRADING_SRC_TABLECONTROLLER_TABLECONTROLLER_H

#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>

class TableController : public QTableWidget {
  Q_OBJECT
 public:
  explicit TableController(QWidget *parent = nullptr);
  bool AddLine(QString);

 private:
  void AddNewRow(int, int, QString);

 public slots:
  void RemoveAll();
  void ItemAction(QTableWidgetItem *);
  void ItemRename(QTableWidgetItem *);

 signals:
  void RemoveRow(int);
  void RenameRow(int, QString);
};

#endif  // ALGORITHM_TRADING_SRC_TABLECONTROLLER_TABLECONTROLLER_H
