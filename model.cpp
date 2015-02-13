#include "provami/model.h"
#include <memory>
#include <dballe/core/record.h>
#include <dballe/db/db.h>
#include <set>
#include <algorithm>
#include <QDebug>

using namespace std;
using namespace dballe;

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
string FilterReportModel::from_record(const dballe::Record& rec) const
{
    return rec.get(DBA_KEY_REP_MEMO, "");
}
void FilterReportModel::filter_select(const string &val) { model.select_report(val); }
void FilterReportModel::filter_unselect() { model.unselect_report(); }
QVariant FilterReportModel::item_to_table_cell(const std::string& val) const { return QVariant(val.c_str()); }


FilterLevelModel::FilterLevelModel(Model &model, QObject *parent)
    : FilterModelBase<dballe::Level>(model, parent)
{
}
Level FilterLevelModel::from_record(const dballe::Record& rec) const
{
    return rec.get_level();
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
Trange FilterTrangeModel::from_record(const dballe::Record& rec) const
{
    return rec.get_trange();
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
wreport::Varcode FilterVarcodeModel::from_record(const dballe::Record& rec) const
{
    const char* scode = rec.get(DBA_KEY_VAR, "");
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

FilterIdentModel::FilterIdentModel(Model &model, QObject *parent)
    : FilterModelBase<std::string>(model, parent)
{
}
string FilterIdentModel::from_record(const dballe::Record& rec) const
{
    return rec.get(DBA_KEY_IDENT, "");
}
void FilterIdentModel::filter_select(const string &val) { model.select_ident(val); }
void FilterIdentModel::filter_unselect() { model.unselect_ident(); }
QVariant FilterIdentModel::item_to_table_cell(const std::string& val) const { return QVariant(val.c_str()); }

Model::Model()
    : db(0), reports(*this), levels(*this), tranges(*this), varcodes(*this), idents(*this)
{
    connect(&refresh_thread, SIGNAL(have_new_summary(bool)), this, SLOT(on_have_new_summary(bool)));
    connect(&refresh_thread, SIGNAL(have_new_data()), this, SLOT(on_have_new_data()));
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

/*
const std::map<SummaryKey, SummaryValue> &Model::summaries() const
{
    return cache_summary;
}
*/

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

void Model::update(StationValue &val, const wreport::Var &new_val)
{
    Record change;
    change.set_ana_context();
    change.set(DBA_KEY_ANA_ID, val.ana_id);
    change.set(DBA_KEY_REP_MEMO, val.rep_memo.c_str());
    change.set(new_val);
    db->insert(change, true, false);
    val.var = new_val;
}

void Model::update(int var_id, wreport::Varcode var_related, const wreport::Var &new_val)
{
    Record change;
    change.set(new_val);
    db->attr_insert(var_id, var_related, change);
}

void Model::remove(const Value &val)
{
    emit begin_data_changed();
    Record change;
    change.set(DBA_KEY_ANA_ID, val.ana_id);
    change.set(DBA_KEY_REP_MEMO, val.rep_memo.c_str());
    change.set(val.level);
    change.set(val.trange);
    change.set_datetime(val.date);
    change.set(DBA_KEY_VAR, format_code(val.var.code()));
    db->remove(change);
    vector<Value>::iterator i = std::find(cache_values.begin(), cache_values.end(), val);
    if (i != cache_values.end())
    {
        cache_values.erase(i);
    }
    emit end_data_changed();
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

    m_dballe_url = dballe_url;

    auto new_db = DB::connect_from_url(dballe_url.c_str());
    db = new_db.release();
    refresh_thread.db = db;

    refresh();
}

void Model::refresh(bool accurate)
{
    // Query summary for the currently active filter
    emit progress("summary", "Loading summary...");

    // Check if the active filter is empty
    bool is_empty = active_filter.iter_keys([](dba_keyword, const wreport::Var&) { return false; });

    bool want_details = is_empty || accurate;

    refresh_thread.query_summary(active_filter, want_details);
}

void Model::on_have_new_summary(bool with_details)
{
    emit progress("summary", "Processing summary...");
    qDebug() << "Refresh summary results arrived";
    emit progress("data", "Loading data...");
    Record query(active_filter);
    query.set("limit", (int)limit);
    refresh_thread.query_data(query);

    bool is_empty = active_filter.iter_keys([](dba_keyword, const wreport::Var&) { return false; });
    Summary& summary = is_empty ? global_summary : current_summary;

    summary.reset(active_filter);

    cache_stations.clear();
    cache_values.clear();

    highlight.reset();

    while (refresh_thread.cur_summary->next())
    {
        int ana_id = refresh_thread.cur_summary->get_station_id();
        if (cache_stations.find(ana_id) == cache_stations.end())
            cache_stations.insert(make_pair(ana_id, Station(*refresh_thread.cur_summary)));

        summary.add_summary(*refresh_thread.cur_summary, with_details);
    }

    // Recompute the available choices
    qDebug() << "Summary collation started";
    process_summary();
    emit progress("summary");
    emit active_filter_changed();
}

void Model::on_have_new_data()
{
    emit progress("data", "Processing data...");

    emit begin_data_changed();
    // Query data for the currently active filter
    while (refresh_thread.cur_data->next())
    {
        cache_values.push_back(Value(*refresh_thread.cur_data));
    }
    emit end_data_changed();

    emit progress("data");
}


void Model::activate_next_filter(bool accurate)
{
    active_filter = next_filter;
    refresh(accurate);
}

void Model::process_summary()
 {
    set<std::string> set_reports;
    set<dballe::Level> set_levels;
    set<dballe::Trange> set_tranges;
    set<wreport::Varcode> set_varcodes;
    set<std::string> set_idents;


    // Scan the filter building a todo list of things to match
    bool has_flt_ident = next_filter.contains(DBA_KEY_IDENT);
    string flt_ident = next_filter.get(DBA_KEY_IDENT, "");
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

    std::function<bool(const SummaryKey&, const SummaryValue&)> process =
            [&](const SummaryKey& key, const SummaryValue& val)
    {
        if (has_flt_station_id && key.ana_id != flt_station_id)
            return true;

        const Station& s = cache_stations.find(key.ana_id)->second;

        if (has_flt_area)
        {
            if (s.lat < flt_area_latmin || s.lat > flt_area_latmax
             || s.lon < flt_area_lonmin || s.lon > flt_area_lonmax)
                return true;
        }

        bool match_rep_memo = !has_flt_rep_memo || flt_rep_memo == key.rep_memo;
        bool match_level    = !has_flt_level || flt_level == key.level;
        bool match_trange   = !has_flt_trange || flt_trange == key.trange;
        bool match_varcode  = !has_flt_varcode || flt_varcode == key.varcode;
        bool match_ident    = !has_flt_ident || flt_ident == s.ident;

        if (match_rep_memo && match_level && match_trange && match_varcode)
            if (!s.ident.empty())
                set_idents.insert(s.ident);
        if (match_ident && match_level && match_trange && match_varcode)
            set_reports.insert(key.rep_memo);
        if (match_ident && match_rep_memo && match_trange && match_varcode)
            set_levels.insert(key.level);
        if (match_ident && match_rep_memo && match_level && match_varcode)
            set_tranges.insert(key.trange);
        if (match_ident && match_rep_memo && match_level && match_trange)
            set_varcodes.insert(key.varcode);

        return true;
    };

    global_summary.iterate(process);

    reports.set_items(set_reports);
    levels.set_items(set_levels);
    tranges.set_items(set_tranges);
    varcodes.set_items(set_varcodes);
    idents.set_items(set_idents);

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

/*
 * TODO: we will want something like this when we will implement
* printing a minimal query equivalent for the current filter,
 * to use in command line apps and to run exporter scripts.
static void optimize_datetime(dballe::Record& rec)
{
    int dtmin[6];
    int dtmax[6];
    rec.parse_date_extremes(dtmin, dtmax);
    if (dtmin == dtmax)
    {
        rec.unset_datetimemin();
        rec.unset_datetimemax();
        rec.set_datetime(dtmin);
    } else {
        rec.unset_datetime();
        rec.set_datetimemin(dtmin);
        rec.set_datetimemax(dtmax);
    }
}
*/

void Model::select_datemin(const dballe::Datetime& val)
{
    int old[6];
    next_filter.get_datetimemin(old);
    if (val == dballe::Datetime(old)) return;
    next_filter.setmin(val);
    process_summary();
}

void Model::select_datemax(const dballe::Datetime& val)
{
    int old[6];
    next_filter.get_datetimemax(old);
    if (val == dballe::Datetime(old)) return;
    next_filter.setmax(val);
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

void Model::select_ident(const string &val)
{
    next_filter.set(DBA_KEY_IDENT, val.c_str());
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

void Model::unselect_datemin()
{
    int old[6];
    next_filter.get_datetimemin(old);
    if (old[0] == MISSING_INT) return;

    next_filter.unset_datetimemin();
    process_summary();
}

void Model::unselect_datemax()
{
    int old[6];
    next_filter.get_datetimemax(old);
    if (old[0] == MISSING_INT) return;

    next_filter.unset_datetimemax();
    process_summary();
}

void Model::set_filter(const Record &new_filter)
{
    next_filter = new_filter;
    process_summary();}

void Model::unselect_station()
{
    next_filter.unset(DBA_KEY_LATMIN);
    next_filter.unset(DBA_KEY_LATMAX);
    next_filter.unset(DBA_KEY_LONMIN);
    next_filter.unset(DBA_KEY_LONMAX);
    next_filter.unset(DBA_KEY_ANA_ID);
    process_summary();
}

void Model::unselect_ident()
{
    next_filter.unset(DBA_KEY_IDENT);
    process_summary();
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

}
