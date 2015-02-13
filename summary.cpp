#include "provami/summary.h"

using namespace std;
using namespace dballe;

namespace provami {

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
    rep_memo = cur.get_rep_memo();
    level = cur.get_level();
    trange = cur.get_trange();
    varcode = cur.get_varcode();
}


SummaryValue::SummaryValue(dballe::db::Cursor &cur, bool want_details)
{
    if (want_details)
    {
        Record rec;
        cur.to_record(rec);
        count = rec[DBA_KEY_CONTEXT_ID].enqi();
        datemin = rec.get_datetimemin();
        datemax = rec.get_datetimemax();
    }
}

Summary::Summary(QObject *parent) :
    QObject(parent)
{
}

void Summary::reset(const dballe::Query &query)
{
    this->query = query;
    summary.clear();
    dtmin = Datetime();
    dtmax = Datetime();
    count = MISSING_INT;
    valid = false;
}

void Summary::add_summary(dballe::db::Cursor &cur, bool with_details)
{
    auto inserted = summary.insert(make_pair(
                         SummaryKey(cur),
                         SummaryValue(cur, with_details)));

    if (with_details)
    {
        const SummaryValue& val = inserted.first->second;
        if (!valid)
        {
            dtmin = val.datemin;
            dtmax = val.datemax;
            count = val.count;
            valid = true;
        } else {
            if (val.datemin < dtmin) dtmin = val.datemin;
            if (val.datemax > dtmax) dtmax = val.datemax;
            count += val.count;
        }
    }
}

bool Summary::iterate(std::function<bool (const SummaryKey &, const SummaryValue)> f) const
{
    for (auto i: summary)
        if (!f(i.first, i.second))
            break;
}

}
