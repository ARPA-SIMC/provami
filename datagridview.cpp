#include "datagridview.h"
#include <QDebug>
#include <QContextMenuEvent>
#include <QMenu>
#include "model.h"
#include "datagridmodel.h"

struct SelectStationIDAction : public ModelAction
{
    const Value& val;

    SelectStationIDAction(Model& model, const Value& val, QObject* parent=0)
        : ModelAction(model, parent), val(val)
    {
        const Station* sta = model.station(val.ana_id);
        if (!sta)
            setIconText(QString("Select station %1").arg(val.ana_id));
        else if (sta->ident.empty())
            setIconText(QString("Select fixed station %1, lat %1 lon %2")
                    .arg(val.ana_id).arg(sta->lat).arg(sta->lon));
        else
            setIconText(QString("Select mobile station %1 at lat %2 lon %3")
                    .arg(sta->ident.c_str()).arg(sta->lat).arg(sta->lon));
    }

    void on_trigger()
    {
        model.select_station_id(val.ana_id);
    }
};

struct SelectIdentAction : public ModelAction
{
    const Value& val;

    SelectIdentAction(Model& model, const Value& val, QObject* parent=0)
        : ModelAction(model, parent), val(val)
    {
        const Station* sta = model.station(val.ana_id);
        setIconText(QString("Select mobile station %1").arg(sta->ident.c_str()));
        setEnabled(model.idents.has_item(sta->ident));
    }

    void on_trigger()
    {
        const Station* sta = model.station(val.ana_id);
        model.idents.select(sta->ident);
    }
};

struct SelectNetworkAction : public ModelAction
{
    const Value& val;

    SelectNetworkAction(Model& model, const Value& val, QObject* parent=0)
        : ModelAction(model, parent), val(val)
    {
        setIconText(QString("Select network %1").arg(val.rep_memo.c_str()));
        setEnabled(model.reports.has_item(val.rep_memo));
    }

    void on_trigger()
    {
        model.reports.select(val.rep_memo);
    }
};

struct SelectLevelAction : public ModelAction
{
    const Value& val;

    SelectLevelAction(Model& model, const Value& val, QObject* parent=0)
        : ModelAction(model, parent), val(val)
    {
        setIconText(QString("Select level %1").arg(val.level.describe().c_str()));
        setEnabled(model.levels.has_item(val.level));
    }

    void on_trigger()
    {
        model.levels.select(val.level);
    }
};

struct SelectTrangeAction : public ModelAction
{
    const Value& val;

    SelectTrangeAction(Model& model, const Value& val, QObject* parent=0)
        : ModelAction(model, parent), val(val)
    {
        setIconText(QString("Select time range %1").arg(val.trange.describe().c_str()));
        setEnabled(model.tranges.has_item(val.trange));
    }

    void on_trigger()
    {
        model.tranges.select(val.trange);
    }
};


struct SelectVarcodeAction : public ModelAction
{
    const Value& val;

    SelectVarcodeAction(Model& model, const Value& val, QObject* parent=0)
        : ModelAction(model, parent), val(val)
    {
        setIconText(QString("Select variable %1").arg(val.var.info()->desc));
        setEnabled(model.varcodes.has_item(val.var.code()));
    }

    void on_trigger()
    {
        model.varcodes.select(val.var.code());
    }
};


struct DeleteRecordAction : public ModelAction
{
    const Value& val;

    DeleteRecordAction(Model& model, const Value& val, QObject* parent=0)
        : ModelAction(model, parent), val(val)
    {
        setIconText("Delete record");
    }

    void on_trigger()
    {
        model.remove(val);
    }
};

DataGridView::DataGridView(QWidget *parent) :
    QTableView(parent)
{
    setMouseTracking(true);

}

static void build_menu(QMenu& menu, Model& model, DataGridModel::ColumnType ctype, const Value& val)
{
    switch(ctype)
    {
    case DataGridModel::CT_STATION:
    {
        const Station* sta = model.station(val.ana_id);
        menu.addAction(new SelectStationIDAction(model, val, &menu));
        if (!sta->ident.empty())
        {
            menu.addAction(new SelectIdentAction(model, val, &menu));
        }
        break;
    }
    case DataGridModel::CT_NETWORK:
        menu.addAction(new SelectNetworkAction(model, val, &menu));
        break;
    case DataGridModel::CT_LEVEL:
        menu.addAction(new SelectLevelAction(model, val, &menu));
        break;
    case DataGridModel::CT_TRANGE:
        menu.addAction(new SelectTrangeAction(model, val, &menu));
        break;
    case DataGridModel::CT_DATETIME:
        break;
    case DataGridModel::CT_VARCODE:
        menu.addAction(new SelectVarcodeAction(model, val, &menu));
        break;
    case DataGridModel::CT_VALUE:
        break;
    case DataGridModel::CT_INVALID:
        break;
    }
    menu.addAction(new DeleteRecordAction(model, val, &menu));

    //menu.addAction(val.var.info()->desc);
    /*
    menu.addAction(copyAct);
    menu.addAction(pasteAct);
    */
}

void DataGridView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    DataGridModel* m = dynamic_cast<DataGridModel*>(model());
    if (!m) return;

    DataGridModel::ColumnType ctype = m->resolveColumnType(index.column());
    const Value* val = m->valueAt(index);
    if (!val) return;

    QMenu menu(this);
    build_menu(menu, m->dataModel(), ctype, *val);
    menu.exec(event->globalPos());
}
