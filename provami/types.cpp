#include "provami/types.h"
#include <dballe/db/db.h>
#include <dballe/db/summary.h>
#include <QDebug>

using namespace dballe;
using namespace std;

namespace provami {

BaseValue::BaseValue(const db::CursorValue& cur)
    : var(cur.get_var())
{
    ana_id = cur.get_station_id();
    rep_memo = cur.get_rep_memo();
    value_id = cur.attr_reference_id();
}

StationValue::StationValue(const dballe::db::CursorStationData& cur)
    : BaseValue(cur)
{
}

Value::Value(const dballe::db::CursorData& cur)
    : BaseValue(cur)
{
    level = cur.get_level();
    trange = cur.get_trange();
    date = cur.get_datetime();
}

}
