#ifndef PROVAMI_HIGHLIGHT_H
#define PROVAMI_HIGHLIGHT_H

#include <QObject>
#include <dballe/core/defs.h>
#include <provami/types.h>

namespace provami {

/**
 * Keep track of the currently selected station and value
 */
class Highlight : public QObject
{
    Q_OBJECT

protected:
    const Value* m_value = 0;
    const StationValue* m_station_value = 0;

public:
    explicit Highlight(QObject *parent = 0);

    int station_id() const
    {
        if (m_value) return m_value->ana_id;
        if (m_station_value) return m_station_value->ana_id;
        return dballe::MISSING_INT;
    }
    const wreport::Var* variable() const
    {
        if (m_value) return &(m_value->var);
        if (m_station_value) return &(m_station_value->var);
        return 0;
    }
    int value_id() const
    {
        if (m_value) return m_value->value_id;
        if (m_station_value) return m_station_value->value_id;
        return dballe::MISSING_INT;
    }
    //int station_var_id() const { return m_station_var_id; }

    /// Set the currently highlighted data
    void select_value(const Value* val);
    /// Set the currently highlighted station data
    void select_station_value(const StationValue* val);
    /// Reset highlights, for example after a full model change
    void reset();

signals:
    void changed();

public slots:

};

}

#endif // HIGHLIGHT_H
