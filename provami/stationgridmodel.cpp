#include "provami/stationgridmodel.h"
#include <QDebug>

using namespace dballe;

namespace provami {

StationGridModel::StationGridModel(Model &model, QObject *parent) :
    QAbstractTableModel(parent), model(model)
{
    QObject::connect(&model.highlight, SIGNAL(changed()),
                     this, SLOT(on_highlight_changed()));
}

StationGridModel::ColumnType StationGridModel::resolveColumnType(int column) const
{
    switch (column)
    {
    case 0: return CT_NETWORK;
    case 1: return CT_VARCODE;
    case 2: return CT_VALUE;
    default: return CT_INVALID;
    }
}

int StationGridModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return values.size();
}

int StationGridModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return 3;
}

QVariant StationGridModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if ((unsigned)index.row() >= values.size()) return QVariant();
    ColumnType ctype = resolveColumnType(index.column());
    if (ctype == CT_INVALID) return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        const StationValue& val = values[index.row()];
        switch (ctype)
        {
        case CT_NETWORK: return QVariant(val.rep_memo.c_str());
        case CT_VARCODE: return QVariant(format_code(val.var.code()).c_str());
        case CT_VALUE: return QVariant(val.var.format().c_str());
        default: return QVariant();
        }
        break;
    }
    case Qt::ToolTipRole:
    case Qt::StatusTipRole:
    {
        const StationValue& val = values[index.row()];
        switch (ctype)
        {
        case CT_NETWORK: return QString("Station network: %1").arg(val.rep_memo.c_str());
        case CT_VARCODE:
        {
            return QString(val.var.info()->desc);
        }
        case CT_VALUE:
        {
            return QString("%1: %2 %3").arg(
                        QString(val.var.info()->desc),
                        QString(val.var.format().c_str()),
                        QString(val.var.info()->unit));
        }
        default: return QVariant();
        }
        break;
    }
    }
    return QVariant();
}

QVariant StationGridModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation != Qt::Horizontal)
        return QVariant();

    switch (resolveColumnType(section))
    {
    case CT_NETWORK: return QVariant("Rep");
    case CT_VARCODE: return QVariant("Var");
    case CT_VALUE: return QVariant("Value");
    default: return QVariant();
    }
}

Qt::ItemFlags StationGridModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    if ((unsigned)index.row() >= values.size()) return Qt::NoItemFlags;

    Qt::ItemFlags res = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (resolveColumnType(index.column()) == CT_VALUE)
        res |= Qt::ItemIsEditable;
    return res;
}

bool StationGridModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) return false;
    if ((unsigned)index.row() >= values.size()) return false;
    if (resolveColumnType(index.column()) != CT_VALUE) return false;

    // Skip changes on empty strings
    QString str_val = value.toString();
    if (str_val.isEmpty()) return false;
    QByteArray utf8_val = str_val.toUtf8();

    // Access the value we need to change
    StationValue& val = values[index.row()];
    wreport::Var new_var(val.var);
    try {
        new_var.set_from_formatted(utf8_val.constData());
    } catch (std::exception& e) {
        qDebug() << "Cannot set value:" << e.what();
        return false;
    }

    // TODO: update the database, and the variable in our index
    try {
        model.update(val, new_var);
    } catch (std::exception& e) {
        qDebug() << "Cannot save value in the database:" << e.what();
        return false;
    }

    emit dataChanged(index, index);

    return true;
}

const StationValue *StationGridModel::valueAt(const QModelIndex &index) const
{
    if (!index.isValid()) return NULL;
    if ((unsigned)index.row() >= values.size()) return NULL;
    const StationValue& val = values[index.row()];
    return &val;
}

void StationGridModel::on_highlight_changed()
{
    if (station_id == model.highlight.station_id())
        return;
    beginResetModel();
    station_id = model.highlight.station_id();
    values.clear();
    dballe::core::Query query;
    query.ana_id = station_id;
    auto cur = model.db->query_station_data(query);
    while (cur->next())
        values.emplace_back(*cur);
    endResetModel();
}

}
