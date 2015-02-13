#ifndef PROGRESSINDICATOR_H
#define PROGRESSINDICATOR_H

#include <QObject>
#include <QStatusBar>
#include <map>

namespace provami {

class ProgressIndicator : public QObject
{
    Q_OBJECT

public:
    struct Task
    {
        QString name;
    };

protected:
    QStatusBar* status_bar = 0;
    std::map<QString, Task> tasks;

public:
    explicit ProgressIndicator(QObject *parent = 0);

    void set_statusbar(QStatusBar& sb)
    {
        status_bar = &sb;
    }

signals:

public slots:
    /**
     * Update progress information for the given task.
     *
     * If progress is a valid QString, it is used as a description
     * of what is happening in that task. Else, it signals the
     * end of a task.
     */
    void update_progress(QString task, QString progress);
};

}

#endif // PROGRESSINDICATOR_H
