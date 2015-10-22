#ifndef PROVAMI_PROVAMIMAINWINDOW_H
#define PROVAMI_PROVAMIMAINWINDOW_H

#include <QMainWindow>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QDateTime>
#include <provami/model.h>
#include <provami/datagridmodel.h>
#include <provami/stationgridmodel.h>
#include <provami/attrgridmodel.h>
#include <provami/rawquerymodel.h>
#include <provami/progressindicator.h>
//#include <provami/debug.h>

namespace Ui {
class ProvamiMainWindow;
}

namespace provami {

class QOptionalIntValidator : public QIntValidator
{
public:
    QOptionalIntValidator(QObject* parent=0);
    QOptionalIntValidator(int minimum, int maximum, QObject* parent=0);
    virtual QValidator::State validate(QString & input, int& pos) const;
};

class QOptionalDoubleValidator : public QDoubleValidator
{
public:
    QOptionalDoubleValidator(QObject* parent=0);
    QOptionalDoubleValidator(double bottom, double top, int decimals, QObject* parent=0);
    virtual QValidator::State validate(QString& input, int& pos) const;
};

class ProvamiMainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    Model& model;
    //DebugAbstractTableModel<DataGridModel> datagrid_model;
    DataGridModel datagrid_model;
    StationGridModel stationgrid_model;
    AttrGridModel attrgrid_model;
    RawQueryModel rawquery_model;
    QOptionalDoubleValidator lat_validator;
    QOptionalDoubleValidator lon_validator;
    QOptionalIntValidator id_validator;
    ProgressIndicator progress_indicator;


public:
    explicit ProvamiMainWindow(Model& model, QWidget *parent = 0);
    ~ProvamiMainWindow();

private slots:
    void next_filter_changed();
    void filter_latlon_changed();
    void results_current_changed(QModelIndex, QModelIndex);
    void station_data_clicked(QModelIndex);
    void on_filter_ana_id_editingFinished();
    void filter_datemin_activated(QDateTime);
    void filter_datemax_activated(QDateTime);
    void filter_limit_changed();
    void text_query_changed();
    void on_actionExit_triggered();
    void on_actionRefresh_triggered();
    void on_actionRefreshAccurate_triggered();
    void on_actionUnselectStations_triggered();
    void stats_changed();
    void highlight_changed();
    void export_go();

private:
    Ui::ProvamiMainWindow *ui;
};

}

#endif // PROVAMIMAINWINDOW_H
