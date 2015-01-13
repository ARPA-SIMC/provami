#include "model.h"
#include <memory>
#include <dballe/core/record.h>
#include <dballe/db/db.h>
#include <set>
#include <algorithm>
#include <QDebug>

using namespace std;
using namespace dballe;

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
int FilterModelBase<ITEM>::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return items.size() + 1;
}

template<typename ITEM>
QVariant FilterModelBase<ITEM>::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (!index.isValid()) return QVariant();
    int idx = index.row();
    if (idx == 0) return "(none)";
    --idx;
    if (idx >= (signed)items.size()) return QVariant();

    return item_to_table_cell(items[idx]);
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
string FilterReportModel::from_model()
{
    return model.next_filter.get(DBA_KEY_REP_MEMO, "");
}
void FilterReportModel::filter_select(const string &val) { model.select_report(val); }
void FilterReportModel::filter_unselect() { model.unselect_report(); }
QVariant FilterReportModel::item_to_table_cell(const std::string& val) const { return QVariant(val.c_str()); }


FilterLevelModel::FilterLevelModel(Model &model, QObject *parent)
    : FilterModelBase<dballe::Level>(model, parent)
{
}
Level FilterLevelModel::from_model()
{
    return model.next_filter.get_level();
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
Trange FilterTrangeModel::from_model()
{
    return model.next_filter.get_trange();
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
wreport::Varcode FilterVarcodeModel::from_model()
{
    const char* scode = model.next_filter.get(DBA_KEY_VAR, "");
    return wreport::descriptor_code(scode);
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


Model::Model()
    : db(0), reports(*this), levels(*this), tranges(*this), varcodes(*this)
{
}

Model::~Model()
{
    if (db) delete db;
}

const std::map<int, Station> &Model::stations() const
{
    return cache_stations;
}

const Station *Model::station(int id) const
{
    std::map<int, Station>::const_iterator i = cache_stations.find(id);
    if (i == cache_stations.end())
        return 0;
    return &(i->second);
}

const std::map<SummaryKey, SummaryValue> &Model::summaries() const
{
    return cache_summary;
}

const std::vector<Value> &Model::values() const
{
    return cache_values;
}

std::vector<Value> &Model::values()
{
    return cache_values;
}

void Model::update(Value &val, const wreport::Var &new_val)
{
    Record change;
    change.set(DBA_KEY_ANA_ID, val.ana_id);
    change.set(DBA_KEY_REP_MEMO, val.rep_memo.c_str());
    change.set(val.level);
    change.set(val.trange);
    change.set_datetime(val.date);
    change.set(new_val);
    db->insert(change, true, false);
    val.var = new_val;
}

void Model::set_initial_filter(const Record &rec)
{
    active_filter = rec;
    next_filter = rec;
}

void Model::dballe_connect(const std::string &dballe_url)
{
    if (db)
    {
        delete db;
        db = 0;
    }

    auto_ptr<DB> new_db = DB::connect_from_url(dballe_url.c_str());
    db = new_db.release();
    refresh();
}

void Model::refresh()
{
    cache_stations.clear();
    cache_summary.clear();
    cache_values.clear();

    // Query summary for the currently active filter
    qDebug() << "Refresh summary started";
    auto_ptr<db::Cursor> cur = this->db->query_summary(active_filter);
    while (cur->next())
    {
        int ana_id = cur->get_station_id();
        if (cache_stations.find(ana_id) == cache_stations.end())
            cache_stations.insert(make_pair(ana_id, Station(*cur)));

        cache_summary.insert(make_pair(SummaryKey(*cur), SummaryValue(*cur)));
    }
    emit active_filter_changed();

    // Query data for the currently active filter
    qDebug() << "Refresh data started";
    Record query(active_filter);
    query.set("limit", 100);
    cur = this->db->query_data(query);
    while (cur->next())
    {
        cache_values.push_back(Value(*cur));
    }
    emit data_changed();

    // Recompute the available choices
    qDebug() << "Summary collation started";
    process_summary();

    qDebug() << "Refresh done";
}

void Model::activate_next_filter()
{
    active_filter = next_filter;
    refresh();
}

void Model::process_summary()
{
    set<std::string> set_reports;
    set<dballe::Level> set_levels;
    set<dballe::Trange> set_tranges;
    set<wreport::Varcode> set_varcodes;

    // Scan the filter building a todo list of things to match
    bool has_flt_rep_memo = next_filter.contains(DBA_KEY_REP_MEMO);
    string flt_rep_memo = next_filter.get(DBA_KEY_REP_MEMO, "");
    bool has_flt_level = next_filter.contains_level();
    Level flt_level = next_filter.get_level();
    bool has_flt_trange = next_filter.contains_trange();
    Trange flt_trange = next_filter.get_trange();
    bool has_flt_varcode = next_filter.contains(DBA_KEY_VAR);
    wreport::Varcode flt_varcode = wreport::descriptor_code(next_filter.get(DBA_KEY_VAR, "B00000"));
    bool has_flt_area = next_filter.contains(DBA_KEY_LATMIN) && next_filter.contains(DBA_KEY_LATMAX)
            && next_filter.contains(DBA_KEY_LONMIN) && next_filter.contains(DBA_KEY_LONMAX);
    double flt_area_latmin = next_filter.get(DBA_KEY_LATMIN, 0.0);
    double flt_area_latmax = next_filter.get(DBA_KEY_LATMAX, 0.0);
    double flt_area_lonmin = next_filter.get(DBA_KEY_LONMIN, 0.0);
    double flt_area_lonmax = next_filter.get(DBA_KEY_LONMAX, 0.0);
    bool has_flt_station_id = next_filter.contains(DBA_KEY_ANA_ID);
    int flt_station_id = next_filter.get(DBA_KEY_ANA_ID, 0);

    for (map<SummaryKey, SummaryValue>::const_iterator i = cache_summary.begin();
         i != cache_summary.end(); ++i)
    {
        if (has_flt_station_id && i->first.ana_id != flt_station_id)
            continue;

        if (has_flt_area)
        {
            const Station& s = cache_stations.find(i->first.ana_id)->second;
            if (s.lat < flt_area_latmin || s.lat > flt_area_latmax
             || s.lon < flt_area_lonmin || s.lon > flt_area_lonmax)
                continue;
        }

        bool match_rep_memo = !has_flt_rep_memo || flt_rep_memo == i->first.rep_memo;
        bool match_level    = !has_flt_level || flt_level == i->first.level;
        bool match_trange   = !has_flt_trange || flt_trange == i->first.trange;
        bool match_varcode  = !has_flt_varcode || flt_varcode == i->first.varcode;

        if (match_level && match_trange && match_varcode)
            set_reports.insert(i->first.rep_memo);
        if (match_rep_memo && match_trange && match_varcode)
            set_levels.insert(i->first.level);
        if(match_rep_memo && match_level && match_varcode)
            set_tranges.insert(i->first.trange);
        if (match_rep_memo && match_level && match_trange)
            set_varcodes.insert(i->first.varcode);
    }

    reports.set_items(set_reports);
    levels.set_items(set_levels);
    tranges.set_items(set_tranges);
    varcodes.set_items(set_varcodes);

    emit next_filter_changed();
}

void Model::select_report(const string &val)
{
    next_filter.set(DBA_KEY_REP_MEMO, val.c_str());
    process_summary();
}

void Model::select_level(const Level &val)
{
    next_filter.set(val);
    process_summary();
}

void Model::select_trange(const Trange &val)
{
    next_filter.set(val);
    process_summary();
}

void Model::select_varcode(wreport::Varcode val)
{
    std::string code = wreport::varcode_format(val);
    next_filter.set(DBA_KEY_VAR, code.c_str());
    process_summary();
}

void Model::select_station_id(int id)
{
    next_filter.unset(DBA_KEY_LATMIN);
    next_filter.unset(DBA_KEY_LATMAX);
    next_filter.unset(DBA_KEY_LONMIN);
    next_filter.unset(DBA_KEY_LONMAX);
    next_filter.set(DBA_KEY_ANA_ID, id);
    process_summary();
}

void Model::select_station_bounds(double latmin, double latmax, double lonmin, double lonmax)
{
    qDebug() << "SSB" << latmin << latmax << lonmin << lonmax;
    if (latmin < -90) latmin = -90;
    if (latmax > 90) latmax = 90;
    if (lonmin < -180) lonmin = -180;
    if (lonmax > 180) lonmax = 180;

    next_filter.set(DBA_KEY_LATMIN, latmin);
    next_filter.set(DBA_KEY_LATMAX, latmax);
    next_filter.set(DBA_KEY_LONMIN, lonmin);
    next_filter.set(DBA_KEY_LONMAX, lonmax);
    next_filter.unset(DBA_KEY_ANA_ID);
    process_summary();
}

void Model::unselect_report()
{
    next_filter.unset(DBA_KEY_REP_MEMO);
    process_summary();
}

void Model::unselect_level()
{
    next_filter.unset(DBA_KEY_LEVELTYPE1);
    next_filter.unset(DBA_KEY_L1);
    next_filter.unset(DBA_KEY_LEVELTYPE2);
    next_filter.unset(DBA_KEY_L2);
    process_summary();
}

void Model::unselect_trange()
{
    next_filter.unset(DBA_KEY_PINDICATOR);
    next_filter.unset(DBA_KEY_P1);
    next_filter.unset(DBA_KEY_P2);
    process_summary();
}

void Model::unselect_varcode()
{
    next_filter.unset(DBA_KEY_VAR);
    process_summary();
}

void Model::unselect_station()
{
    next_filter.unset(DBA_KEY_LATMIN);
    next_filter.unset(DBA_KEY_LATMAX);
    next_filter.unset(DBA_KEY_LONMIN);
    next_filter.unset(DBA_KEY_LONMAX);
    next_filter.unset(DBA_KEY_ANA_ID);
    process_summary();
}

Station::Station(const dballe::db::Cursor &cur)
{
    id = cur.get_station_id();
    lat = cur.get_lat();
    lon = cur.get_lon();
    ident = cur.get_ident("");
}


bool SummaryKey::operator <(const SummaryKey &sk) const
{
    if (ana_id < sk.ana_id) return true;
    if (ana_id > sk.ana_id) return false;
    if (rep_memo < sk.rep_memo) return true;
    if (rep_memo > sk.rep_memo) return false;
    if (int cmp = level.compare(sk.level)) return cmp < 0;
    if (int cmp = trange.compare(sk.trange)) return cmp < 0;
    return varcode < sk.varcode;
}

SummaryKey::SummaryKey(const dballe::db::Cursor &cur)
{
    ana_id = cur.get_station_id();
    rep_memo = cur.get_rep_memo("");
    level = cur.get_level();
    trange = cur.get_trange();
    varcode = cur.get_varcode();
}


SummaryValue::SummaryValue(const dballe::db::Cursor &cur)
{

}


Value::Value(const dballe::db::Cursor &cur)
    : var(cur.get_var())
{
    ana_id = cur.get_station_id();
    rep_memo = cur.get_rep_memo("");
    level = cur.get_level();
    trange = cur.get_trange();
    cur.get_datetime(date);
}


ModelAction::ModelAction(Model &model, QObject *parent)
    : QAction(parent), model(model)
{
    connect(this, SIGNAL(triggered()), this, SLOT(on_trigger()));
}


template class FilterModelBase<std::string>;
template class FilterModelBase<Level>;
template class FilterModelBase<Trange>;
template class FilterModelBase<wreport::Varcode>;
