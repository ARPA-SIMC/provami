#include "provami/mapcontroller.h"
#include <QDebug>

MapController::MapController(QObject *parent) :
    QObject(parent)
{
}

void MapController::area_selected(double latmin, double latmax, double lonmin, double lonmax)
{
    qDebug() << "area selected" << latmin << latmax << lonmin << lonmax;
}

void MapController::area_unselected()
{
    qDebug() << "area unselected";
}
