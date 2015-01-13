#include "provami/attrgridmodel.h"
#include <dballe/core/var.h>
#include <QDebug>

using namespace dballe;

namespace provami {

AttrGridModel::AttrGridModel(Model &model, QObject *parent) :
    QAbstractTableModel(parent), model(model)
{
    QObject::connect(&model.highlight, SIGNAL(changed()),
                     this, SLOT(on_highlight_changed()));
}

AttrGridModel::ColumnType AttrGridModel::resolveColumnType(int column) const
{
    switch (column)
    {
    case 0: return CT_VARCODE;
    case 1: return CT_VALUE;
    default: return CT_INVALID;
    }
}

int AttrGridModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return values.size();
}

int AttrGridModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return 2;
}

QVariant AttrGridModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if ((unsigned)index.row() >= values.size()) return QVariant();
    ColumnType ctype = resolveColumnType(index.column());
    if (ctype == CT_INVALID) return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        const wreport::Var& var = values[index.row()];
        switch (ctype)
        {
        case CT_VARCODE: return QVariant(format_code(var.code()).c_str());
        case CT_VALUE: return QVariant(var.format().c_str());
        default: return QVariant();
        }
        break;
    }
    case Qt::ToolTipRole:
    case Qt::StatusTipRole:
    {
        const wreport::Var& var = values[index.row()];
        switch (ctype)
        {
        case CT_VARCODE: return QString(var.info()->desc);
        case CT_VALUE:
        {
            return QString("%1: %2 %3").arg(
                        QString(var.info()->desc),
                        QString(var.format().c_str()),
                        QString(var.info()->unit));
        }
        default: return QVariant();
        }
        break;
    }
    }
    return QVariant();
}

QVariant AttrGridModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation != Qt::Horizontal)
        return QVariant();

    switch (resolveColumnType(section))
    {
    case CT_VARCODE: return QVariant("Var");
    case CT_VALUE: return QVariant("Value");
    default: return QVariant();
    }
}

Qt::ItemFlags AttrGridModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    if ((unsigned)index.row() >= values.size()) return Qt::NoItemFlags;

    Qt::ItemFlags res = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (resolveColumnType(index.column()) == CT_VALUE)
        res |= Qt::ItemIsEditable;
    return res;
}

bool AttrGridModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) return false;
    if ((unsigned)index.row() >= values.size()) return false;
    if (resolveColumnType(index.column()) != CT_VALUE) return false;

    // Skip changes on empty strings
    QString str_val = value.toString();
    if (str_val.isEmpty()) return false;
    QByteArray utf8_val = str_val.toUtf8();

    // Access the value we need to change
    wreport::Var& var = values[index.row()];
    wreport::Var new_attr(var);
    try {
        new_attr.set_from_formatted(utf8_val.constData());
    } catch (std::exception& e) {
        qDebug() << "Cannot set value:" << e.what();
        return false;
    }
/* TODO: update
    try {
        model.update(val, new_var);
    } catch (std::exception& e) {
        qDebug() << "Cannot save value in the database:" << e.what();
        return false;
    }
*/
    emit dataChanged(index, index);

    return true;
}

const wreport::Var *AttrGridModel::valueAt(const QModelIndex &index) const
{
    if (!index.isValid()) return NULL;
    if ((unsigned)index.row() >= values.size()) return NULL;
    const wreport::Var& var = values[index.row()];
    return &var;
}

void AttrGridModel::on_highlight_changed()
{
    /* TODO: extract varid and varcode from model.highlight and query stuff
    if (station_id == model.highlight.station_id())
        return;
    station_id = model.highlight.station_id();
    values.clear();
    dballe::Record rec;
    rec.set(DBA_KEY_ANA_ID, station_id);
    rec.set_ana_context();
    auto cur = model.db->query_data(rec);
    while (cur->next())
        values.emplace_back(*cur);
        */
    reset();
}

}
