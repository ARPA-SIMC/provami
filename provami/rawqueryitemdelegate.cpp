#include "provami/rawqueryitemdelegate.h"
#include <QComboBox>
#include <QDebug>

using namespace std;

namespace provami {

RawQueryItemDelegate::RawQueryItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    //QObject::connect(&model, SIGNAL(next_filter_changed()), this, SLOT(next_filter_changed()));
}

/*
QVariant RawQueryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if ((unsigned)index.row() >= values.size()) return QVariant();
    ColumnType ctype = resolveColumnType(index.column());
    if (ctype == CT_INVALID) return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        const rawquery::Item& item = values[index.row()];
        switch (ctype)
        {
        case CT_KEY: return QVariant(item.key.c_str());
        case CT_VALUE: return QVariant(item.val.c_str());
        default: return QVariant();
        }
        break;
    }
    case Qt::ToolTipRole:
    case Qt::StatusTipRole:
    {
        const rawquery::Item& item = values[index.row()];
        switch (ctype)
        {
        case CT_KEY:
        {
            if (item.key.empty()) return QVariant();
            try {
                wreport::Varinfo info = varinfo_by_name(item.key);
                return QString(info->desc);
            } catch (std::exception& e) {
                return QVariant(e.what());
            }
        }
        default: return QVariant();
        }
        break;
    }
    }
    return QVariant();
}

QVariant RawQueryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation != Qt::Horizontal)
        return QVariant();

    switch (resolveColumnType(section))
    {
    case CT_KEY: return QVariant("Key");
    case CT_VALUE: return QVariant("Value");
    default: return QVariant();
    }
}


Qt::ItemFlags RawQueryModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    if ((unsigned)index.row() >= values.size()) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool RawQueryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) return false;
    if ((unsigned)index.row() >= values.size()) return false;
    ColumnType ctype = resolveColumnType(index.column());
    if (ctype == CT_INVALID) return false;

    // Update values
    switch (ctype)
    {
    case CT_KEY: values[index.row()].key = value.toString().toStdString(); break;
    case CT_VALUE: values[index.row()].val = value.toString().toStdString(); break;
    }

    auto query = Query::create();
    query->set_from_record(*build_record());
    model.set_filter(*query);

    return true;
}

const rawquery::Item* RawQueryModel::valueAt(const QModelIndex &index) const
{
    if (!index.isValid()) return NULL;
    if ((unsigned)index.row() >= values.size()) return NULL;
    const rawquery::Item& item = values[index.row()];
    return &item;
}

*/

QWidget *provami::RawQueryItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // ComboBox ony in column 0
    if (index.column() != 0)
        return QStyledItemDelegate::createEditor(parent, option, index);

    // Create the combobox and populate it
    QComboBox* cb = new QComboBox(parent);
    int row = index.row();
    cb->addItem("ana_id");
    cb->addItem("priority");
    cb->addItem("priomin");
    cb->addItem("priomax");
    cb->addItem("rep_memo");
    cb->addItem("mobile");
    cb->addItem("ident");
    cb->addItem("lat");
    cb->addItem("latmin");
    cb->addItem("latmax");
    cb->addItem("lon");
    cb->addItem("lonmin");
    cb->addItem("lonmax");
    cb->addItem("year");
    cb->addItem("month");
    cb->addItem("day");
    cb->addItem("hour");
    cb->addItem("min");
    cb->addItem("sec");
    cb->addItem("yearmin");
    cb->addItem("monthmin");
    cb->addItem("daymin");
    cb->addItem("hourmin");
    cb->addItem("minumin");
    cb->addItem("secmin");
    cb->addItem("yearmax");
    cb->addItem("monthmax");
    cb->addItem("daymax");
    cb->addItem("hourmax");
    cb->addItem("minumax");
    cb->addItem("secmax");
    cb->addItem("leveltype1");
    cb->addItem("l1");
    cb->addItem("leveltype2");
    cb->addItem("l2");
    cb->addItem("pindicator");
    cb->addItem("p1");
    cb->addItem("p2");
    cb->addItem("var");
    cb->addItem("varlist");
    cb->addItem("query");
    cb->addItem("ana_filter");
    cb->addItem("data_filter");
    cb->addItem("attr_filter");
    cb->addItem("limit");
    cb->addItem("block");
    cb->addItem("station");
    cb->addItem("context_id");
    return cb;
}

void provami::RawQueryItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (QComboBox* cb = qobject_cast<QComboBox*>(editor))
    {
       // get the index of the text in the combobox that matches the current value of the itenm
       QString currentText = index.data(Qt::EditRole).toString();
       int cbIndex = cb->findText(currentText);
       // if it is valid, adjust the combobox
       if (cbIndex >= 0)
           cb->setCurrentIndex(cbIndex);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void provami::RawQueryItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (QComboBox* cb = qobject_cast<QComboBox*>(editor))
    {
        // save the current text of the combo box as the current value of the item
        model->setData(index, cb->currentText(), Qt::EditRole);
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);

}


}
