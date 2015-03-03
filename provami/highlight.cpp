#include "provami/highlight.h"

namespace provami {

Highlight::Highlight(QObject *parent) :
    QObject(parent)
{
}

void Highlight::select_value(const Value *val)
{
    m_value = val;
    m_station_value = 0;
    emit changed();
}

void Highlight::select_station_value(const StationValue *val)
{
    m_value = 0;
    m_station_value = val;
    emit changed();
}

void Highlight::reset()
{
    m_value = 0;
    m_station_value = 0;
    emit changed();
}

}
