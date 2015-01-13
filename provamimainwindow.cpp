#include "provamimainwindow.h"
#include "ui_provamimainwindow.h"
#include <stdio.h>
#include <set>
#include <map>
#include <dballe/core/defs.h>
#include <dballe/core/var.h>
#include "model.h"

using namespace std;


ProvamiMainWindow::ProvamiMainWindow(Model& model, QWidget *parent) :
    QMainWindow(parent),
    model(model), datagrid_model(model),
    filter_report_model(model),
    filter_level_model(model),
    filter_trange_model(model),
    filter_varcode_model(model),
    ui(new Ui::ProvamiMainWindow)
{
    ui->setupUi(this);

    ui->results->setModel(&datagrid_model);
    ui->filter_report->setModel(&filter_report_model);
    ui->filter_level->setModel(&filter_level_model);
    ui->filter_trange->setModel(&filter_trange_model);
    ui->filter_varcode->setModel(&filter_varcode_model);
}

ProvamiMainWindow::~ProvamiMainWindow()
{
    delete ui;
}

void ProvamiMainWindow::on_refresh_clicked()
{
    model.refresh();
}
