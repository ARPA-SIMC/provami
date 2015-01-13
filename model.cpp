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
    fprintf(stderr, "Refresh summary started\n");
    cache_stations.clear();
    cache_summary.clear();
    cache_values.clear();
    cache_reports.clear();
    cache_levels.clear();
    cache_tranges.clear();
    cache_varcodes.clear();

    Record query;
    auto_ptr<db::Cursor> cur = this->db->query_summary(query);
    while (cur->next())
    {
        int ana_id = cur->get_station_id();
        if (cache_stations.find(ana_id) == cache_stations.end())
            cache_stations.insert(make_pair(ana_id, Station(*cur)));

        cache_summary.insert(make_pair(SummaryKey(*cur), SummaryValue(*cur)));
    }

    fprintf(stderr, "Refresh data started\n");
    query.set("limit", 100);
    cur = this->db->query_data(query);
    while (cur->next())
    {
        cache_values.push_back(Value(*cur));
    }

    fprintf(stderr, "Summary collation started\n");

    set<std::string> set_reports;
    set<dballe::Level> set_levels;
    set<dballe::Trange> set_tranges;
    set<wreport::Varcode> set_vars;
    for (map<SummaryKey, SummaryValue>::const_iterator i = cache_summary.begin();
         i != cache_summary.end(); ++i)
    {
        set_reports.insert(i->first.rep_memo);
        set_levels.insert(i->first.level);
        set_tranges.insert(i->first.trange);
        set_vars.insert(i->first.var);
    }

    std::copy(set_reports.begin(), set_reports.end(), back_inserter(cache_reports));
    std::copy(set_levels.begin(), set_levels.end(), back_inserter(cache_levels));
    std::copy(set_tranges.begin(), set_tranges.end(), back_inserter(cache_tranges));
    std::copy(set_vars.begin(), set_vars.end(), back_inserter(cache_varcodes));

    fprintf(stderr, "Notifying refresh done\n");

    emit refreshed();

    fprintf(stderr, "Refresh done\n");
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
    return var < sk.var;
}

SummaryKey::SummaryKey(const dballe::db::Cursor &cur)
{
    ana_id = cur.get_station_id();
    rep_memo = cur.get_rep_memo("");
    level = cur.get_level();
    trange = cur.get_trange();
    var = cur.get_varcode();
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
