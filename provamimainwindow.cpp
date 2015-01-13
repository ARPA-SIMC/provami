#include "provamimainwindow.h"
#include "ui_provamimainwindow.h"
#include <set>
#include <map>
#include <limits>
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
    lat_validator(-90, 90, 5),
    lon_validator(-180, 180, 5),
    id_validator(0, std::numeric_limits<int>::max()),
    ui(new Ui::ProvamiMainWindow)
{
    ui->setupUi(this);
    connect(&model, SIGNAL(next_filter_changed()), this, SLOT(next_filter_changed()));
    connect(ui->filter_latmin, SIGNAL(editingFinished()), this, SLOT(filter_latlon_changed()));
    connect(ui->filter_latmax, SIGNAL(editingFinished()), this, SLOT(filter_latlon_changed()));
    connect(ui->filter_lonmin, SIGNAL(editingFinished()), this, SLOT(filter_latlon_changed()));
    connect(ui->filter_lonmax, SIGNAL(editingFinished()), this, SLOT(filter_latlon_changed()));

    ui->results->setModel(&datagrid_model);
    ui->filter_report->setModel(&model.reports);
    ui->filter_level->setModel(&model.levels);
    ui->filter_trange->setModel(&model.tranges);
    ui->filter_varcode->setModel(&model.varcodes);
    ui->filter_latmin->setValidator(&lat_validator);
    ui->filter_latmax->setValidator(&lat_validator);
    ui->filter_lonmin->setValidator(&lon_validator);
    ui->filter_lonmax->setValidator(&lon_validator);
    ui->filter_ana_id->setValidator(&id_validator);
    ui->filter_latmin->set_record(model.next_filter, DBA_KEY_LATMIN);
    ui->filter_latmax->set_record(model.next_filter, DBA_KEY_LATMAX);
    ui->filter_lonmin->set_record(model.next_filter, DBA_KEY_LONMIN);
    ui->filter_lonmax->set_record(model.next_filter, DBA_KEY_LONMAX);
    ui->filter_ana_id->set_record(model.next_filter, DBA_KEY_ANA_ID);

    map_scene.load_coastlines("/home/enrico/lavori/arpa/provami/world.dat");
    ui->mapview->setScene(&map_scene.scene);

    qDebug() << "Scene rect: " << map_scene.scene.sceneRect();

    statusBar()->showMessage("Antani");
}

ProvamiMainWindow::~ProvamiMainWindow()
{
    delete ui;
}

void ProvamiMainWindow::on_refresh_clicked()
{
    model.activate_next_filter();
}

void ProvamiMainWindow::next_filter_changed()
{
    ui->filter_latmin->reset();
    ui->filter_latmax->reset();
    ui->filter_lonmin->reset();
    ui->filter_lonmax->reset();
    ui->filter_ana_id->reset();

}

void ProvamiMainWindow::filter_latlon_changed()
{
    QString slatmin = ui->filter_latmin->text();
    QString slatmax = ui->filter_latmax->text();
    QString slonmin = ui->filter_lonmin->text();
    QString slonmax = ui->filter_lonmax->text();
    bool ok;

    double latmin = slatmin.toDouble(&ok);
    if (!ok) latmin = -90;
    double latmax = slatmax.toDouble(&ok);
    if (!ok) latmax = 90;
    double lonmin = slonmin.toDouble(&ok);
    if (!ok) lonmin = -180;
    double lonmax = slonmax.toDouble(&ok);
    if (!ok) lonmax = 180;

    if (latmin == -90 && latmax == 90 && lonmin == -180 && lonmax == 180)
        model.unselect_station();
    else
        model.select_station_bounds(latmin, latmax, lonmin, lonmax);
}

void ProvamiMainWindow::on_filter_ana_id_editingFinished()
{
    QString sid = ui->filter_ana_id->text();
    bool ok;
    int id = sid.toInt(&ok);
    if (ok)
        model.select_station_id(id);
    else
        model.unselect_station();
}


QOptionalIntValidator::QOptionalIntValidator(QObject *parent)
    : QIntValidator(parent)
{
}

QOptionalIntValidator::QOptionalIntValidator(int minimum, int maximum, QObject *parent)
    : QIntValidator(minimum, maximum, parent)
{
}

QValidator::State QOptionalIntValidator::validate(QString &input, int &pos) const
{
    if (input.isEmpty()) return Acceptable;
    return QIntValidator::validate(input, pos);
}


QOptionalDoubleValidator::QOptionalDoubleValidator(QObject *parent)
    : QDoubleValidator(parent)
{
}

QOptionalDoubleValidator::QOptionalDoubleValidator(double bottom, double top, int decimals, QObject *parent)
    : QDoubleValidator(bottom, top, decimals, parent)
{
}

QValidator::State QOptionalDoubleValidator::validate(QString &input, int &pos) const
{
    if (input.isEmpty()) return Acceptable;
    return QDoubleValidator::validate(input, pos);
}
