#include "highlight.h"

Highlight::Highlight(QObject *parent) :
    QObject(parent)
{
}

void Highlight::notify_changed()
{
    emit changed();
}
