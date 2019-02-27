#ifndef PROVAMI_MAPVIEW_H
#define PROVAMI_MAPVIEW_H

#include <provami/model.h>
#include <QWebView>

namespace provami {

/**
 * Show a world map with the stations on it, and allow to perform some interactive selection.
 *
 * This is implemented with a QWebView, with the actual map logic implemented in HTML5+JavaScript.
 */
class MapView : public QWebView
{
    Q_OBJECT

protected:
    Model* model = 0;
    bool _load_finished = false;
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
    void load_finished(bool ok);
    void update_stations();
    void station_selected(int id);
    void area_selected(double latmin, double latmax, double lonmin, double lonmax);
    void area_unselected();
    void update_highlight();
};

}

#endif // MAPVIEW_H
