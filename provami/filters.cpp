#include "provami/filters.h"
#include "provami/model.h"

using namespace dballe;
using namespace std;

namespace provami {

FilterModelQObjectBase::FilterModelQObjectBase(Model& model, QObject *parent)
    : QAbstractListModel(parent), model(model)
{
}

template<typename ITEM>
FilterModelBase<ITEM>::FilterModelBase(Model& model, QObject *parent)
    : FilterModelQObjectBase(model, parent)
{
}

template<typename ITEM>
ITEM FilterModelBase<ITEM>::from_model() const
{
    return from_record(model.next_filter);
}

template<typename ITEM>
ITEM FilterModelBase<ITEM>::active_from_model() const
{
    return from_record(model.active_filter);
}

template<typename ITEM>
int FilterModelBase<ITEM>::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return items.size() + 1;
}

template<typename ITEM>
QVariant FilterModelBase<ITEM>::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    int idx = index.row();

    switch (role)
    {
    case Qt::DisplayRole:
        if (idx == 0) return "(none)";
        --idx;
        if (idx >= (signed)items.size()) return QVariant();

        return item_to_table_cell(items[idx]);
    case Qt::FontRole:
        if (idx == 0)
            return QVariant();
        else if (active_from_model() == items[idx-1])
        {
            QFont res;
            res.setBold(true);
            return res;
        } else
            return QVariant();
    default:
        return QVariant();
    }
}

template<typename ITEM>
void FilterModelBase<ITEM>::select(const ITEM &item)
{
    if (!has_item(item)) return;
    filter_select(item);
}

template<typename ITEM>
bool FilterModelBase<ITEM>::has_item(const ITEM &item)
{
    typename vector<ITEM>::const_iterator i = std::find(items.begin(), items.end(), item);
    return i != items.end();
}

template<typename ITEM>
void FilterModelBase<ITEM>::set_items(std::set<ITEM> &new_items)
{
    emit layoutAboutToBeChanged();
    QModelIndexList pil = persistentIndexList();
    vector<ITEM> old = items;
    items.clear();
    std::copy(new_items.begin(), new_items.end(), back_inserter(items));
    foreach (QModelIndex pi, pil)
    {
        int row = pi.row();

        // If it was pointing to (none), it doesn't change
        if (row == 0) continue;
        --row;

        ITEM oitem = old[row];
        typename vector<ITEM>::const_iterator i = std::find(items.begin(), items.end(), oitem);
        if (i == items.end())
        {
            changePersistentIndex(pi, QModelIndex());
        } else {
            int new_pos = i - items.begin() + 1;
            changePersistentIndex(pi, index(new_pos));
        }
    }
    emit layoutChanged();
}

template<typename ITEM>
void FilterModelBase<ITEM>::set_next_filter(int index)
{
    if (index <= 0)
    {
        filter_unselect();
        return;
    }

    --index;
    if (index > (signed)items.size())
    {
        filter_unselect();
        return;
    }
    filter_select(items[index]);
}

template<typename ITEM>
int FilterModelBase<ITEM>::get_current_index_from_model()
{
    ITEM selected = from_model();
    typename vector<ITEM>::const_iterator i = std::find(items.begin(), items.end(), selected);
    if (i == items.end())
        return 0;
    int pos = i - items.begin();
    return pos + 1;
}



FilterReportModel::FilterReportModel(Model &model, QObject *parent)
    : FilterModelBase<std::string>(model, parent)
{
}
std::string FilterReportModel::from_record(const dballe::Query& rec) const
{
    return rec.rep_memo;
}
void FilterReportModel::filter_select(const string &val) { model.select_report(val); }
void FilterReportModel::filter_unselect() { model.unselect_report(); }
QVariant FilterReportModel::item_to_table_cell(const std::string& val) const { return QVariant(val.c_str()); }


FilterLevelModel::FilterLevelModel(Model &model, QObject *parent)
    : FilterModelBase<dballe::Level>(model, parent)
{
}
Level FilterLevelModel::from_record(const dballe::Query& rec) const
{
    return rec.level;
}
void FilterLevelModel::filter_select(const Level &val) { model.select_level(val); }
void FilterLevelModel::filter_unselect() { model.unselect_level(); }
QVariant FilterLevelModel::item_to_table_cell(const Level& val) const
{
    return QVariant(val.describe().c_str());
}


FilterTrangeModel::FilterTrangeModel(Model &model, QObject *parent)
    : FilterModelBase<dballe::Trange>(model, parent)
{
}
Trange FilterTrangeModel::from_record(const dballe::Query& rec) const
{
    return rec.trange;
}
void FilterTrangeModel::filter_select(const Trange &val) { model.select_trange(val); }
void FilterTrangeModel::filter_unselect() { model.unselect_trange(); }
QVariant FilterTrangeModel::item_to_table_cell(const Trange& val) const
{
    return QVariant(val.describe().c_str());
}

FilterVarcodeModel::FilterVarcodeModel(Model &model, QObject *parent)
    : FilterModelBase<wreport::Varcode>(model, parent)
{
}
wreport::Varcode FilterVarcodeModel::from_record(const dballe::Query& rec) const
{
    if (rec.varcodes.empty())
        return 0;
    else
        return *rec.varcodes.begin();
}
void FilterVarcodeModel::filter_select(const wreport::Varcode &val) { model.select_varcode(val); }
void FilterVarcodeModel::filter_unselect() { model.unselect_varcode(); }
QVariant FilterVarcodeModel::item_to_table_cell(const wreport::Varcode& val) const
{
    string desc = format_code(val);
    try {
        wreport::Varinfo info = varinfo(val);
        desc += ": ";
        desc += info->desc;
    } catch (wreport::error_notfound) {
    }
    return QVariant(desc.c_str());
}

FilterIdentModel::FilterIdentModel(Model &model, QObject *parent)
    : FilterModelBase<std::string>(model, parent)
{
}
string FilterIdentModel::from_record(const dballe::Query& rec) const
{
    if (rec.has_ident)
        return rec.ident;
    else
        // FIXME: it could be possible that a station has an empty string as
        // ident. To support that, we need to return a QString instead of a
        // std::string, since QString allows a null value different from the
        // empty string
        return string();
}
void FilterIdentModel::filter_select(const string &val) { model.select_ident(val); }
void FilterIdentModel::filter_unselect() { model.unselect_ident(); }
QVariant FilterIdentModel::item_to_table_cell(const std::string& val) const { return QVariant(val.c_str()); }


template class FilterModelBase<std::string>;
template class FilterModelBase<Level>;
template class FilterModelBase<Trange>;
template class FilterModelBase<wreport::Varcode>;

}
