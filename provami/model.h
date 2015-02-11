#ifndef PROVAMI_MODEL_H
#define PROVAMI_MODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <dballe/core/defs.h>
#include <dballe/core/record.h>
#include <dballe/db/db.h>

#include <string>
#include <map>
#include <set>
#include <vector>
#include <QAction>
#include <provami/types.h>
#include <provami/highlight.h>

namespace dballe {
class Record;
}

namespace provami {
class Model;

// Base class with common signals and slots for all subclasses
// This allows us to use a template as a child class.
// See http://doc.qt.digia.com/qq/qq15-academic.html
class FilterModelQObjectBase : public QAbstractListModel
{
    Q_OBJECT

protected:
    Model& model;

public:
    explicit FilterModelQObjectBase(Model& model, QObject *parent=0);
    Model& get_model() { return model; }
    const Model& get_model() const { return model; }
    virtual int get_current_index_from_model() = 0;
    /// String tag used to identify this model in debug messages
    virtual const char* debug_tag() = 0;

public slots:
    virtual void set_next_filter(int index) = 0;
};

template<class ITEM>
class FilterModelBase : public FilterModelQObjectBase
{
protected:
    std::vector<ITEM> items;

    /// Read this filter item from a filter record
    virtual ITEM from_record(const dballe::Record& rec) const = 0;
    /// Read the currently selected ITEM from the model
    virtual ITEM from_model() const;
    /// Read the currently active ITEM from the model
    virtual ITEM active_from_model() const;
    /// Select the given item in the model
    virtual void filter_select(const ITEM& val) = 0;
    /// Unselect the filter we control in the model
    virtual void filter_unselect() = 0;
    /// Convert an item to a QVariant used as DisplayRole data
    virtual QVariant item_to_table_cell(const ITEM& val) const = 0;

public:
    explicit FilterModelBase(Model& model, QObject *parent=0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    /// Try selecting this item, if it is available in the model
    void select(const ITEM& item);
    bool has_item(const ITEM& item);
    void set_items(std::set<ITEM>& new_items);
    virtual void set_next_filter(int index);
    virtual int get_current_index_from_model();
};

class FilterReportModel : public FilterModelBase<std::string>
{
protected:
    virtual std::string from_record(const dballe::Record& rec) const;
    virtual void filter_select(const std::string& val);
    virtual void filter_unselect();
    virtual QVariant item_to_table_cell(const std::string& val) const;
    virtual const char* debug_tag() { return "report"; }

public:
    explicit FilterReportModel(Model& model, QObject* parent=0);
};

class FilterLevelModel : public FilterModelBase<dballe::Level>
{
protected:
    virtual dballe::Level from_record(const dballe::Record& rec) const;
    virtual void filter_select(const dballe::Level& val);
    virtual void filter_unselect();
    virtual QVariant item_to_table_cell(const dballe::Level &val) const;
    virtual const char* debug_tag() { return "level"; }

public:
    explicit FilterLevelModel(Model &model, QObject *parent=0);
};


class FilterTrangeModel : public FilterModelBase<dballe::Trange>
{
protected:
    virtual dballe::Trange from_record(const dballe::Record& rec) const;
    virtual void filter_select(const dballe::Trange& val);
    virtual void filter_unselect();
    virtual QVariant item_to_table_cell(const dballe::Trange &val) const;
    virtual const char* debug_tag() { return "trange"; }

public:
    explicit FilterTrangeModel(Model &model, QObject *parent = 0);
};

class FilterVarcodeModel : public FilterModelBase<wreport::Varcode>
{
protected:
    virtual wreport::Varcode from_record(const dballe::Record& rec) const;
    virtual void filter_select(const wreport::Varcode& val);
    virtual void filter_unselect();
    virtual QVariant item_to_table_cell(const wreport::Varcode &val) const;
    virtual const char* debug_tag() { return "varcode"; }

public:
    explicit FilterVarcodeModel(Model &model, QObject *parent = 0);
};

class FilterIdentModel : public FilterModelBase<std::string>
{
protected:
    virtual std::string from_record(const dballe::Record& rec) const;
    virtual void filter_select(const std::string& val);
    virtual void filter_unselect();
    virtual QVariant item_to_table_cell(const std::string& val) const;
    virtual const char* debug_tag() { return "report"; }

public:
    explicit FilterIdentModel(Model& model, QObject* parent=0);
};


class Model : public QObject
{
    Q_OBJECT

public slots:
    /// Reload data summary from the database
    void refresh(bool accurate=false);

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

signals:
    void next_filter_changed();
    void active_filter_changed();
    void begin_data_changed();
    void end_data_changed();

public:
    dballe::DB* db;

protected:    
    // Filtering elements
    std::map<int, Station> cache_stations;

    // Summary of items for the currently active filter
    std::map<SummaryKey, SummaryValue> cache_summary;
    // Sample values for the currently active filter
    std::vector<Value> cache_values;

    std::string m_dballe_url;

    /// Process the summary value regenerating the filtering elements lists
    void process_summary();

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

    // Last known minimum datetime for the data that we have
    dballe::Datetime dtmin;
    // Last known maximum datetime for the data that we have
    dballe::Datetime dtmax;
    // Last known count for the data that we have
    unsigned count;

    // Maximum number of results loaded on the results table
    unsigned limit = 100;

    Model();
    ~Model();

    const std::map<int, Station>& stations() const;
    const Station* station(int id) const;
    const std::map<SummaryKey, SummaryValue>& summaries() const;
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
