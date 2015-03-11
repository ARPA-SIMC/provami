#ifndef PROVAMI_MODEL_H
#define PROVAMI_MODEL_H

#include <provami/types.h>
#include <provami/highlight.h>
#include <provami/refreshthread.h>
#include <provami/filters.h>
#include <QObject>
#include <dballe/core/defs.h>
#include <dballe/core/record.h>
#include <dballe/db/db.h>
#include <dballe/db/summary.h>
#include <string>
#include <map>
#include <vector>
#include <QAction>

namespace dballe {
class Record;
}

namespace provami {
class Model;
class RefreshThread;

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
    void unselect_station();
    void unselect_ident();
    void unselect_report();
    void unselect_level();
    void unselect_trange();
    void unselect_varcode();
    void unselect_datemin();
    void unselect_datemax();
    void set_filter(const dballe::Record& new_filter);
    void on_have_new_summary(dballe::Query query, bool with_details);
    void on_have_new_data();

signals:
    void next_filter_changed();
    void active_filter_changed();
    void begin_data_changed();
    void end_data_changed();
    void progress(QString task, QString progress=QString());

public:
    dballe::DB* db;
    RefreshThread refresh_thread;

protected:
    // True if there is a pending station refresh request
    bool refreshing_stations = false;
    // True if there is a pending data refresh request
    bool refreshing_data = false;

    // Filtering elements
    std::map<int, Station> cache_stations;

    /// Cached summary data
    dballe::db::summary::Stack summaries;

    // Sample values for the currently active filter
    std::vector<Value> cache_values;

    std::string m_dballe_url;

    /// Reload data summary from the database
    void refresh(bool accurate=false);

    /// Refresh the data selected by active_filter
    void refresh_data();

    /// Refresh the summary information selected by active_filter
    void refresh_summary(bool accurate=false);

    /// Process the summary value regenerating the filtering elements lists
    void process_summary();

    /// Filter the toplevel summary using a matcher, sending the results to out
    void filter_top_summary(const Matcher& matcher, dballe::db::Summary& out) const;

    /// Mark as hidden all the stations not present in summary
    void mark_hidden_stations(const dballe::db::Summary& summary);

public:
    // Current highlight
    Highlight highlight;
    // Filter corresponding to the data currently shown
    dballe::Record active_filter;
    // Filter that is being edited
    dballe::Record next_filter;

    FilterReportModel reports;
    FilterLevelModel levels;
    FilterTrangeModel tranges;
    FilterVarcodeModel varcodes;
    FilterIdentModel idents;

    // Maximum number of results loaded on the results table
    unsigned limit = 100;

    Model();
    ~Model();

    const dballe::Datetime& summary_datetime_min() const;
    const dballe::Datetime& summary_datetime_max() const;
    unsigned summary_count() const;

    const std::map<int, Station>& stations() const;
    const Station* station(int id) const;
    const std::vector<Value>& values() const;
    std::vector<Value>& values();

    const std::string& dballe_url() const { return m_dballe_url; }

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
    void set_initial_filter(const dballe::Record& rec);

    /// Connect to a new database, possibly disconnecting from the previous one
    void dballe_connect(const std::string& dballe_url);

    /// Take over an existing db
    void set_db(std::unique_ptr<dballe::DB>&& db, const std::string &url);

    /// Synchronously wait for the refresh to finish. Uses only for tests.
    void test_wait_for_refresh();
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
