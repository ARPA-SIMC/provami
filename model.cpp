#include "model.h"
#include <memory>
#include <dballe/core/record.h>
#include <dballe/db/db.h>
#include <set>
#include <stdio.h>

using namespace std;
using namespace dballe;

Model::Model()
    : db(0)
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

const std::vector<string>& Model::reports() const { return cache_reports; }
const std::vector<Level> &Model::levels() const { return cache_levels; }
const std::vector<Trange> &Model::tranges() const { return cache_tranges; }
const std::vector<wreport::Varcode> &Model::varcodes() const { return cache_varcodes; }

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
    fprintf(stderr, "Refresh summary started\n");
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
    fprintf(stderr, "Refresh data started\n");
    Record query(active_filter);
    query.set("limit", 100);
    cur = this->db->query_data(query);
    while (cur->next())
    {
        cache_values.push_back(Value(*cur));
    }
    emit data_changed();

    // Recompute the available choices
    fprintf(stderr, "Summary collation started\n");
    process_summary();

    fprintf(stderr, "Refresh done\n");
}

void Model::activate_next_filter()
{
    active_filter = next_filter;
    refresh();
}

void Model::process_summary()
{
    fprintf(stderr, "PS\n");
    cache_reports.clear();
    cache_levels.clear();
    cache_tranges.clear();
    cache_varcodes.clear();

    set<std::string> set_reports;
    set<dballe::Level> set_levels;
    set<dballe::Trange> set_tranges;
    set<wreport::Varcode> set_varcode;

    // Scan the filter building a todo list of things to match
    bool has_flt_rep_memo = next_filter.contains(DBA_KEY_REP_MEMO);
    string flt_rep_memo = next_filter.get(DBA_KEY_REP_MEMO, "");
    bool has_flt_level = next_filter.contains_level();
    Level flt_level = next_filter.get_level();
    bool has_flt_trange = next_filter.contains_trange();
    Trange flt_trange = next_filter.get_trange();
    bool has_flt_varcode = next_filter.contains(DBA_KEY_VAR);
    wreport::Varcode flt_varcode = next_filter.get(DBA_KEY_VAR, 0);

    for (map<SummaryKey, SummaryValue>::const_iterator i = cache_summary.begin();
         i != cache_summary.end(); ++i)
    {
        bool match_rep_memo = !has_flt_rep_memo || flt_rep_memo == i->first.rep_memo;
        bool match_level    = !has_flt_level || flt_level == i->first.level;
        bool match_trange   = !has_flt_trange || flt_trange == i->first.trange;
        bool match_varcode  = !has_flt_varcode || flt_varcode == i->first.varcode;

        if (match_level && match_trange && match_varcode)
            set_reports.insert(i->first.rep_memo);
        if (match_rep_memo && match_trange && match_varcode)
            set_levels.insert(i->first.level);
        if (match_rep_memo && match_level && match_varcode)
            set_tranges.insert(i->first.trange);
        if (match_rep_memo && match_level && match_trange)
            set_varcode.insert(i->first.varcode);
    }

    std::copy(set_reports.begin(), set_reports.end(), back_inserter(cache_reports));
    std::copy(set_levels.begin(), set_levels.end(), back_inserter(cache_levels));
    std::copy(set_tranges.begin(), set_tranges.end(), back_inserter(cache_tranges));
    std::copy(set_varcode.begin(), set_varcode.end(), back_inserter(cache_varcodes));

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
    next_filter.set(DBA_KEY_VAR, val);
    process_summary();
}

Station::Station(const dballe::db::Cursor &cur)
{
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
