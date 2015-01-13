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
    ui(new Ui::ProvamiMainWindow)
{
    ui->setupUi(this);

    ui->results->setModel(&datagrid_model);
    ui->filter_report->setModel(&model.reports);
    ui->filter_level->setModel(&model.levels);
    ui->filter_trange->setModel(&model.tranges);
    ui->filter_varcode->setModel(&model.varcodes);
}

ProvamiMainWindow::~ProvamiMainWindow()
{
    delete ui;
}

void ProvamiMainWindow::on_refresh_clicked()
{
    model.refresh();
}

void ProvamiMainWindow::on_filter_report_activated(int index)
{
    model.reports.set_next_filter(index);
}
void ProvamiMainWindow::on_filter_level_activated(int index)
{
    model.levels.set_next_filter(index);
}
void ProvamiMainWindow::on_filter_trange_activated(int index)
{
    model.tranges.set_next_filter(index);
}
void ProvamiMainWindow::on_filter_varcode_activated(int index)
{
    model.varcodes.set_next_filter(index);
}
