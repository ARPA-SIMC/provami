#ifndef FILTERS_H
#define FILTERS_H

#include <QAbstractListModel>
#include <dballe/types.h>
#include <wreport/varinfo.h>
#include <set>

namespace dballe {
class Query;
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
    virtual ITEM from_record(const dballe::Query& rec) const = 0;
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
    std::string from_record(const dballe::Query& rec) const override;
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
    dballe::Level from_record(const dballe::Query& rec) const override;
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
    dballe::Trange from_record(const dballe::Query& rec) const override;
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
    wreport::Varcode from_record(const dballe::Query& rec) const override;
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
    std::string from_record(const dballe::Query& rec) const override;
    virtual void filter_select(const std::string& val);
    virtual void filter_unselect();
    virtual QVariant item_to_table_cell(const std::string& val) const;
    virtual const char* debug_tag() { return "report"; }

public:
    explicit FilterIdentModel(Model& model, QObject* parent=0);
};

};

#endif // FILTERS_H
