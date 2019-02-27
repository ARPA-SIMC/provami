#include "provami/model.h"
#include <memory>
#include <dballe/db/db.h>
#include <dballe/core/data.h>
#include <set>
#include <algorithm>
#include <QDebug>
#include <QEventLoop>

using namespace std;
using namespace dballe;

namespace provami {

Model::Model()
    : db(0), reports(*this), levels(*this), tranges(*this), varcodes(*this), idents(*this)
{
}

Model::~Model()
{
}

static const dballe::Datetime missing_datetime;

const Datetime &Model::summary_datetime_min() const
{
    return explorer.active_summary().datetime_min();
}

const Datetime &Model::summary_datetime_max() const
{
    return explorer.active_summary().datetime_max();
}

unsigned Model::summary_count() const
{
    return explorer.active_summary().data_count();
}

#if 0
const std::map<int, Station> &Model::stations() const
{
    return cache_stations;
}
#endif

#if 0
const Station *Model::station(int id) const
{
    std::map<int, Station>::const_iterator i = cache_stations.find(id);
    if (i == cache_stations.end())
        return 0;
    return &(i->second);
}
#endif

const std::vector<Value> &Model::values() const
{
    return cache_values;
}

std::vector<Value> &Model::values()
{
    return cache_values;
}

void Model::update(Value &val, const wreport::Var &new_val)
{
    core::Data data;
    data.station.id = val.ana_id;
    data.station.report = val.rep_memo;
    data.level = val.level;
    data.trange = val.trange;
    data.datetime = val.date;
    data.values.set(new_val);
    auto opts = DBInsertOptions::create();
    opts->can_replace = true;
    opts->can_add_stations = false;
    db->insert_data(data, *opts);
    val.var = new_val;
}

void Model::update(StationValue &val, const wreport::Var &new_val)
{
    core::Data data;
    data.station.id = val.ana_id;
    data.station.report = val.rep_memo;
    data.values.set(new_val);
    auto opts = DBInsertOptions::create();
    opts->can_replace = true;
    opts->can_add_stations = false;
    db->insert_station_data(data, *opts);
    val.var = new_val;
}

void Model::update(int var_id, wreport::Varcode var_related, const wreport::Var &new_val)
{
    dballe::Values values;
    values.set(new_val);
    dynamic_pointer_cast<db::DB>(db)->attr_insert_data(var_id, values);
}

void Model::remove(const Value &val)
{
    emit begin_data_changed();
    auto change = Query::create();
    core::Query::downcast(*change).ana_id = val.ana_id;
    core::Query::downcast(*change).report = val.rep_memo;
    change->set_level(val.level);
    change->set_trange(val.trange);
    change->set_datetimerange(DatetimeRange(val.date, val.date));
    core::Query::downcast(*change).varcodes.clear();
    core::Query::downcast(*change).varcodes.insert(val.var.code());
    db->remove_data(*change);
    vector<Value>::iterator i = std::find(cache_values.begin(), cache_values.end(), val);
    if (i != cache_values.end())
    {
        cache_values.erase(i);
    }
    emit end_data_changed();
}

void Model::set_initial_filter(const Query& rec)
{
    show_filter(rec);
}

void Model::dballe_connect(const std::string &dballe_url)
{
    if (db)
        db.reset();

    m_dballe_url = dballe_url;

    db = DB::connect_from_url(dballe_url.c_str());

    refresh();
}

void Model::set_db(std::shared_ptr<DB> db, const std::string& url)
{
    m_dballe_url = url;
    this->db = db;

    //refresh();
}

std::shared_ptr<dballe::Transaction> Model::get_refresh_transaction()
{
    if (!refresh_transaction.expired())
        return refresh_transaction.lock();
    auto res = db->transaction(true);
    refresh_transaction = res;
    return res;
}

void Model::refresh(bool accurate)
{
    auto tr = get_refresh_transaction();
    refresh_data(*tr);
    refresh_summary(*tr, accurate);
    tr->rollback();
}

void Model::refresh_data(dballe::Transaction& tr)
{
    emit progress("data", "Loading data...");
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.limit = limit;
    auto cur = tr.query_data(query);

    emit progress("data", "Processing data...");

    emit begin_data_changed();
    // Query data for the currently active filter
    cache_values.clear();
    while (cur->next())
        cache_values.push_back(Value(*cur));
    emit end_data_changed();

    emit progress("data");
}

void Model::refresh_summary(dballe::Transaction& tr, bool accurate)
{
    using namespace dballe::db::summary;

    emit progress("summary", "Loading summary...");

    {
        auto update = explorer.rebuild();
        update.add_db(*dynamic_cast<db::Transaction*>(&tr));
    }
    highlight.reset();
    explorer_to_fields();

    emit progress("summary");
    emit active_filter_changed();
}

void Model::activate_next_filter(bool accurate)
{
    refresh(accurate);
}

void Model::show_filter(const dballe::Query& filter)
{
    explorer.set_filter(filter);
    explorer_to_fields();
}

void Model::explorer_to_fields()
{
    const auto& s = explorer.active_summary();
    const auto& stations = s.stations();
    std::set<std::string> new_idents;
    for (const auto& se: stations)
    {
        if (se.station.ident)
            new_idents.insert(se.station.ident);
    }
    idents.set_items(new_idents);
    reports.set_items(s.reports());
    levels.set_items(s.levels());
    tranges.set_items(s.tranges());
    varcodes.set_items(s.varcodes());
    emit next_filter_changed();
}

void Model::select_report(const string &val)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.report = val;
    show_filter(query);
}

void Model::select_level(const Level &val)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.set_level(val);
    show_filter(query);
}

void Model::select_trange(const Trange &val)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.set_trange(val);
    show_filter(query);
}

void Model::select_varcode(wreport::Varcode val)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.varcodes.clear();
    query.varcodes.insert(val);
    show_filter(query);
}

void Model::select_datemin(const dballe::Datetime& val)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    DatetimeRange dtr = query.get_datetimerange();
    if (dtr.min == val) return;
    dtr.min = val;
    query.set_datetimerange(dtr);
    show_filter(query);
}

void Model::select_datemax(const dballe::Datetime& val)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    DatetimeRange dtr = query.get_datetimerange();
    if (dtr.max == val) return;
    dtr.max = val;
    query.set_datetimerange(dtr);
    show_filter(query);
}

void Model::select_station_id(int id)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.set_latrange(LatRange());
    query.set_lonrange(LonRange());
    query.ident.clear();
    query.ana_id = id;
    show_filter(query);
}

void Model::select_station_bounds(double latmin, double latmax, double lonmin, double lonmax)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    if (latmin < -90) latmin = -90;
    if (latmax > 90) latmax = 90;
    if (lonmin < -180) lonmin = -180;
    if (lonmax > 180) lonmax = 180;

    query.set_latrange(LatRange(latmin, latmax));
    query.set_lonrange(LonRange(lonmin, lonmax));
    query.ana_id = MISSING_INT;
    show_filter(query);
}

void Model::select_ident(const string &val)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.ident = val;
    query.ana_id = MISSING_INT;
    show_filter(query);
}

void Model::unselect_report()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.report.clear();
    show_filter(query);
}

void Model::unselect_level()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.set_level(Level());
    show_filter(query);
}

void Model::unselect_trange()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.set_trange(Trange());
    show_filter(query);
}

void Model::unselect_varcode()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.varcodes.clear();
    show_filter(query);
}

void Model::unselect_datemin()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    DatetimeRange dtr = query.get_datetimerange();
    if (dtr.min.is_missing()) return;
    dtr.min = Datetime();
    query.set_datetimerange(dtr);
    show_filter(query);
}

void Model::unselect_datemax()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    DatetimeRange dtr = query.get_datetimerange();
    if (dtr.max.is_missing()) return;
    dtr.max = Datetime();
    query.set_datetimerange(dtr);
    show_filter(query);
}

void Model::set_filter(const Query& new_filter)
{
    show_filter(new_filter);
}

void Model::unselect_station()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.set_latrange(LatRange());
    query.set_lonrange(LonRange());
    query.ident.clear();
    query.ana_id = MISSING_INT;
    show_filter(query);
}

void Model::unselect_ident()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.ident.clear();
    show_filter(query);
}


ModelAction::ModelAction(Model &model, QObject *parent)
    : QAction(parent), model(model)
{
    connect(this, SIGNAL(triggered()), this, SLOT(on_trigger()));
}

}
