#ifndef PROVAMI_MAPSCENE_H
#define PROVAMI_MAPSCENE_H

#include <QObject>
#include <QGraphicsScene>
#include <string>
#include <map>
#include <provami/model.h>

namespace provami {

struct StationItem;

class MapScene : public QObject
{
    Q_OBJECT

protected:
    std::map<int, StationItem*> stations;
    Model& model;
    QPen coastline_pen;
    QGraphicsItemGroup* coastline_group;
    StationItem* highlighted = 0;

    // Takes ownership of i, passing it to 'scene'
    void add_coastline_path(const QPainterPath& p);

public:
    QGraphicsScene scene;

    explicit MapScene(Model& model, QObject *parent = 0);
    
    void load_coastlines(const QString& fname);

    void to_proj(QPointF& point);
    void to_proj(QRectF& rect);
    void to_latlon(QPointF& point);
    void to_latlon(QRectF& rect);

signals:
    
public slots:
    void update_stations();
    void on_selection_changed();
    void update_highlight();
};

}
#endif // MAPSCENE_H
