#include "tests.h"
#include "provami/model.h"
#include <wibble/string.h>
#include <dballe/core/defs.h>
#include <QDebug>

using namespace std;
using namespace wibble;
using namespace wibble::tests;
using namespace dballe;
using namespace provami;

namespace {

// Set a record from a ", "-separated string of assignments
void set_record_from_string(Record& rec, const std::string& s)
{
    str::Split splitter(", ", s);
    for (str::Split::const_iterator i = splitter.begin(); i != splitter.end(); ++i)
        rec.set_from_string(i->c_str());
}

}

namespace tut {

struct model_shar
{
    model_shar()
    {
    }

    unique_ptr<DB> populate()
    {
        std::unique_ptr<DB> db = DB::connect_test();
        db->reset();

        string stations[] = {
            "lat=45.0, lon=11.0",
            "lat=46.0, lon=12.0",
            "lat=45.0, lon=11.0, ident=foo",
            "lat=45.0, lon=11.0, ident=bar",
        };
        string records[] = {
            "synop",
            "temp",
        };
        Datetime datetimes[] = {
            Datetime(2015, 1, 1, 0, 0, 0),
            Datetime(2015, 2, 1, 0, 0, 0),
        };

        Record rec;
        for (auto s: stations)
            for (auto r: records)
                for (auto d: datetimes)
                {
                    rec.clear();
                    set_record_from_string(rec, s);
                    rec.set(DBA_KEY_REP_MEMO, r);
                    rec.set(Level(1, 0));
                    rec.set(Trange::instant());
                    rec.set(d);
                    rec.add(newvar(WR_VAR(0, 12, 101), 280.0));

                    db->insert(rec, true, true);
                }

        return db;
    }
};

TESTGRP(model);


template<> template<>
void to::test<1>()
{
    qDebug() << "start";
    Model model;
    model.set_db(populate(), "test");
    model.refresh_thread.start();

    qDebug() << "have db";

    model.test_wait_for_refresh();

    qDebug() << "populated";

    wassert(actual(model.summary_datetime_min()) == Datetime(2015, 1, 1));
    wassert(actual(model.summary_datetime_max()) == Datetime(2015, 2, 1));
    wassert(actual(model.summary_count()) == 16);
    wassert(actual(model.stations().size()) == 4);
    wassert(actual(model.values().size()) == 16);

    qDebug() << "filter";

    model.select_report("temp");
    model.activate_next_filter(true);
    model.test_wait_for_refresh();

    wassert(actual(model.summary_datetime_min()) == Datetime(2015, 1, 1));
    wassert(actual(model.summary_datetime_max()) == Datetime(2015, 2, 1));
    wassert(actual(model.summary_count()) == 8);
    wassert(actual(model.values().size()) == 8);
    wassert(actual(model.stations().size()) == 4);

#if 0
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

    protected:
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
#endif
}

}
