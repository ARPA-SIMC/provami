#ifndef PROVAMI_HIGHLIGHT_H
#define PROVAMI_HIGHLIGHT_H

#include <QObject>
#include <dballe/core/defs.h>
#include <provami/types.h>

namespace provami {

class Highlight : public QObject
{
    Q_OBJECT

public:
    explicit Highlight(QObject *parent = 0);

    int station_id = dballe::MISSING_INT;
    int value_id = dballe::MISSING_INT;
    int station_var_id = dballe::MISSING_INT;

    void select_value(const Value*);
    void select_value(const StationValue*);
    void notify_changed();

signals:
    void changed();

public slots:

};

}

#endif // HIGHLIGHT_H
