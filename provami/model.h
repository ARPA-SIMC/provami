#ifndef PROVAMI_MODEL_H
#define PROVAMI_MODEL_H

#include <provami/types.h>
#include <provami/highlight.h>
#include <provami/filters.h>
#include <QObject>
#include <dballe/types.h>
#include <dballe/query.h>
#include <dballe/db.h>
#include <dballe/db/summary.h>
#include <dballe/db/explorer.h>
#include <string>
#include <map>
#include <vector>
#include <QAction>
#include <QFutureWatcher>

namespace provami {
class Model;

class Model : public QObject
{
    Q_OBJECT

public slots:
    void activate_next_filter(bool accurate=false);
    void select_station_id(int id);
    void select_station_bounds(double latmin, double latmax, double lonmin, double lonmax);
    void select_ident(const std::string& val);
    void select_report(const std::string& val);
    void select_level(const dballe::Level& val);
    void select_trange(const dballe::Trange& val);
    void select_varcode(wreport::Varcode val);
    void select_datemin(const dballe::Datetime& val);
    void select_datemax(const dballe::Datetime& val);
    /// Reset lat, lon, ana_id and ident filters
    void unselect_station();
    /// Reset ident filter
    void unselect_ident();
    void unselect_report();
    void unselect_level();
    void unselect_trange();
    void unselect_varcode();
    void unselect_datemin();
    void unselect_datemax();
    void set_filter(const dballe::Query& new_filter);

signals:
    void next_filter_changed();
    void active_filter_changed();
    void begin_data_changed();
    void end_data_changed();
    void progress(QString task, QString progress=QString());

protected:
    std::shared_ptr<dballe::DB> db;

    /// Current transaction for refreshing values (if any)
    std::weak_ptr<dballe::Transaction> refresh_transaction;

    /// Currently selected stations
    /// Sample values for the currently active filter
    std::vector<Value> cache_values;

    std::string m_dballe_url;

    /// Reload data summary from the database
    void refresh(bool accurate=false);

    /// Refresh the data selected by active_filter
    void refresh_data(dballe::Transaction& tr);

    /// Refresh the summary information selected by active_filter
    void refresh_summary(dballe::Transaction& tr, bool accurate=false);

    /// Process the summary value regenerating the filtering elements lists
    //void process_summary();

    void on_have_new_summary(std::unique_ptr<dballe::CursorSummary>, const dballe::Query& query);

    void show_filter(const dballe::Query& filter);
    void explorer_to_fields();

public:
    // Explorer interface to the database
    dballe::db::DBExplorer explorer;
    // Current highlight
    Highlight highlight;

    FilterReportModel reports;
    FilterLevelModel levels;
    FilterTrangeModel tranges;
    FilterVarcodeModel varcodes;
    FilterIdentModel idents;

    // Maximum number of results loaded on the results table
    unsigned limit = 100;

    Model();
    Model(const Model&) = delete;
    Model(const Model&&) = delete;
    ~Model();
    Model& operator=(const Model&) = delete;
    Model& operator=(const Model&&) = delete;

    const dballe::Datetime& summary_datetime_min() const;
    const dballe::Datetime& summary_datetime_max() const;
    unsigned summary_count() const;

    //const std::map<int, Station>& stations() const;
    // const Station* station(int id) const;
    const std::vector<Value>& values() const;
    std::vector<Value>& values();

    const std::string& dballe_url() const { return m_dballe_url; }

    std::shared_ptr<dballe::Transaction> get_refresh_transaction();

    /**
     * Update \a val in the database to have the value \a new_val
     *
     * Updates the 'val' member of 'val' if it succeeded, otherwise
     * exceptions are raised
     */
    void update(Value& val, const wreport::Var& new_val);

    /**
     * Update \a val in the database to have the value \a new_val
     *
     * Updates the 'val' member of 'val' if it succeeded, otherwise
     * exceptions are raised
     */
    void update(StationValue& val, const wreport::Var& new_val);

    /**
     * Update an attribute
     */
    void update(int var_id, wreport::Varcode var_related, const wreport::Var& new_val);

    /// Remove the value from the database
    void remove(const Value& val);

    /// Set a filter before the initial connect
    void set_initial_filter(const dballe::Query& rec);

    /// Connect to a new database, possibly disconnecting from the previous one
    void dballe_connect(const std::string& dballe_url);

    /// Take over an existing db
    void set_db(std::shared_ptr<dballe::DB> db, const std::string &url);
};

class ModelAction : public QAction
{
    Q_OBJECT

protected:
    Model& model;

protected slots:
    virtual void on_trigger() = 0;

public:
    ModelAction(Model& model, QObject* parent=0);
};

}

#endif // MODEL_H
