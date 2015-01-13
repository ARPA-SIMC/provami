#include "provami/types.h"
#include <dballe/db/db.h>

using namespace dballe;

namespace provami {

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


BaseValue::BaseValue(const db::Cursor &cur)
    : var(cur.get_var())
{
    ana_id = cur.get_station_id();
    value_id = cur.attr_reference_id();
}

StationValue::StationValue(const dballe::db::Cursor &cur)
    : BaseValue(cur)
{
}

Value::Value(const dballe::db::Cursor &cur)
    : BaseValue(cur)
{
    rep_memo = cur.get_rep_memo("");
    level = cur.get_level();
    trange = cur.get_trange();
    cur.get_datetime(date);
}

}
