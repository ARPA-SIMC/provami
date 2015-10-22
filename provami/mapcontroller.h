#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <QObject>

class MapController : public QObject
{
    Q_OBJECT
public:
    explicit MapController(QObject *parent = 0);

signals:
    // Called when a station has been selected on the map view
    void station_selected(int id);

    // Called when an area has been selected on the map view
    void area_selected(double latmin, double latmax, double lonmin, double lonmax);

    // Called when the area selection has been cleared in the map view
    void area_unselected();

public slots:
    // js_* slots are intended to be called by JavaScript

    void js_station_selected(int id);
    void js_area_selected(double latmin, double latmax, double lonmin, double lonmax);
    void js_area_unselected();
};

#endif // MAPCONTROLLER_H
