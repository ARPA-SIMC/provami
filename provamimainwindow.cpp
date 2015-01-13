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
    model(model),
    ui(new Ui::ProvamiMainWindow)
{
    ui->setupUi(this);

    QObject::connect(&model, SIGNAL(refreshed()),
                     this, SLOT(on_model_refreshed()));
}

ProvamiMainWindow::~ProvamiMainWindow()
{
    delete ui;
}

void ProvamiMainWindow::on_refresh_clicked()
{
    model.refresh();
}

void ProvamiMainWindow::on_model_refreshed()
{
    using namespace dballe;

    fprintf(stderr, "PMW MODEL REF\n");

    set<string> reports;
    set<Level> levels;
    set<Trange> tranges;
    set<wreport::Varcode> vars;

    const map<SummaryKey, SummaryValue>& summary = model.summary();

    for (map<SummaryKey, SummaryValue>::const_iterator i = summary.begin();
         i != summary.end(); ++i)
    {
        reports.insert(i->first.rep_memo);
        levels.insert(i->first.level);
        tranges.insert(i->first.trange);
        vars.insert(i->first.var);
    }

    ui->filter_report->clear();
    for (set<string>::const_iterator i = reports.begin(); i != reports.end(); ++i)
        ui->filter_report->addItem(QString::fromStdString(*i));

    ui->filter_level->clear();
    for (set<Level>::const_iterator i = levels.begin(); i != levels.end(); ++i)
        ui->filter_level->addItem(QString::fromStdString(i->describe()));

    ui->filter_trange->clear();
    for (set<Trange>::const_iterator i = tranges.begin(); i != tranges.end(); ++i)
        ui->filter_trange->addItem(QString::fromStdString(i->describe()));

    ui->filter_var->clear();
    for (set<wreport::Varcode>::const_iterator i = vars.begin(); i != vars.end(); ++i)
    {
        try {
            wreport::Varinfo info = varinfo(*i);
            ui->filter_var->addItem(QString(info->desc));
        } catch (wreport::error_notfound) {
            ui->filter_var->addItem(QString::fromStdString(format_code(*i)));
        }
    }
}
