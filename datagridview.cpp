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

struct SelectNetworkAction : public ModelAction
{
    const Value& val;

    SelectNetworkAction(Model& model, const Value& val, QObject* parent=0)
        : ModelAction(model, parent), val(val)
    {
        setIconText(QString("Select network %1").arg(val.rep_memo.c_str()));
    }

    void on_trigger()
    {
        model.select_report(val.rep_memo);
    }
};

struct SelectLevelAction : public ModelAction
{
    const Value& val;

    SelectLevelAction(Model& model, const Value& val, QObject* parent=0)
        : ModelAction(model, parent), val(val)
    {
        setIconText(QString("Select level %1").arg(val.level.describe().c_str()));
    }

    void on_trigger()
    {
        model.select_level(val.level);
    }
};

struct SelectTrangeAction : public ModelAction
{
    const Value& val;

    SelectTrangeAction(Model& model, const Value& val, QObject* parent=0)
        : ModelAction(model, parent), val(val)
    {
        setIconText(QString("Select time range %1").arg(val.trange.describe().c_str()));
    }

    void on_trigger()
    {
        model.select_trange(val.trange);
    }
};


struct SelectVarcodeAction : public ModelAction
{
    const Value& val;

    SelectVarcodeAction(Model& model, const Value& val, QObject* parent=0)
        : ModelAction(model, parent), val(val)
    {
        setIconText(QString("Select variable %1").arg(val.var.info()->desc));
    }

    void on_trigger()
    {
        model.select_varcode(val.var.code());
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
        menu.addAction(new SelectStationIDAction(model, val, &menu));
        break;
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

    //menu.addAction(val.var.info()->desc);
    /*
    menu.addAction(copyAct);
    menu.addAction(pasteAct);
    */
}

void DataGridView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    qDebug() << "CME" << index.row() << index.column();
    DataGridModel* m = dynamic_cast<DataGridModel*>(model());
    if (!m) return;

    DataGridModel::ColumnType ctype = m->resolveColumnType(index.column());
    const Value* val = m->valueAt(index);
    if (!val) return;

    QMenu menu(this);
    build_menu(menu, m->dataModel(), ctype, *val);
    menu.exec(event->globalPos());
}
