#include "tablecontroller.h"

TableController::TableController(QWidget* parent) : QTableWidget{parent} {
  connect(this, SIGNAL(itemClicked(QTableWidgetItem*)), this,
          SLOT(ItemAction(QTableWidgetItem*)));
  connect(this, SIGNAL(itemChanged(QTableWidgetItem*)), this,
          SLOT(ItemRename(QTableWidgetItem*)));
}

bool TableController::AddLine(QString info) {
  if (rowCount() < 5) {
    int row = rowCount();
    setRowCount(row + 1);
    QStringList text;
    text << ("line " + QString::number(rowCount())) << info << "Remove";
    emit RenameRow(row, text[0]);
    for (int col = 0; col < text.size(); ++col) {
      AddNewRow(row, col, text[col]);
    }
    return true;
  }
  return false;
}

void TableController::AddNewRow(int row, int col, QString text) {
  QTableWidgetItem* new_item = new QTableWidgetItem(text);
  if (col) {
    new_item->setFlags(new_item->flags() ^ Qt::ItemIsEditable);
  }
  setItem(row, col, new_item);
}

void TableController::RemoveAll() {
  while (rowCount()) {
    emit RemoveRow(rowCount() - 1);
    removeRow(rowCount() - 1);
  }
}

void TableController::ItemAction(QTableWidgetItem* item) {
  if (item->column() == columnCount() - 1) {
    emit RemoveRow(item->row());
    removeRow(item->row());
  }
}

void TableController::ItemRename(QTableWidgetItem* item) {
  if (item->column() == 0) {
    emit RenameRow(item->row(), item->text());
  }
}
