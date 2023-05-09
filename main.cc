#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "controller.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QTranslator translator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();
  for (const QString &locale : uiLanguages) {
    const QString baseName = "AlgorithmicTrading_" + QLocale(locale).name();
    if (translator.load(":/i18n/" + baseName)) {
      app.installTranslator(&translator);
      break;
    }
  }
  Controller controller;
  controller.show();
  return app.exec();
}
