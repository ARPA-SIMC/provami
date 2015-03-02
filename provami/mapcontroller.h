#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <QObject>

class MapController : public QObject
{
    Q_OBJECT
public:
    explicit MapController(QObject *parent = 0);

signals:
    void station_selected(int id);
    void area_selected(double latmin, double latmax, double lonmin, double lonmax);
    void area_unselected();

public slots:
    void js_station_selected(int id);
    void js_area_selected(double latmin, double latmax, double lonmin, double lonmax);
    void js_area_unselected();
};

#endif // MAPCONTROLLER_H
