#include "provami/types.h"
#include <dballe/db/db.h>
#include <dballe/db/summary.h>
#include <QDebug>

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
    date = cur.get_datetime();
}

Matcher::Matcher(const dballe::Query& query_gen, const std::map<int, Station> &all_stations)
{
    const core::Query& query = core::Query::downcast(query_gen);

    // Scan the filter building a todo list of things to match

    // If there is any filtering on the station, build a whitelist of matching stations
    bool has_flt_ident = query.has_ident;
    bool has_flt_area = !query.coords_min.is_missing() && !query.coords_max.is_missing();
    bool has_flt_station_id = query.ana_id != MISSING_INT;
    if (has_flt_ident || has_flt_area || has_flt_station_id)
    {
        int flt_station_id = query.ana_id;
        string flt_ident = query.has_ident ? query.ident : string();
        // FIXME: this only supports longitude filters that do not cross the date change line
        double flt_area_latmin = query.coords_min.lat != MISSING_INT ? query.coords_min.dlat() : -90.0;
        double flt_area_latmax = query.coords_max.lat != MISSING_INT ? query.coords_max.dlat() : 90.0;
        double flt_area_lonmin = query.coords_min.lon != MISSING_INT ? query.coords_min.dlon() : -1000.0;
        double flt_area_lonmax = query.coords_max.lon != MISSING_INT ? query.coords_max.dlon() : 1000.0;
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

    has_flt_rep_memo = !query.rep_memo.empty();
    wanted_rep_memo = query.rep_memo;

    has_flt_level = !query.level.is_missing();
    wanted_level = query.level;

    has_flt_trange = !query.trange.is_missing();
    wanted_trange = query.trange;

    has_flt_varcode = !query.varcodes.empty();
    wanted_varcode = has_flt_varcode ? *query.varcodes.begin() : 0;

    wanted_dtmin = query.datetime_min;
    wanted_dtmax = query.datetime_max;
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
