#include "provamimainwindow.h"
#include "ui_provamimainwindow.h"
#include <set>
#include <map>
#include <dballe/core/defs.h>
#include <dballe/core/var.h>
#include <dballe/core/record.h>
#include <QDebug>
#include "model.h"
#include "mapscene.h"

using namespace std;
using namespace dballe;


ProvamiMainWindow::ProvamiMainWindow(Model& model, QWidget *parent) :
    QMainWindow(parent),
    model(model), datagrid_model(model), map_scene(model),
    ui(new Ui::ProvamiMainWindow)
{
    ui->setupUi(this);
    connect(&model, SIGNAL(next_filter_changed()), this, SLOT(on_next_filter_changed()));

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

static void set_field_with_record(QLineEdit* field, Record& rec, dba_keyword key)
{
    if (rec.contains(key))
    {
        string val = rec.get(key).format();
        field->setText(QString::fromStdString(val));
    }
    else
        field->setText("");
}

void ProvamiMainWindow::on_next_filter_changed()
{
    set_field_with_record(ui->filter_latmin, model.next_filter, DBA_KEY_LATMIN);
    set_field_with_record(ui->filter_latmax, model.next_filter, DBA_KEY_LATMAX);
    set_field_with_record(ui->filter_lonmin, model.next_filter, DBA_KEY_LONMIN);
    set_field_with_record(ui->filter_lonmax, model.next_filter, DBA_KEY_LONMAX);
    set_field_with_record(ui->filter_ana_id, model.next_filter, DBA_KEY_ANA_ID);
}
