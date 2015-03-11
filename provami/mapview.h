#ifndef PROVAMI_MAPVIEW_H
#define PROVAMI_MAPVIEW_H

#include <provami/model.h>
#include <QWebView>

namespace provami {

class MapView : public QWebView
{
    Q_OBJECT

protected:
    Model* model = 0;
    /*
    // Implement wheel zooming
    virtual void wheelEvent(QWheelEvent* event);

    // Selection on shift+drag
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    */

    // Run this javascript code in the current page
    void run_javascript(QString code);

public:
    explicit MapView(QWidget *parent = 0);

    void set_model(Model& model);
    QSize sizeHint() const override;

signals:
    
public slots:
    void update_stations();
    void station_selected(int id);
    void area_selected(double latmin, double latmax, double lonmin, double lonmax);
    void area_unselected();
    void update_highlight();
};

#if 0
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
#endif
}

#endif // MAPVIEW_H
