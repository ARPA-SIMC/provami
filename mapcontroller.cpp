#include "provami/mapcontroller.h"
#include <algorithm>
#include <QDebug>

using namespace std;

MapController::MapController(QObject *parent) :
    QObject(parent)
{
}

void MapController::js_station_selected(int id)
{
    emit station_selected(id);
}

void MapController::js_area_selected(double latmin, double latmax, double lonmin, double lonmax)
{
    emit area_selected(
                min(latmin, latmax),
                max(latmin, latmax),
                min(lonmin, lonmax),
                max(lonmin, lonmax));
}

void MapController::js_area_unselected()
{
    emit area_unselected();
}
