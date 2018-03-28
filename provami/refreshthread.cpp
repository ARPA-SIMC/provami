#include "provami/refreshthread.h"
#include <dballe/core/query.h>
#include <dballe/db/db.h>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

using namespace std;
using namespace dballe;

namespace provami {

static db::CursorData* do_refresh_data(std::shared_ptr<dballe::db::Transaction> tr, Query* q)
{
    try {
        return tr->query_data(*q).release();
    } catch (std::exception& e) {
        fprintf(stderr, "do_refresh_data: %s\n", e.what());
        return nullptr;
    }
}

PendingDataRequest::PendingDataRequest(std::shared_ptr<dballe::db::Transaction> tr, std::unique_ptr<dballe::Query>&& query, const QObject* receiver, const char* method)
    : PendingRequest(receiver, method), query(query.release())
{
    future = QtConcurrent::run(do_refresh_data, tr, this->query);
    future_watcher.setFuture(future);
}

PendingDataRequest::~PendingDataRequest()
{
    delete query;
}

static db::CursorSummary* do_refresh_summary(std::shared_ptr<dballe::db::Transaction> tr, Query* q)
{
    try {
        return tr->query_summary(*q).release();
    } catch (std::exception& e) {
        fprintf(stderr, "do_refresh_data: %s\n", e.what());
        return nullptr;
    }
}

PendingSummaryRequest::PendingSummaryRequest(std::shared_ptr<dballe::db::Transaction> tr, std::unique_ptr<dballe::Query>&& query, const QObject* receiver, const char* method)
    : PendingRequest(receiver, method), query(query.release())
{
    future = QtConcurrent::run(do_refresh_summary, tr, this->query);
    future_watcher.setFuture(future);
}

PendingSummaryRequest::~PendingSummaryRequest()
{
    delete query;
}

}
