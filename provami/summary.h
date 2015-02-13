#ifndef SUMMARY_H
#define SUMMARY_H

#include <dballe/core/record.h>
#include <dballe/db/db.h>
#include <QObject>
#include <vector>

namespace provami {

class Summary;

struct SummaryEntry
{
    int ana_id;
    std::string rep_memo;
    dballe::Level level;
    dballe::Trange trange;
    wreport::Varcode varcode;
    dballe::Datetime datemin;
    dballe::Datetime datemax;
    int count = dballe::MISSING_INT;

    SummaryEntry(dballe::db::Cursor& cur, bool want_details);
};

class Summary : public QObject
{
    Q_OBJECT

protected:
    // Query that generated this summary
    dballe::Query query;

    // Summary of items for the currently active filter
    std::vector<SummaryEntry> summary;

    // True if the summary has been filled with data
    bool valid = false;

    // Last known minimum datetime for the data that we have
    dballe::Datetime dtmin;
    // Last known maximum datetime for the data that we have
    dballe::Datetime dtmax;
    // Last known count for the data that we have
    unsigned count;

public:
    explicit Summary(QObject *parent = 0);

    /// Return true if the summary has been filled with data
    bool is_valid() const { return valid; }

    const dballe::Datetime& datetime_min() const { return dtmin; }
    const dballe::Datetime& datetime_max() const { return dtmax; }
    unsigned data_count() const { return count; }

    /**
     * Checks if this summary correctly generate a
     * summary for the given query.
     *
     * Returns 0 if it cannot, 1 if the query may select less data
     * than this summary can estimate, 2 if the query selects data
     * that this summary can estimate exactly.
     */
    int supports(const dballe::Query& query) const;

    /// Reset the summary
    void reset(const dballe::Query& query);

    /// Add an entry to the summary taken from the current status of \a cur
    void add_summary(dballe::db::Cursor& cur, bool with_details);

    /// Iterate all values in the summary
    bool iterate(std::function<bool(const SummaryEntry&)> f) const;

signals:

public slots:

};

}

#endif // SUMMARY_H
