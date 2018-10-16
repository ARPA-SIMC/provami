#ifndef PROVAMI_TYPES_H
#define PROVAMI_TYPES_H

#include <wreport/var.h>
#include <dballe/types.h>
#include <set>
#include <map>

namespace dballe {
struct Query;
namespace db {
class CursorStation;
class CursorSummary;
class CursorValue;
class CursorStationData;
class CursorData;
}
}

namespace provami {

struct BaseValue
{
    int ana_id;
    int value_id;
    std::string rep_memo;
    wreport::Var var;

    bool operator==(const BaseValue& val) const
    {
        if (ana_id != val.ana_id) return false;
        if (value_id != val.value_id) return false;
        if (var.code() != val.var.code()) return false;
        if (rep_memo != val.rep_memo) return false;
        return true;
    }
protected:
    BaseValue(const dballe::db::CursorValue& cur);
};

struct StationValue : public BaseValue
{
    StationValue(const dballe::db::CursorStationData& cur);
};

struct Value : public BaseValue
{
    dballe::Level level;
    dballe::Trange trange;
    dballe::Datetime date;

    bool operator==(const Value& val) const
    {
        if (!BaseValue::operator==(val)) return false;
        if (level != val.level) return false;
        if (trange != val.trange) return false;
        if (date != val.date) return false;
        return true;
    }

protected:
    Value(const dballe::db::CursorData& cur);

    friend class Model;
};

}

#endif // TYPES_H
