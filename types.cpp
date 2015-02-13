#include "provami/types.h"
#include <dballe/db/db.h>
#include <dballe/core/record.h>

using namespace dballe;

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

}
