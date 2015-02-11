#include "provami/refreshthread.h"
#include <dballe/db/db.h>
#include <QDebug>

using namespace std;
using namespace dballe;

namespace provami {

RefreshThread::~RefreshThread()
{
    mutex.lock();
    do_quit = true;
    condition.wakeOne();
    mutex.unlock();
    wait();
}

// RAII mutex lock
struct MutexLock
{
    QMutex& mutex;
    MutexLock(QMutex& mutex) : mutex(mutex)
    {
        mutex.lock();
    }
    ~MutexLock()
    {
        mutex.unlock();
    }
    void wait(QWaitCondition& cond)
    {
        cond.wait(&mutex);
    }
};

void RefreshThread::run()
{
    qDebug() << "worker: start thread";
    while (true)
    {
        unique_ptr<dballe::Query> sum_query;
        unique_ptr<dballe::Query> data_query;
        // Check if an abort is requested
        {
            bool has_work = false;
            MutexLock checkQueue(mutex);
            if (do_quit) break;
            if (pending_summary_query)
            {
                sum_query.reset(pending_summary_query);
                pending_summary_query = 0;
                has_work = true;
            }
            if (pending_data_query)
            {
                data_query.reset(pending_data_query);
                pending_data_query = 0;
                has_work = true;
            }

            if (!has_work)
            {
                qDebug() << "worker: no more work, wait for some";
                checkQueue.wait(condition);
                continue;
            }
        }

        qDebug() << "worker: Wakeup refresh thread: summary: " << (sum_query.get() != 0)
                 << " data: " << (data_query.get() != 0);

        if (sum_query)
        {
            qDebug() << "worker: starting summary query";
            const char* query_request = sum_query->key_peek_value(DBA_KEY_QUERY);
            bool with_details = query_request ? strcmp(query_request, "details") == 0 : false;
            {
                MutexLock lock(mutex);
                qDebug() << "worker: starting dballe summary query";
                cur_summary = db->query_summary(*sum_query);
            }
            qDebug() << "worker: notifying summary query";
            emit have_new_summary(with_details);
            qDebug() << "worker: done summary query";
        }

        if (data_query)
        {
            qDebug() << "worker: starting data query";
            {
                MutexLock lock(mutex);
                qDebug() << "worker: starting dballe data query";
                cur_data = db->query_data(*data_query);
            }
            qDebug() << "worker: notifying data query";
            emit have_new_data();
            qDebug() << "worker: done data query";
        }
    }
    qDebug() << "worker: end thread";
}

void RefreshThread::query_summary(const Query &query, bool want_details)
{
    qDebug("query summary requested");
    // Make a copy to pass to pending_summary_query
    unique_ptr<Query> q(new Query(query));

    // If the active filter is empty, request all details
    if (want_details) q->set(DBA_KEY_QUERY, "details");

    // Enqueue the job for the worker thread
    MutexLock lock(mutex);
    delete pending_summary_query;
    pending_summary_query = q.release();
    condition.wakeOne();
}

void RefreshThread::query_data(const Query &query)
{
    qDebug("query data requested");
    // Make a copy to pass to pending_data_query
    unique_ptr<Query> q(new Query(query));

    // Enqueue the job for the worker thread
    MutexLock lock(mutex);
    delete pending_data_query;
    pending_data_query = q.release();
    condition.wakeOne();
}

}
