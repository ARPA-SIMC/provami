#include "provami/refreshthread.h"
#include <dballe/core/query.h>
#include <dballe/db/db.h>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

using namespace std;
using namespace dballe;

namespace provami {

static db::CursorData* do_refresh_data(DB* db, Query* q)
{
    return db->query_data(*q).release();
}

PendingDataRequest::PendingDataRequest(dballe::DB* db, std::unique_ptr<dballe::Query>&& query, const QObject* receiver, const char* method)
    : PendingRequest(receiver, method), query(query.release())
{
    future = QtConcurrent::run(do_refresh_data, db, this->query);
    future_watcher.setFuture(future);
}

PendingDataRequest::~PendingDataRequest()
{
    delete query;
}

static db::CursorSummary* do_refresh_summary(DB* db, Query* q)
{
    return db->query_summary(*q).release();
}

PendingSummaryRequest::PendingSummaryRequest(dballe::DB* db, std::unique_ptr<dballe::Query>&& query, const QObject* receiver, const char* method)
    : PendingRequest(receiver, method), query(query.release())
{
    future = QtConcurrent::run(do_refresh_summary, db, this->query);
    future_watcher.setFuture(future);
}

PendingSummaryRequest::~PendingSummaryRequest()
{
    delete query;
}

}
