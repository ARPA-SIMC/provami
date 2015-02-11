#include "provami/provamimainwindow.h"
#include "ui_provamimainwindow.h"
#include <set>
#include <map>
#include <limits>
#include <sstream>
#include <dballe/core/defs.h>
#include <dballe/core/var.h>
#include <dballe/core/record.h>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <QDebug>
#include "provami/model.h"
#include "provami/mapscene.h"

using namespace std;
using namespace dballe;

namespace provami {

ProvamiMainWindow::ProvamiMainWindow(Model& model, QWidget *parent) :
    QMainWindow(parent),
    model(model), datagrid_model(model), stationgrid_model(model), attrgrid_model(model),
    rawquery_model(model), map_scene(model),
    lat_validator(-90, 90, 5),
    lon_validator(-180, 180, 5),
    id_validator(0, std::numeric_limits<int>::max()),
    ui(new Ui::ProvamiMainWindow)
{
    ui->setupUi(this);
    connect(&model, SIGNAL(next_filter_changed()), this, SLOT(next_filter_changed()));
    connect(&model.highlight, SIGNAL(changed()), this, SLOT(highlight_changed()));
    connect(ui->filter_latmin, SIGNAL(editingFinished()), this, SLOT(filter_latlon_changed()));
    connect(ui->filter_latmax, SIGNAL(editingFinished()), this, SLOT(filter_latlon_changed()));
    connect(ui->filter_lonmin, SIGNAL(editingFinished()), this, SLOT(filter_latlon_changed()));
    connect(ui->filter_lonmax, SIGNAL(editingFinished()), this, SLOT(filter_latlon_changed()));
    connect(ui->filter_datemin, SIGNAL(activate(QDateTime)), this, SLOT(filter_datemin_activated(QDateTime)));
    connect(ui->filter_datemax, SIGNAL(activate(QDateTime)), this, SLOT(filter_datemax_activated(QDateTime)));
    connect(ui->filter_limit, SIGNAL(editingFinished()), this, SLOT(filter_limit_changed()));
    //connect(ui->text_query, SIGNAL(textChanged()), this, SLOT(text_query_changed());
    connect(ui->results, SIGNAL(clicked(QModelIndex)), this, SLOT(results_clicked(QModelIndex)));
    connect(ui->station_data, SIGNAL(clicked(QModelIndex)), this, SLOT(station_data_clicked(QModelIndex)));
    connect(ui->export_go, SIGNAL(clicked()), this, SLOT(export_go()));
    connect(&model, SIGNAL(active_filter_changed()), this, SLOT(on_stats_changed()));

    ui->results->setModel(&datagrid_model);
    ui->station_data->setModel(&stationgrid_model);
    ui->attr_data->setModel(&attrgrid_model);
    ui->raw_query->setModel(&rawquery_model);
    ui->filter_report->setModel(&model.reports);
    ui->filter_level->setModel(&model.levels);
    ui->filter_trange->setModel(&model.tranges);
    ui->filter_varcode->setModel(&model.varcodes);
    ui->filter_ident->setModel(&model.idents);
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
    ui->filter_datemin->set_model(model);
    ui->filter_datemax->set_model(model);
    ui->filter_limit->setText(QString::number(model.limit));

    map_scene.load_coastlines("/usr/share/provami/world.dat");
    ui->mapview->setScene(&map_scene.scene);
    //qDebug() << "Scene rect: " << map_scene.scene.sceneRect();

    ui->export_format->addItem("BUFR", "bufr");
    ui->export_format->addItem("BUFR (generic)", "gbufr");
    ui->export_format->addItem("CREX", "crex");
    ui->export_format->addItem("CREX (generic)", "gcrex");
    ui->export_format->addItem("Gnu R", "gnur");
    ui->export_format->addItem("CSV", "csv");

    //statusBar()->showMessage("Antani");
}

ProvamiMainWindow::~ProvamiMainWindow()
{
    delete ui;
}

void ProvamiMainWindow::next_filter_changed()
{
    ui->filter_latmin->reset();
    ui->filter_latmax->reset();
    ui->filter_lonmin->reset();
    ui->filter_lonmax->reset();
    ui->filter_ana_id->reset();
    ui->filter_datemin->reset();
    ui->filter_datemax->reset();
    ui->raw_query_shell->setText(rawquery_model.as_shell_args().join(" "));
}

void ProvamiMainWindow::results_clicked(QModelIndex idx)
{
    const Value* val = datagrid_model.valueAt(idx);
    model.highlight.select_value(val);
}

void ProvamiMainWindow::station_data_clicked(QModelIndex idx)
{
    const StationValue* val = stationgrid_model.valueAt(idx);
    model.highlight.select_station_value(val);
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

void ProvamiMainWindow::filter_datemin_activated(QDateTime dt)
{
    if (dt.isNull())
        model.unselect_datemin();
    else
        model.select_datemin(dballe::Datetime(
            dt.date().year(), dt.date().month(), dt.date().day(),
            dt.time().hour(), dt.time().minute(), dt.time().second()));
}

void ProvamiMainWindow::filter_datemax_activated(QDateTime dt)
{
    if (dt.isNull())
        model.unselect_datemax();
    else
        model.select_datemax(dballe::Datetime(
            dt.date().year(), dt.date().month(), dt.date().day(),
                                 dt.time().hour(), dt.time().minute(), dt.time().second()));
}

void ProvamiMainWindow::filter_limit_changed()
{
    model.limit = ui->filter_limit->text().toUInt();
}

void ProvamiMainWindow::text_query_changed()
{
    // TODO: argparse
    // TODO: set_from_string
    // if ok: set filter
    // if not ok: mark background as red
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

void ProvamiMainWindow::on_actionExit_triggered()
{
    close();
}

void ProvamiMainWindow::on_actionRefresh_triggered()
{
    model.activate_next_filter();
}

void ProvamiMainWindow::on_actionRefreshAccurate_triggered()
{
    model.activate_next_filter(true);
}

void ProvamiMainWindow::on_stats_changed()
{
    stringstream buf;
    buf << model.dtmin << " to " << model.dtmax << ": " << model.count;
    ui->filter_summary->setText(QString::fromStdString(buf.str()));
}

void ProvamiMainWindow::highlight_changed()
{
    const Station* station = model.station(model.highlight.station_id());
    if (station)
    {
        ui->cur_st_lat->setText(QString("%1").arg(station->lat, 0, 'f', 5));
        ui->cur_st_lon->setText(QString("%1").arg(station->lon, 0, 'f', 5));
        ui->cur_st_id->setText(QString("%1").arg(station->id));
        if (station->ident.empty())
            ui->cur_st_name->setText("(fixed station)");
        else
            ui->cur_st_name->setText(station->ident.c_str());
    } else {
        ui->cur_st_lat->setText("-");
        ui->cur_st_lon->setText("-");
        ui->cur_st_id->setText("-");
        ui->cur_st_name->setText("-");
    }
    const wreport::Var* var = model.highlight.variable();
    if (var)
    {
        string formatted = wreport::varcode_format(var->code());
        formatted += " ";
        formatted += var->info()->desc;
        ui->attr_var->setText(formatted.c_str());
    } else {
        ui->attr_var->setText("-");
    }
}

void ProvamiMainWindow::export_go()
{
    int fmt_idx = ui->export_format->currentIndex();
    if (fmt_idx == -1) return;

    QString format = ui->export_format->itemData(fmt_idx).toString();

    QString fileName = QFileDialog::getSaveFileName(this, "Export data");
    if (fileName.isEmpty()) return;

    qDebug() << "Export " << format << " " << fileName;

    QStringList args;
    args << "--dsn" << model.dballe_url().c_str();
    args << "--outfile" << fileName;
    if (format == "gbufr")
        args << "bufr" << "--generic";
    else if (format == "gcrex")
        args << "crex" << "--generic";
    else
        args << format;
    args += rawquery_model.as_shell_args(false);

    qDebug() << "Running dbaexport" << args;

    int res = QProcess::execute("dbaexport", args);
    if (res != 0)
        QMessageBox::warning(this, "Export failed", "Export failed: dbaexport returned an error code.");
    else
        qDebug() << "dbaexport ran successfully";
}

}
