#include "parser.h"
Parser::Parser(QObject *parent) : QObject{parent} {}

void Parser::readFile(const QString filename) {
  QFile file(filename);
  if (file.open(QFile::ReadOnly)) {
    m_raw_data.clear();
    m_weight.clear();
    emit NewData();

    QTextStream stream(&file);
    stream.readLine();

    QPair<QDateTime, double> data;
    while (!stream.atEnd()) {
      QString str(stream.readLine());
      QStringList fields = str.split(',', Qt::SkipEmptyParts);
      data.first.setDate(QDate::fromString(fields[0], "yyyy-MM-dd"));
      data.second = fields[1].toDouble();
      m_raw_data.push_back(data);
      if (fields.size() == 3) {
        m_weight.push_back(fields[2].toDouble());
      } else {
        m_weight.push_back(1.0);
      }
      emit RawData(-1, {static_cast<qreal>(data.first.toMSecsSinceEpoch()),
                        data.second});
    }

    emit MinPoints(m_raw_data.size());
    file.close();
  }
}
