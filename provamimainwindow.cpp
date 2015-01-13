#include "provamimainwindow.h"
#include "ui_provamimainwindow.h"
#include <set>
#include <map>
#include <dballe/core/defs.h>
#include <dballe/core/var.h>
#include <QDebug>
#include "model.h"
#include "mapscene.h"

using namespace std;


ProvamiMainWindow::ProvamiMainWindow(Model& model, QWidget *parent) :
    QMainWindow(parent),
    model(model), datagrid_model(model), map_scene(model),
    ui(new Ui::ProvamiMainWindow)
{
    ui->setupUi(this);

    ui->results->setModel(&datagrid_model);
    ui->filter_report->setModel(&model.reports);
    ui->filter_level->setModel(&model.levels);
    ui->filter_trange->setModel(&model.tranges);
    ui->filter_varcode->setModel(&model.varcodes);

    map_scene.load_coastlines("/home/enrico/lavori/arpa/provami/world.dat");
    ui->mapview->setScene(&map_scene.scene);

    qDebug() << "Scene rect: " << map_scene.scene.sceneRect();
}

ProvamiMainWindow::~ProvamiMainWindow()
{
    delete ui;
}

void ProvamiMainWindow::on_refresh_clicked()
{
    model.activate_next_filter();
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
