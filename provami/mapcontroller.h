#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <QObject>

class MapController : public QObject
{
    Q_OBJECT
public:
    explicit MapController(QObject *parent = 0);

signals:

public slots:
    void area_selected(double latmin, double latmax, double lonmin, double lonmax);
    void area_unselected();
};

#endif // MAPCONTROLLER_H
