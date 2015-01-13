#include "datagridmodel.h"
#include <stdio.h>

DataGridModel::DataGridModel(Model& model, QObject *parent) :
    QAbstractTableModel(parent), model(model)
{
}

int DataGridModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    fprintf(stderr, "RC %d\n", (int)model.values().size());
    return model.values().size();
}

int DataGridModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return 10;
}

QVariant DataGridModel::data(const QModelIndex &index, int role) const
{
    fprintf(stderr, "HD %d %d %d\n", (int)index.isValid(), index.row(), index.column());

    if (!index.isValid()) return QVariant();
    if (index.row() >= model.values().size()) return QVariant();
    const Value& val = model.values()[index.row()];
    switch (index.column())
    {
    case 0: return QVariant(val.ana_id);
    case 1: return QVariant();
    case 2: return QVariant();
    case 3: return QVariant();
    case 4: return QVariant(val.rep_memo.c_str());
    case 5: return QVariant();
    case 6: return QVariant();
    case 7: return QVariant();
    case 8: return QVariant();
    case 9: return QVariant();
    default: return QVariant();
    }
}

QVariant DataGridModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation != Qt::Horizontal)
        return QVariant();

    switch (section)
    {
    case 0: return QVariant("St.ID");
    case 1: return QVariant("Lat");
    case 2: return QVariant("Lon");
    case 3: return QVariant("Ident");
    case 4: return QVariant("Rep");
    case 5: return QVariant("Level");
    case 6: return QVariant("Trange");
    case 7: return QVariant("Date");
    case 8: return QVariant("Var");
    case 9: return QVariant("Value");
    default: return QVariant();
    }
}
