#include "provami/types.h"
#include <dballe/db/db.h>
#include <dballe/db/summary.h>
#include <dballe/core/record.h>

using namespace dballe;
using namespace std;

namespace provami {

Station::Station(const dballe::db::Cursor &cur)
{
    id = cur.get_station_id();
    lat = cur.get_lat();
    lon = cur.get_lon();
    ident = cur.get_ident("");
}



BaseValue::BaseValue(const db::Cursor &cur)
    : var(cur.get_var())
{
    ana_id = cur.get_station_id();
    rep_memo = cur.get_rep_memo();
    value_id = cur.attr_reference_id();
}

StationValue::StationValue(const dballe::db::Cursor &cur)
    : BaseValue(cur)
{
}

Value::Value(const dballe::db::Cursor &cur)
    : BaseValue(cur)
{
    level = cur.get_level();
    trange = cur.get_trange();
    cur.get_datetime(date);
}

Matcher::Matcher(const dballe::Query &query, const std::map<int, Station> &all_stations)
{
    // Scan the filter building a todo list of things to match

    // If there is any filtering on the station, build a whitelist of matching stations
    bool has_flt_ident = query.contains(DBA_KEY_IDENT);
    bool has_flt_area = query.contains(DBA_KEY_LATMIN) && query.contains(DBA_KEY_LATMAX)
            && query.contains(DBA_KEY_LONMIN) && query.contains(DBA_KEY_LONMAX);
    bool has_flt_station_id = query.contains(DBA_KEY_ANA_ID);
    if (has_flt_ident || has_flt_area || has_flt_station_id)
    {
        int flt_station_id = query.get(DBA_KEY_ANA_ID, 0);
        string flt_ident = query.get(DBA_KEY_IDENT, "");
        double flt_area_latmin = query.get(DBA_KEY_LATMIN, -100000);
        double flt_area_latmax = query.get(DBA_KEY_LATMAX,  100000);
        double flt_area_lonmin = query.get(DBA_KEY_LONMIN, -100000);
        double flt_area_lonmax = query.get(DBA_KEY_LONMAX,  100000);
        has_flt_station = true;
        for (auto s: all_stations)
        {
            const Station& station = s.second;
            if (has_flt_station_id && station.id != flt_station_id)
                continue;

            if (has_flt_area)
            {
                if (station.lat < flt_area_latmin || station.lat > flt_area_latmax
                 || station.lon < flt_area_lonmin || station.lon > flt_area_lonmax)
                    continue;
            }

            if (has_flt_ident && flt_ident != station.ident)
                continue;

            wanted_stations.insert(station.id);
        }
    }

    has_flt_rep_memo = query.contains(DBA_KEY_REP_MEMO);
    wanted_rep_memo = query.get(DBA_KEY_REP_MEMO, "");

    has_flt_level = query.contains_level();
    wanted_level = query.get_level();

    has_flt_trange = query.contains_trange();
    wanted_trange = query.get_trange();

    has_flt_varcode = query.contains(DBA_KEY_VAR);
    wanted_varcode = wreport::descriptor_code(query.get(DBA_KEY_VAR, "B00000"));

    query.parse_date_extremes(wanted_dtmin, wanted_dtmax);
}

bool Matcher::match(const dballe::db::summary::Entry& entry) const
{
    if (has_flt_station && wanted_stations.find(entry.ana_id) == wanted_stations.end())
        return false;

    if (has_flt_rep_memo && wanted_rep_memo != entry.rep_memo)
        return false;

    if (has_flt_level && wanted_level != entry.level)
        return false;

    if (has_flt_trange && wanted_trange != entry.trange)
        return false;

    if (has_flt_varcode && wanted_varcode != entry.varcode)
        return false;

    if (!Datetime::range_contains(wanted_dtmin, wanted_dtmax,
                                  entry.datemin, entry.datemax))
        return false;

    return true;
}

}
