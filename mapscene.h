#ifndef MAPSCENE_H
#define MAPSCENE_H

#include <QObject>
#include <QGraphicsScene>
#include <string>
#include "model.h"

class MapScene : public QObject
{
    Q_OBJECT

protected:
    Model& model;
    QPen coastline_pen;
    QGraphicsItemGroup* coastline_group;

    // Takes ownership of i, passing it to 'scene'
    void add_coastline_path(const QPainterPath& p);

public:
    QGraphicsScene scene;

    explicit MapScene(Model& model, QObject *parent = 0);
    
    void load_coastlines(const QString& fname);

    void to_proj(QPointF& point);
    void to_latlon(QPointF& point);

signals:
    
public slots:
    void update_stations();
    void on_selection_changed();
};

#endif // MAPSCENE_H
