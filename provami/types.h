#ifndef PROVAMI_TYPES_H
#define PROVAMI_TYPES_H

#include <wreport/var.h>
#include <dballe/core/defs.h>

namespace dballe {
namespace db {
class Cursor;
}
}

namespace provami {

struct Station
{
    int id;
    double lat;
    double lon;
    std::string ident;

protected:
    Station(const dballe::db::Cursor& cur);

    friend class Model;
};

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
    BaseValue(const dballe::db::Cursor& cur);
};

struct StationValue : public BaseValue
{
    StationValue(const dballe::db::Cursor& cur);
};

struct Value : public BaseValue
{
    dballe::Level level;
    dballe::Trange trange;
    int date[6];

    bool operator==(const Value& val) const
    {
        if (!BaseValue::operator==(val)) return false;
        if (level != val.level) return false;
        if (trange != val.trange) return false;
        for (unsigned i = 0; i < 6; ++i)
            if (date[i] != val.date[i]) return false;
        return true;
    }

protected:
    Value(const dballe::db::Cursor& cur);

    friend class Model;
};

}

#endif // TYPES_H
