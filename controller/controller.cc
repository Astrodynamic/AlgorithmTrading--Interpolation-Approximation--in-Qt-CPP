#include "controller.h"

#include "./ui_view.h"

Controller::Controller(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::View),
      m_model_int(new Interpolation),
      m_model_app(new Approximation),
      m_model_research(new Interpolation),
      pool(QThreadPool::globalInstance()) {
  ui->setupUi(this);
  ui->int_degree_sbx->hide();
  SetChartArea();
  SetModels();
  SetTable();
  last_m = ui->dsb_app_points_after->value();
}

Controller::~Controller() { delete ui; }

void Controller::SetChartArea() {
  connect(ui->int_tab, SIGNAL(RenameRow(int, QString)), ui->int_graph_wgt,
          SLOT(RenameGraph(int, QString)));
  connect(ui->int_tab, SIGNAL(RemoveRow(int)), ui->int_graph_wgt,
          SLOT(RemoveGraph(int)));
  connect(m_model_int->ConnectParser(), SIGNAL(NewData()), ui->int_graph_wgt,
          SLOT(DropMinMax()));
  connect(m_model_int->ConnectParser(), SIGNAL(NewData()), ui->int_graph_wgt,
          SLOT(FullClear()));
  connect(m_model_int, SIGNAL(NewPoint(int, QPointF)), ui->int_graph_wgt,
          SLOT(AddData(int, QPointF)));

  connect(ui->app_tab, SIGNAL(RenameRow(int, QString)), ui->app_graph_wgt,
          SLOT(RenameGraph(int, QString)));
  connect(ui->app_tab, SIGNAL(RemoveRow(int)), ui->app_graph_wgt,
          SLOT(RemoveGraph(int)));
  connect(m_model_app->ConnectParser(), SIGNAL(NewData()), ui->app_graph_wgt,
          SLOT(DropMinMax()));
  connect(m_model_app->ConnectParser(), SIGNAL(NewData()), ui->app_graph_wgt,
          SLOT(FullClear()));
  connect(m_model_app, SIGNAL(NewPoint(int, QPointF)), ui->app_graph_wgt,
          SLOT(AddData(int, QPointF)));

  connect(m_model_research, SIGNAL(ResearchData(int, QPointF)), ui->res_graph,
          SLOT(AddData(int, QPointF)));
  connect(m_model_research, SIGNAL(ResearchDone()), this,
          SLOT(unlockResearch()));
}

void Controller::SetModels() {
  connect(ui->int_tab, SIGNAL(RemoveRow(int)), m_model_int,
          SLOT(RemoveGraph(int)));
  connect(ui->app_tab, SIGNAL(RemoveRow(int)), m_model_app,
          SLOT(RemoveGraph(int)));
}

void Controller::SetTable() {
  connect(m_model_int->ConnectParser(), SIGNAL(MinPoints(int)), this,
          SLOT(setIntDegreeMax(int)));
  connect(m_model_research->ConnectParser(), SIGNAL(MinPoints(int)), this,
          SLOT(setIntDegreeMax(int)));
  connect(m_model_app->ConnectParser(), SIGNAL(MinPoints(int)), this,
          SLOT(setAppDegreeMax(int)));
  connect(ui->btn_int_clear, SIGNAL(clicked()), ui->int_tab, SLOT(RemoveAll()));
  connect(ui->btn_app_clear, SIGNAL(clicked()), ui->app_tab, SLOT(RemoveAll()));
}

void Controller::on_int_add_graph_clicked() {
  if (!ui->int_file_path_lbl->text().isEmpty() &&
      ui->int_tab->AddLine(
          ui->sb_int_points->text() + " " + ui->int_type_cbox->currentText() +
          (ui->int_type_cbox->currentIndex() ? " " + ui->int_degree_sbx->text()
                                             : ""))) {
    if (ui->int_type_cbox->currentIndex()) {
      pool.start([=]() {
        m_model_int->Newton(ui->int_degree_sbx->value(),
                            ui->sb_int_points->value());
      });

    } else {
      pool.start([=]() { m_model_int->Spline(ui->sb_int_points->value()); });
    }
  }
}

void Controller::on_app_add_graph_clicked() {
  QString weights;
  if (ui->app_usersweights_cbx->isChecked()) {
    weights = " Users weights";
  }
  if (last_m != ui->dsb_app_points_after->value()) {
    last_m = ui->dsb_app_points_after->value();
    emit ui->btn_app_clear->clicked();
  }
  if (!ui->app_file_path_lbl->text().isEmpty() &&
      ui->app_tab->AddLine(
          ui->sb_app_points->text() + " " +
          QString("Polinominal " + ui->app_degree_sbx->text()) + " " +
          ui->dsb_app_points_after->text() + weights)) {
    pool.start([=]() {
      m_model_app->MLQ(ui->app_degree_sbx->value(), ui->sb_app_points->value(),
                       ui->dsb_app_points_after->value(),
                       ui->app_usersweights_cbx->isChecked());
    });
  }
}

void Controller::on_int_type_cbox_currentIndexChanged(int index) {
  if (!index) {
    ui->int_degree_sbx->hide();
  } else {
    ui->int_degree_sbx->show();
  }
}

void Controller::setAppDegreeMax(int val) {
  ui->app_degree_sbx->setMaximum((val > 50) ? 50 : val);
}

void Controller::unlockResearch() {
  ui->res_open_btn->blockSignals(false);
  ui->res_calc_btn->blockSignals(false);
}

void Controller::setIntDegreeMax(int val) {
  ui->int_degree_sbx->setMaximum((val > 8) ? 8 : val);
}

void Controller::on_app_open_btn_clicked() {
  QString path = QFileDialog::getOpenFileName(
      this, tr("Load data"), QDir::homePath(), tr("Data (*.csv)"));
  if (!path.isEmpty()) {
    emit ui->btn_app_clear->clicked();
    ui->app_file_path_lbl->setText(path);
    m_model_app->InitData(path);
  }
}

void Controller::on_int_open_btn_clicked() {
  QString path = QFileDialog::getOpenFileName(
      this, tr("Load data"), QDir::homePath(), tr("Data (*.csv)"));
  if (!path.isEmpty()) {
    emit ui->btn_int_clear->clicked();
    ui->int_file_path_lbl->setText(path);
    m_model_int->InitData(path);
  }
}

void Controller::on_res_open_btn_clicked() {
  QString path = QFileDialog::getOpenFileName(
      this, tr("Load data"), QDir::homePath(), tr("Data (*.csv)"));
  if (!path.isEmpty()) {
    m_model_research->InitData(path);
    ui->res_path_lbl->setText(path);
  }
}

void Controller::on_app_data_to_date_clicked() {
  auto dialog_date = DateSelectDialog(ui->app_graph_wgt->GetMinMax(), this);
  auto dialog_response = QMessageBox(this);
  if (dialog_date.exec() == QDialog::Accepted) {
    QDateTime selected_date = dialog_date.SelectedDate();
    QString text = "On date " + selected_date.toString() + " value is " +
                   QString::number(m_model_app->MQL(selected_date));
    dialog_response.setText(text);
    dialog_response.exec();
  }
  dialog_date.deleteLater();
}

void Controller::on_int_data_to_date_clicked() {
  auto dialog = DateSelectDialog(ui->int_graph_wgt->GetMinMax(), this);
  auto dialog_response = QMessageBox(this);
  if (dialog.exec() == QDialog::Accepted) {
    QDateTime selected_date = dialog.SelectedDate();
    QString text = "On date " + selected_date.toString() +
                   "\nSpline value is " +
                   QString::number(m_model_int->Spline(selected_date)) +
                   "\nNewton value is " +
                   QString::number(m_model_int->Newton(selected_date));
    dialog_response.setText(text);
    dialog_response.exec();
  }
  dialog.deleteLater();
}

void Controller::on_res_calc_btn_clicked() {
  if (!ui->res_path_lbl->text().isEmpty()) {
    ui->res_graph->AllClear();
    ui->res_open_btn->blockSignals(true);
    ui->res_calc_btn->blockSignals(true);
    pool.start([=]() {
      m_model_research->Research(ui->res_k_sbx->value(),
                                 ui->res_h_sbx->value());
    });
  }
}
