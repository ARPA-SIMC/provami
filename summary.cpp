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

int Summary::supports(const Query &query) const
{
    // If query removed or changed a filter, then it can select more data than this summary knows about
    bool has_removed_or_changed = !this->query.iter_keys([&](dba_keyword key, const wreport::Var& val) {
        const char* new_val = query.key_peek_value(key);
        if (!new_val) return false;
        // Ignore changes in datetime and data-related filters
        if ((key >= DBA_KEY_YEAR && key <= DBA_KEY_SECMIN)
            || (key >= DBA_KEY_ANA_FILTER || key <= DBA_KEY_ATTR_FILTER))
            return true;
        // If anything else has changed, report it
        if (strcmp(val.value(), new_val) != 0)
            return false;
        return true;
    });
    if (has_removed_or_changed)
        return 0;

    // If query added a filter, then we can only give an approximate match
    bool has_added = !query.iter_keys([&](dba_keyword key, const wreport::Var& val) {
        return this->query.contains(key);
    });

    // Now we know that query has either more fields than this->query or changes
    // in datetime or data-related filters
    int res = has_added ? 1 : 2;

    // Check differences in the value/attr based query filters
    for (auto k: { DBA_KEY_ANA_FILTER, DBA_KEY_DATA_FILTER, DBA_KEY_ATTR_FILTER })
    {
        // If query has introduced a filter, then this filter can only give approximate results
        const char* our_filter = this->query.key_peek_value(k);
        const char* new_filter = query.key_peek_value(k);

        if (our_filter)
        {
            if (new_filter)
            {
                // If they are different, query can return more values than we know of
                if (strcmp(our_filter, new_filter) != 0)
                    return 0;
                // If they are the same, then nothing has changed
            } else {
                // Filter has been removed
                // (this is caught in the diff above, but I repeat the branch here
                // to avoid leaving loose ends in the code)
                return 0;
            }
        } else {
            if (new_filter)
            {
                // If the query introduced a new filter, then we can only return approximate results
                res = 1;
            } else {
                ; // Nothing to do: both queries do not contain this filter
            }
        }
    }

    // datetime extremes should be the same, or query should have more restrictive extremes
    static const auto undef = { MISSING_INT, MISSING_INT, MISSING_INT, MISSING_INT, MISSING_INT, MISSING_INT };
    Datetime our_min;
    Datetime our_max;
    Datetime new_min;
    Datetime new_max;
    this->query.parse_date_extremes(our_min, our_max);
    query.parse_date_extremes(new_min, new_max);
    if (Datetime::range_equals(our_min, our_max, new_min, new_max))
        ; // No change in datetime, good
    else if (Datetime::range_contains(our_min, our_max, new_min, new_max))
        // The query introduced further restrictions, but we can still return approximate results
        res = 1;
    else
        return 0;

    return res;
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
