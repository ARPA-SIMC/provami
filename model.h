#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <dballe/core/defs.h>
#include <dballe/core/record.h>
#include <dballe/db/db.h>
#include <wreport/var.h>
#include <string>
#include <map>
#include <set>
#include <vector>

namespace dballe {
class Record;
}

class Model;


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

public slots:
    virtual void set_next_filter(int index) = 0;
};

template<class ITEM>
class FilterModelBase : public FilterModelQObjectBase
{
protected:
    std::vector<ITEM> items;

    virtual void filter_select(const ITEM& val) = 0;
    virtual void filter_unselect() = 0;
    virtual QVariant item_to_table_cell(const ITEM& val) const = 0;

public:
    explicit FilterModelBase(Model& model, QObject *parent=0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    void set_items(std::set<ITEM>& new_items);
    virtual void set_next_filter(int index);
};

class FilterReportModel : public FilterModelBase<std::string>
{
protected:
    virtual void filter_select(const std::string& val);
    virtual void filter_unselect();
    virtual QVariant item_to_table_cell(const std::string& val) const;

public:
    explicit FilterReportModel(Model& model, QObject* parent=0);
};

class FilterLevelModel : public FilterModelBase<dballe::Level>
{
protected:
    virtual void filter_select(const dballe::Level& val);
    virtual void filter_unselect();
    virtual QVariant item_to_table_cell(const dballe::Level &val) const;

public:
    explicit FilterLevelModel(Model &model, QObject *parent=0);
};


class FilterTrangeModel : public FilterModelBase<dballe::Trange>
{
protected:
    virtual void filter_select(const dballe::Trange& val);
    virtual void filter_unselect();
    virtual QVariant item_to_table_cell(const dballe::Trange &val) const;

public:
    explicit FilterTrangeModel(Model &model, QObject *parent = 0);
};

class FilterVarcodeModel : public FilterModelBase<wreport::Varcode>
{
protected:
    virtual void filter_select(const wreport::Varcode& val);
    virtual void filter_unselect();
    virtual QVariant item_to_table_cell(const wreport::Varcode &val) const;

public:
    explicit FilterVarcodeModel(Model &model, QObject *parent = 0);
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
    void unselect_report();
    void unselect_level();
    void unselect_trange();
    void unselect_varcode();

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

    // Summary of items for the currently active filter
    std::map<SummaryKey, SummaryValue> cache_summary;
    // Sample values for the currently active filter
    std::vector<Value> cache_values;

    /// Process the summary value regenerating the filtering elements lists
    void process_summary();

public:
    FilterReportModel reports;
    FilterLevelModel levels;
    FilterTrangeModel tranges;
    FilterVarcodeModel varcodes;

    Model();
    ~Model();

    const std::map<int, Station>& stations() const;
    const Station* station(int id) const;
    const std::map<SummaryKey, SummaryValue>& summaries() const;
    const std::vector<Value>& values() const;

    /// Connect to a new database, possibly disconnecting from the previous one
    void dballe_connect(const std::string& dballe_url);
};

#endif // MODEL_H
