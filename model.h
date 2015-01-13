#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <dballe/core/defs.h>
#include <dballe/core/record.h>
#include <dballe/db/db.h>
#include <wreport/var.h>
#include <string>
#include <map>
#include <vector>

namespace dballe {
class Record;
}

struct Station
{
    double lat;
    double lon;
    std::string ident;

protected:
    Station(const dballe::db::Cursor& cur);

    friend class Model;
};

struct SummaryKey
{
    int ana_id;
    std::string rep_memo;
    dballe::Level level;
    dballe::Trange trange;
    wreport::Varcode varcode;

    bool operator<(const SummaryKey& sk) const;

protected:
    SummaryKey(const dballe::db::Cursor& cur);

    friend class Model;
};

struct SummaryValue
{
    // TODO

protected:
    SummaryValue(const dballe::db::Cursor& cur);

    friend class Model;
};

struct Value
{
    int ana_id;
    std::string rep_memo;
    dballe::Level level;
    dballe::Trange trange;
    int date[6];
    wreport::Var var;

protected:
    Value(const dballe::db::Cursor& cur);

    friend class Model;
};

class Model : public QObject
{
    Q_OBJECT

public slots:
    /// Reload data summary from the database
    void refresh();

    void activate_next_filter();
    void select_report(const std::string& val);
    void select_level(const dballe::Level& val);
    void select_trange(const dballe::Trange& val);
    void select_varcode(wreport::Varcode val);

signals:
    void next_filter_changed();
    void active_filter_changed();
    void data_changed();

protected:
    dballe::DB* db;

    // Filter corresponding to the data currently shown
    dballe::Record active_filter;
    // Filter that is being edited
    dballe::Record next_filter;

    // Filtering elements
    std::map<int, Station> cache_stations;
    std::vector<std::string> cache_reports;
    std::vector<dballe::Level> cache_levels;
    std::vector<dballe::Trange> cache_tranges;
    std::vector<wreport::Varcode> cache_varcodes;

    // Summary of items for the currently active filter
    std::map<SummaryKey, SummaryValue> cache_summary;
    // Sample values for the currently active filter
    std::vector<Value> cache_values;

    /// Process the summary value regenerating the filtering elements lists
    void process_summary();

public:
    Model();
    ~Model();

    const std::map<int, Station>& stations() const;
    const Station* station(int id) const;
    const std::map<SummaryKey, SummaryValue>& summaries() const;
    const std::vector<Value>& values() const;
    const std::vector<std::string>& reports() const;
    const std::vector<dballe::Level>& levels() const;
    const std::vector<dballe::Trange>& tranges() const;
    const std::vector<wreport::Varcode>& varcodes() const;

    /// Connect to a new database, possibly disconnecting from the previous one
    void dballe_connect(const std::string& dballe_url);
};

#endif // MODEL_H
