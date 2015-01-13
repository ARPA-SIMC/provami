#include "datagridmodel.h"

DataGridModel::DataGridModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int DataGridModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    // TODO
    return 5;
}

int DataGridModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    // TODO
    return 5;
}

QVariant DataGridModel::data(const QModelIndex &index, int role) const
{
    return QVariant(0);
}
