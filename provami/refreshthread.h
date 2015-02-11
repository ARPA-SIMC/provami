#ifndef REFRESHTHREAD_H
#define REFRESHTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <memory>
#include <dballe/core/record.h>

namespace dballe {
struct DB;
namespace db {
struct Cursor;
}
}

namespace provami {

class RefreshThread : public QThread
{
    Q_OBJECT

protected:
    QMutex mutex;
    QWaitCondition condition;

    /*
     * Set to nonzero to send a query object to the worker thread.
     * The worker thread will take care of deallocating the object
     * after it has processed it.
     */
    dballe::Query* pending_summary_query = 0;
    dballe::Query* pending_data_query = 0;

    // Set when the worker thread should quit
    bool do_quit = false;

    void run();

public:
    ~RefreshThread();

    dballe::DB* db = 0;
    std::unique_ptr<dballe::db::Cursor> cur_summary;
    std::unique_ptr<dballe::db::Cursor> cur_data;

    void query_summary(const dballe::Query& query, bool want_details);
    void query_data(const dballe::Query& query);

signals:
    void have_new_summary(bool with_details);
    void have_new_data();
};

}

#endif // REFRESHTHREAD_H
