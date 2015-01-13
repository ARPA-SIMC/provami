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

    // Takes ownership of i, passing it to 'scene'
    void add_coastline_path(const QPainterPath& p);

public:
    QGraphicsScene scene;

    explicit MapScene(Model& model, QObject *parent = 0);
    
    void load_coastlines(const std::string& fname);

    void to_proj(QPointF& point);
    void to_latlon(QPointF& point);

signals:
    
public slots:
    void update_stations();
    
};

#endif // MAPSCENE_H
