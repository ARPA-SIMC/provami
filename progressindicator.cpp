#include "provami/progressindicator.h"
#include <QDebug>

using namespace std;

namespace provami {

void ProgressIndicator::Task::update(const QString &progress)
{
    if (!label) label = new QLabel();
    if (!visible)
    {
        sb.addWidget(label);
        label->show();
        visible = true;
    }
    label->setText(progress);
}

void ProgressIndicator::Task::clear()
{
    sb.removeWidget(label);
    visible = false;
}

ProgressIndicator::ProgressIndicator(QObject *parent) :
    QObject(parent)
{
}

ProgressIndicator::~ProgressIndicator()
{
    for (auto i: tasks)
        delete i.second;
}

void ProgressIndicator::update_progress(QString task, QString progress)
{
    if (!status_bar)
    {
        qDebug() << "progress " << task << ": no status bar";
        if (!tasks.empty())
            tasks.clear();
        return;
    }

    auto i = tasks.find(task);
    if (i == tasks.end())
    {
        if (progress.isNull())
        {
            qDebug() << "progress " << task << ": task ends without having started";
            return;
        }
        qDebug() << "progress " << task << ": new task: " << progress;
        Task* new_task;
        tasks.emplace(task, new_task = new Task(*status_bar));
        new_task->update(progress);
    } else {
        if (progress.isNull())
        {
            // End task
            qDebug() << "progress " << task << ": task ends";
            i->second->clear();
        } else {
            // Update progress
            qDebug() << "progress " << task << ": task update: " << progress;
            i->second->update(progress);
        }
    }
}

}
