#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <dballe/core/defs.h>
#include <dballe/db/db.h>
#include <wreport/var.h>
#include <string>
#include <map>

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
    wreport::Varcode var;

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

signals:
    void refreshed();

protected:
    dballe::DB* db;
    std::map<int, Station> cache_stations;
    std::map<SummaryKey, SummaryValue> cache_summary;
    std::vector<Value> cache_values;

public:
    Model();
    ~Model();

    const std::map<int, Station>& stations() const;
    const std::map<SummaryKey, SummaryValue>& summary() const;
    const std::vector<Value>& values() const;

    /// Connect to a new database, possibly disconnecting from the previous one
    void dballe_connect(const std::string& dballe_url);
};

#endif // MODEL_H
