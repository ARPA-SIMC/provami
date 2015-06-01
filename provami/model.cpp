#include "provami/model.h"
#include <memory>
#include <dballe/db/db.h>
#include <set>
#include <algorithm>
#include <QDebug>
#include <QEventLoop>

using namespace std;
using namespace dballe;

namespace provami {

Model::Model()
    : db(0), active_filter(Query::create()), next_filter(Query::create()),
      reports(*this), levels(*this), tranges(*this), varcodes(*this), idents(*this)
{
    connect(&refresh_thread, SIGNAL(have_new_summary(dballe::core::Query, bool)), this, SLOT(on_have_new_summary(dballe::core::Query, bool)));
    connect(&refresh_thread, SIGNAL(have_new_data()), this, SLOT(on_have_new_data()));
}

Model::~Model()
{
    if (db) delete db;
}

static const dballe::Datetime missing_datetime;

const Datetime &Model::summary_datetime_min() const
{
    if (summaries.empty()) return missing_datetime;
    return summaries.top().datetime_min();
}

const Datetime &Model::summary_datetime_max() const
{
    if (summaries.empty()) return missing_datetime;
    return summaries.top().datetime_max();
}

unsigned Model::summary_count() const
{
    if (summaries.empty()) return dballe::MISSING_INT;
    return summaries.top().data_count();
}

const std::map<int, Station> &Model::stations() const
{
    return cache_stations;
}

const std::set<int> &Model::selected_stations() const
{
    return _selected_stations;
}

const Station *Model::station(int id) const
{
    std::map<int, Station>::const_iterator i = cache_stations.find(id);
    if (i == cache_stations.end())
        return 0;
    return &(i->second);
}

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
    Record change;
    change.set(DBA_KEY_ANA_ID, val.ana_id);
    change.set(DBA_KEY_REP_MEMO, val.rep_memo.c_str());
    change.set(val.level);
    change.set(val.trange);
    change.set(val.date);
    change.set(new_val);
    db->insert(change, true, false);
    val.var = new_val;
}

void Model::update(StationValue &val, const wreport::Var &new_val)
{
    Record change;
    change.set_ana_context();
    change.set(DBA_KEY_ANA_ID, val.ana_id);
    change.set(DBA_KEY_REP_MEMO, val.rep_memo.c_str());
    change.set(new_val);
    db->insert(change, true, false);
    val.var = new_val;
}

void Model::update(int var_id, wreport::Varcode var_related, const wreport::Var &new_val)
{
    Record change;
    change.set(new_val);
    db->attr_insert(var_id, var_related, change);
}

void Model::remove(const Value &val)
{
    emit begin_data_changed();
    auto change = Query::create();
    change->set("ana_id", val.ana_id);
    change->set("rep_memo", val.rep_memo);
    change->set_level(val.level);
    change->set_trange(val.trange);
    change->set_datetime_exact(val.date);
    change->set("var", format_code(val.var.code()));
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
    active_filter = rec.clone();
    next_filter = rec.clone();
}

void Model::dballe_connect(const std::string &dballe_url)
{
    if (db)
    {
        delete db;
        db = 0;
    }

    m_dballe_url = dballe_url;

    auto new_db = DB::connect_from_url(dballe_url.c_str());
    db = new_db.release();
    refresh_thread.db = db;

    refresh();
}

void Model::set_db(std::unique_ptr<DB> &&_db, const std::string& url)
{
    if (db)
    {
        delete db;
        db = 0;
    }

    m_dballe_url = url;

    db = _db.release();
    refresh_thread.db = db;

    refresh();
}

void Model::test_wait_for_refresh()
{
    // Wait for the model to get refreshed
    QEventLoop loop;
    QObject::connect(this, SIGNAL(active_filter_changed()), &loop, SLOT(quit()));
    QObject::connect(this, SIGNAL(end_data_changed()), &loop, SLOT(quit()));
    while (refreshing_stations || refreshing_data)
        loop.exec();
}

void Model::refresh(bool accurate)
{
    refresh_data();
    refresh_summary(accurate);
}

void Model::refresh_data()
{
    emit progress("data", "Loading data...");
    auto query = active_filter->clone();
    query->seti("limit", limit);
    refreshing_data = true;
    refresh_thread.query_data(*query);
}

void Model::on_have_new_data()
{
    emit progress("data", "Processing data...");

    refreshing_data = false;

    emit begin_data_changed();
    // Query data for the currently active filter
    cache_values.clear();
    while (refresh_thread.cur_data->next())
    {
        cache_values.push_back(Value(*refresh_thread.cur_data));
    }
    emit end_data_changed();

    emit progress("data");
}

void Model::refresh_summary(bool accurate)
{
    using namespace dballe::db::summary;

    emit progress("summary", "Loading summary...");

    if (summaries.empty())
    {
        emit progress("summary", "Loading initial summary from db...");
        refreshing_stations = true;
        refresh_thread.query_summary(*Query::create(), true);
        return;
    }

    Matcher matcher(*active_filter, cache_stations);
    auto supported = summaries.query(*active_filter, accurate, [&](const Entry& entry) {
        return matcher.match(entry);
    });

    if (supported == UNSUPPORTED || (supported == OVERESTIMATED && accurate))
    {
        emit progress("summary", "Loading summary from db...");
        refreshing_stations = true;
        // The best summary that we have do not support what we need: hit the database
        refresh_thread.query_summary(*active_filter, accurate);
    }
    else
    {
        // Recompute the available choices
        qDebug() << "No need to hit the database, summary collation started";
        process_summary();
        emit progress("summary");
        emit active_filter_changed();
    }

}

void Model::on_have_new_summary(core::Query query, bool with_details)
{
    emit progress("summary", "Processing summary...");
    qDebug() << "Refresh summary results arrived";

    refreshing_stations = false;

    db::Summary& s = summaries.push(query);

    cache_stations.clear();
    cache_values.clear();

    highlight.reset();

    while (refresh_thread.cur_summary->next())
    {
        int ana_id = refresh_thread.cur_summary->get_station_id();
        if (cache_stations.find(ana_id) == cache_stations.end())
            cache_stations.insert(make_pair(ana_id, Station(*refresh_thread.cur_summary)));

        s.add_summary(*refresh_thread.cur_summary, with_details);
    }

    // Recompute the available choices
    qDebug() << "Summary collation started";
    process_summary();
    emit progress("summary");
    emit active_filter_changed();
}

void Model::activate_next_filter(bool accurate)
{
    active_filter = next_filter->clone();
    refresh(accurate);
}

void Model::filter_top_summary(const Matcher &matcher, db::Summary &out) const
{
    summaries.top().iterate([&](const db::summary::Entry& entry) {
        if (matcher.match(entry))
            out.add_entry(entry);
        return true;
    });
}

void Model::mark_hidden_stations(const db::Summary &summary)
{
    for (auto& s: cache_stations)
        s.second.hidden = summary.all_stations.find(s.first) == summary.all_stations.end();
}

void Model::process_summary()
{
    next_filter->print(stderr);
    //qDebug() << "process_summary" << QString::fromStdString(next_filter.to_string());
    if (summaries.empty()) return;
    Matcher matcher(*next_filter, cache_stations);
    db::Summary temp(*next_filter);
    summaries.top().iterate([&](const db::summary::Entry& entry) {
        if (matcher.match(entry))
            temp.add_entry(entry);
        return true;
    });

    // Mark disappeared stations as hidden
    set<string> all_idents;
    if (matcher.has_flt_station)
    {
        auto subrec = next_filter->clone();
        subrec->unset("ana_id");
        subrec->unset("mobile");
        subrec->unset("ident");
        subrec->unset("lat");
        subrec->unset("lon");
        subrec->unset("latmax");
        subrec->unset("latmin");
        subrec->unset("lonmax");
        subrec->unset("lonmin");
        Matcher submatcher(*subrec, cache_stations);
        db::Summary sub(*subrec);
        filter_top_summary(submatcher, sub);
        mark_hidden_stations(sub);
        for (int s_id : sub.all_stations)
        {
            auto s = cache_stations.find(s_id);
            if (s != cache_stations.end() && !s->second.ident.empty())
                all_idents.insert(s->second.ident);
        }

        _selected_stations = temp.all_stations;
    } else {
        mark_hidden_stations(temp);
        for (int s_id : temp.all_stations)
        {
            auto s = cache_stations.find(s_id);
            if (s != cache_stations.end() && !s->second.ident.empty())
                all_idents.insert(s->second.ident);
        }
    }
    idents.set_items(all_idents);

    if (matcher.has_flt_rep_memo)
    {
        auto subrec = next_filter->clone();
        subrec->unset("rep_memo");
        Matcher submatcher(*subrec, cache_stations);
        db::Summary sub(*subrec);
        filter_top_summary(submatcher, sub);
        reports.set_items(sub.all_reports);
    }
    else
        reports.set_items(temp.all_reports);

    if (matcher.has_flt_level)
    {
        auto subrec = next_filter->clone();
        subrec->set_level(Level());
        Matcher submatcher(*subrec, cache_stations);
        db::Summary sub(*subrec);
        filter_top_summary(submatcher, sub);
        levels.set_items(sub.all_levels);
    } else
        levels.set_items(temp.all_levels);

    if (matcher.has_flt_trange)
    {
        auto subrec = next_filter->clone();
        subrec->set_trange(Trange());
        Matcher submatcher(*subrec, cache_stations);
        db::Summary sub(*subrec);
        filter_top_summary(submatcher, sub);
        tranges.set_items(sub.all_tranges);
    } else
        tranges.set_items(temp.all_tranges);

    if (matcher.has_flt_varcode)
    {
        auto subrec = next_filter->clone();;
        subrec->unset("var");
        Matcher submatcher(*subrec, cache_stations);
        db::Summary sub(*subrec);
        filter_top_summary(submatcher, sub);
        varcodes.set_items(sub.all_varcodes);
    } else
        varcodes.set_items(temp.all_varcodes);

    emit next_filter_changed();
}

void Model::select_report(const string &val)
{
    next_filter->set("rep_memo", val.c_str());
    process_summary();
}

void Model::select_level(const Level &val)
{
    next_filter->set_level(val);
    process_summary();
}

void Model::select_trange(const Trange &val)
{
    next_filter->set_trange(val);
    process_summary();
}

void Model::select_varcode(wreport::Varcode val)
{
    next_filter->set("var", wreport::varcode_format(val));
    process_summary();
}

void Model::select_datemin(const dballe::Datetime& val)
{
    Datetime dtmin, dtmax;
    next_filter->get_datetime_bounds(dtmin, dtmax);
    if (dtmin == val) return;
    next_filter->set_datetime_bounds(val, dtmax);
    process_summary();
}

void Model::select_datemax(const dballe::Datetime& val)
{
    Datetime dtmin, dtmax;
    next_filter->get_datetime_bounds(dtmin, dtmax);
    if (dtmax == val) return;
    next_filter->set_datetime_bounds(dtmin, val);
    process_summary();
}

void Model::select_station_id(int id)
{
    next_filter->unset("latmin");
    next_filter->unset("latmax");
    next_filter->unset("lonmin");
    next_filter->unset("lonmax");
    next_filter->unset("ident");
    next_filter->set("ana_id", id);
    process_summary();
}

void Model::select_station_bounds(double latmin, double latmax, double lonmin, double lonmax)
{
    if (latmin < -90) latmin = -90;
    if (latmax > 90) latmax = 90;
    if (lonmin < -180) lonmin = -180;
    if (lonmax > 180) lonmax = 180;

    next_filter->set("latmin", latmin);
    next_filter->set("latmax", latmax);
    next_filter->set("lonmin", lonmin);
    next_filter->set("lonmax", lonmax);
    next_filter->unset("ana_id");
    process_summary();
}

void Model::select_ident(const string &val)
{
    next_filter->set("ident", val.c_str());
    process_summary();
}

void Model::unselect_report()
{
    next_filter->unset("rep_memo");
    process_summary();
}

void Model::unselect_level()
{
    next_filter->set_level(Level());
    process_summary();
}

void Model::unselect_trange()
{
    next_filter->set_trange(Trange());
    process_summary();
}

void Model::unselect_varcode()
{
    next_filter->unset("var");
    process_summary();
}

void Model::unselect_datemin()
{
    Datetime dtmin, dtmax;
    next_filter->get_datetime_bounds(dtmin, dtmax);
    if (dtmin.is_missing()) return;
    next_filter->set_datetime_bounds(Datetime(), dtmax);
    process_summary();
}

void Model::unselect_datemax()
{
    Datetime dtmin, dtmax;
    next_filter->get_datetime_bounds(dtmin, dtmax);
    if (dtmax.is_missing()) return;
    next_filter->set_datetime_bounds(dtmin, Datetime());
    process_summary();
}

void Model::set_filter(const Query& new_filter)
{
    next_filter = new_filter.clone();
    process_summary();}

void Model::unselect_station()
{
    next_filter->unset("latmin");
    next_filter->unset("latmax");
    next_filter->unset("lonmin");
    next_filter->unset("lonmax");
    next_filter->unset("ident");
    next_filter->unset("ana_id");
    process_summary();
}

void Model::unselect_ident()
{
    next_filter->unset("ident");
    process_summary();
}


ModelAction::ModelAction(Model &model, QObject *parent)
    : QAction(parent), model(model)
{
    connect(this, SIGNAL(triggered()), this, SLOT(on_trigger()));
}

}
