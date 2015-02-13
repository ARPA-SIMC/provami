#include "provami/progressindicator.h"

namespace provami {

ProgressIndicator::ProgressIndicator(QObject *parent) :
    QObject(parent)
{
}

void ProgressIndicator::update_progress(QString task, QString progress)
{
#if 0
    auto i = tasks.find(task);
    if (i == tasks.end())
    {
        if (progress.isNull())
            return;
        tasks.insert(make_pair(task, Task(progress)));
    } else {
        if (progress.isNull())
        {
            // End task
        } else {
            // Update progress
            /*
            void ProvamiMainWindow::on_progress(QString task, QString progress)
            {
                if (progress.isNull() || progress.isEmpty())
                    statusBar()->clearMessage();
                else
                    statusBar()->showMessage(progress);
            }*/
        }
    }
#endif
    if (!status_bar) return;

    if (progress.isNull() || progress.isEmpty())
        status_bar->clearMessage();
    else
        status_bar->showMessage(progress);
}

}
