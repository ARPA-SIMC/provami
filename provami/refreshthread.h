#ifndef REFRESHTHREAD_H
#define REFRESHTHREAD_H

//#include <QThread>
//#include <QMutex>
//#include <QWaitCondition>
#include <memory>
//#include <dballe/core/record.h>
//#include <dballe/core/query.h>
#include <QFutureWatcher>

namespace dballe {
struct DB;
struct Query;
namespace db {
struct CursorSummary;
struct CursorData;
}
}

namespace provami {

template<typename Result>
class PendingRequest
{
public:
    QFuture<Result> future;
    QFutureWatcher<Result> future_watcher;

    PendingRequest(const QObject* receiver, const char* method)
    {
        QObject::connect(&future_watcher, SIGNAL(finished()), receiver, method);
    }
};

class PendingDataRequest : public PendingRequest<dballe::db::CursorData*>
{
public:
    dballe::Query* query = nullptr;

    PendingDataRequest(dballe::DB* db, std::unique_ptr<dballe::Query>&& query, const QObject* receiver, const char* method);
    ~PendingDataRequest();
};

class PendingSummaryRequest : public PendingRequest<dballe::db::CursorSummary*>
{
public:
    dballe::Query* query = nullptr;

    PendingSummaryRequest(dballe::DB* db, std::unique_ptr<dballe::Query>&& query, const QObject* receiver, const char* method);
    ~PendingSummaryRequest();
};

}

#endif // REFRESHTHREAD_H
