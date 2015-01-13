#include "model.h"
#include <memory>
#include <dballe/core/record.h>
#include <dballe/db/db.h>
#include <stdio.h>

using namespace std;

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

const std::map<SummaryKey, SummaryValue> &Model::summary() const
{
    return cache_summary;
}

void Model::dballe_connect(const std::string &dballe_url)
{
    using namespace dballe;

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
    using namespace dballe;

    fprintf(stderr, "Refresh started\n");
    cache_stations.clear();
    cache_summary.clear();

    Record query, result;
    auto_ptr<db::Cursor> cur = this->db->query_summary(query);
    while (cur->next())
    {
        cur->to_record(result);
        int ana_id = result.get(DBA_KEY_ANA_ID).enqi();
        if (cache_stations.find(ana_id) == cache_stations.end())
            cache_stations.insert(make_pair(ana_id, Station(result)));

        cache_summary.insert(make_pair(SummaryKey(result), SummaryValue(result)));
    }
    fprintf(stderr, "Notifying refresh done\n");

    emit refreshed();

    fprintf(stderr, "Refresh done\n");
}

Station::Station(const dballe::Record &rec)
{
    using namespace dballe;

    lat = rec.get(DBA_KEY_LAT).enqd();
    lon = rec.get(DBA_KEY_LON).enqd();
    ident = rec.get(DBA_KEY_IDENT, "");
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

SummaryKey::SummaryKey(const dballe::Record &rec)
{
    using namespace dballe;

    ana_id = rec.get(DBA_KEY_ANA_ID).enqi();
    rep_memo = rec.get(DBA_KEY_REP_MEMO, "");
    level = rec.get_level();
    trange = rec.get_trange();
    fprintf(stderr, "VAVA %s", rec.get(DBA_KEY_VAR).enqc());
    var = resolve_varcode(rec.get(DBA_KEY_VAR).enqc());
}


SummaryValue::SummaryValue(const dballe::Record &rec)
{
}
