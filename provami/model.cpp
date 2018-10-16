#include "provami/model.h"
#include <memory>
#include <dballe/db/db.h>
#include <dballe/core/values.h>
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

const std::set<int> &Model::selected_stations() const
{
    return _selected_stations;
}

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
    DataValues vals;
    vals.info.id = val.ana_id;
    vals.info.report = val.rep_memo;
    vals.info.level = val.level;
    vals.info.trange = val.trange;
    vals.info.datetime = val.date;
    vals.values.set(new_val);
    db->insert_data(vals, true, false);
    val.var = new_val;
}

void Model::update(StationValue &val, const wreport::Var &new_val)
{
    StationValues vals;
    vals.info.id = val.ana_id;
    vals.info.report = val.rep_memo;
    vals.values.set(new_val);
    db->insert_station_data(vals, true, false);
    val.var = new_val;
}

void Model::update(int var_id, wreport::Varcode var_related, const wreport::Var &new_val)
{
    Values values;
    values.set(new_val);
    db->attr_insert_data(var_id, values);
}

void Model::remove(const Value &val)
{
    emit begin_data_changed();
    auto change = Query::create();
    core::Query::downcast(*change).ana_id = val.ana_id;
    core::Query::downcast(*change).rep_memo = val.rep_memo;
    change->set_level(val.level);
    change->set_trange(val.trange);
    change->set_datetimerange(DatetimeRange(val.date, val.date));
    core::Query::downcast(*change).varcodes.clear();
    core::Query::downcast(*change).varcodes.insert(val.var.code());
    db->remove(*change);
    vector<Value>::iterator i = std::find(cache_values.begin(), cache_values.end(), val);
    if (i != cache_values.end())
    {
        cache_values.erase(i);
    }
    emit end_data_changed();
}

void Model::set_initial_filter(const Query& rec)
{
    explorer.set_filter(rec);
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

void Model::test_wait_for_refresh()
{
/*
    if (pending_query_data)
        pending_query_data->future_watcher.waitForFinished();
    if (pending_query_summary)
        pending_query_summary->future_watcher.waitForFinished();

    QEventLoop loop;
    loop.processEvents();
 */
}

std::shared_ptr<dballe::db::Transaction> Model::get_refresh_transaction()
{
    if (!refresh_transaction.expired())
        return refresh_transaction.lock();
    auto res = db->transaction(true);
    refresh_transaction = res;
    return res;
}

void Model::refresh(bool accurate)
{
    refresh_data();
    refresh_summary(accurate);
}

void Model::refresh_data()
{
    emit progress("data", "Loading data...");
    auto tr = get_refresh_transaction();
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.limit = limit;
    on_have_new_data(tr->query_data(query));
}

void Model::on_have_new_data(std::unique_ptr<db::CursorData> cur)
{
    emit progress("data", "Processing data...");

    emit begin_data_changed();
    // Query data for the currently active filter
    cache_values.clear();
    while (cur->next())
    {
        cache_values.push_back(Value(*cur));
    }
    emit end_data_changed();

    emit progress("data");
}

void Model::refresh_summary(bool accurate)
{
    using namespace dballe::db::summary;

    emit progress("summary", "Loading summary...");

    auto tr = get_refresh_transaction();
    explorer.revalidate(*tr);
    highlight.reset();

    emit progress("summary");
    emit active_filter_changed();
}

void Model::activate_next_filter(bool accurate)
{
    refresh(accurate);
}

#if 0
void Model::mark_hidden_stations(const db::Summary &summary)
{
    for (auto& s: cache_stations)
        s.second.hidden = summary.all_stations.find(s.first) == summary.all_stations.end();
}
#endif

void Model::select_report(const string &val)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.rep_memo = val;
    explorer.set_filter(query);
}

void Model::select_level(const Level &val)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.set_level(val);
    explorer.set_filter(query);
}

void Model::select_trange(const Trange &val)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.set_trange(val);
    explorer.set_filter(query);
}

void Model::select_varcode(wreport::Varcode val)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.varcodes.clear();
    query.varcodes.insert(val);
    explorer.set_filter(query);
}

void Model::select_datemin(const dballe::Datetime& val)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    DatetimeRange dtr = query.get_datetimerange();
    if (dtr.min == val) return;
    dtr.min = val;
    query.set_datetimerange(dtr);
    explorer.set_filter(query);
}

void Model::select_datemax(const dballe::Datetime& val)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    DatetimeRange dtr = query.get_datetimerange();
    if (dtr.max == val) return;
    dtr.max = val;
    query.set_datetimerange(dtr);
    explorer.set_filter(query);
}

void Model::select_station_id(int id)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.set_latrange(LatRange());
    query.set_lonrange(LonRange());
    query.ident.clear();
    query.ana_id = id;
    explorer.set_filter(query);
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
    explorer.set_filter(query);
}

void Model::select_ident(const string &val)
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.ident = val;
    query.ana_id = MISSING_INT;
    explorer.set_filter(query);
}

void Model::unselect_report()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.rep_memo.clear();
    explorer.set_filter(query);
}

void Model::unselect_level()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.set_level(Level());
    explorer.set_filter(query);
}

void Model::unselect_trange()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.set_trange(Trange());
    explorer.set_filter(query);
}

void Model::unselect_varcode()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.varcodes.clear();
    explorer.set_filter(query);
}

void Model::unselect_datemin()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    DatetimeRange dtr = query.get_datetimerange();
    if (dtr.min.is_missing()) return;
    dtr.min = Datetime();
    query.set_datetimerange(dtr);
    explorer.set_filter(query);
}

void Model::unselect_datemax()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    DatetimeRange dtr = query.get_datetimerange();
    if (dtr.max.is_missing()) return;
    dtr.max = Datetime();
    query.set_datetimerange(dtr);
    explorer.set_filter(query);
}

void Model::set_filter(const Query& new_filter)
{
    explorer.set_filter(new_filter);
}

void Model::unselect_station()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.set_latrange(LatRange());
    query.set_lonrange(LonRange());
    query.ident.clear();
    query.ana_id = MISSING_INT;
    explorer.set_filter(query);
}

void Model::unselect_ident()
{
    core::Query query(core::Query::downcast(explorer.get_filter()));
    query.ident.clear();
    explorer.set_filter(query);
}


ModelAction::ModelAction(Model &model, QObject *parent)
    : QAction(parent), model(model)
{
    connect(this, SIGNAL(triggered()), this, SLOT(on_trigger()));
}

}
