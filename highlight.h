#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <QObject>
#include <dballe/core/defs.h>

class Highlight : public QObject
{
    Q_OBJECT

public:
    explicit Highlight(QObject *parent = 0);

    int station_id = dballe::MISSING_INT;
    int value_id = dballe::MISSING_INT;
    int station_var_id = dballe::MISSING_INT;

    void notify_changed();

signals:
    void changed();

public slots:

};

#endif // HIGHLIGHT_H
