#include "provami/highlight.h"

namespace provami {

Highlight::Highlight(QObject *parent) :
    QObject(parent)
{
}

void Highlight::notify_changed()
{
    emit changed();
}

}
