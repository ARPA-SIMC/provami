#include "provami/types.h"
#include <dballe/cursor.h>
#include <dballe/db/db.h>
#include <QDebug>

using namespace dballe;
using namespace std;

namespace provami {

BaseValue::BaseValue(const CursorStationData& cur)
    : var(cur.get_var())
{
    auto station = cur.get_station();
    ana_id = station.id;
    rep_memo = station.report;
    value_id = dynamic_cast<const db::CursorStationData*>(&cur)->attr_reference_id();
}

BaseValue::BaseValue(const CursorData& cur)
    : var(cur.get_var())
{
    auto station = cur.get_station();
    ana_id = station.id;
    rep_memo = station.report;
    value_id = dynamic_cast<const db::CursorData*>(&cur)->attr_reference_id();
}

StationValue::StationValue(const dballe::CursorStationData& cur)
    : BaseValue(cur)
{
}

Value::Value(const dballe::CursorData& cur)
    : BaseValue(cur)
{
    level = cur.get_level();
    trange = cur.get_trange();
    date = cur.get_datetime();
}

}
