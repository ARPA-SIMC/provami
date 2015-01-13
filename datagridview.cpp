#include "datagridview.h"
#include <QDebug>
#include <QContextMenuEvent>
#include <QMenu>
#include "model.h"
#include "datagridmodel.h"

DataGridView::DataGridView(QWidget *parent) :
    QTableView(parent)
{
    setMouseTracking(true);

}

static void build_menu(QMenu& menu, const Value& val)
{
    menu.addAction(val.var.info()->desc);
    /*
    menu.addAction(copyAct);
    menu.addAction(pasteAct);
    */
}

void DataGridView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    qDebug() << "CME" << index.row() << index.column();
    const Value* val = dynamic_cast<const DataGridModel*>(model())->valueAt(index);
    if (!val) return;

    QMenu menu(this);
    build_menu(menu, *val);
    menu.exec(event->globalPos());
}
