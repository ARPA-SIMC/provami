#include "provami/mapview.h"
#include "provami/mapcontroller.h"
#include "provami/config.h"
#include <QDebug>
#include <QWebFrame>

using namespace dballe;

namespace provami {

/**
 * Web page with the map
 */
class MapPage : public QWebPage
{
public:
    MapController controller;

    MapPage(QObject* parent=0)
        : QWebPage(parent)
    {
        QWebFrame* frame = mainFrame();

        // Export the MapController instance to JavaScript
        frame->addToJavaScriptWindowObject("provami", &controller);
    }

    /// Called when console.log or similar is called by JavaScript in the page
    virtual void javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID)
    {
        qDebug() << QString("%1:%2: %3").arg(sourceID).arg(lineNumber).arg(message);
    }
};

MapView::MapView(QWidget *parent) :
    QWebView(parent)
{
    MapPage* page = new MapPage(this);
    setPage(page);

    connect(&page->controller, SIGNAL(station_selected(int)), this, SLOT(station_selected(int)));
    connect(&page->controller, SIGNAL(area_selected(double,double,double,double)), this, SLOT(area_selected(double, double, double, double)));
    connect(&page->controller, SIGNAL(area_unselected()), this, SLOT(area_unselected()));

    // Disable context menu (see #23)
    setContextMenuPolicy(Qt::NoContextMenu);

    /*
    setInteractive(true);
    setDragMode(ScrollHandDrag);
    */

    //ui->map->setMapThemeId("earth/openstreetmap/openstreetmap.dgml");
    auto cfg = Config::get();
    load(QUrl("file:" + cfg.data_dir.absoluteFilePath("mapview/index.html")));
}

void MapView::run_javascript(QString code)
{
    //qDebug() << "JSRUN" << code;
    QWebFrame* frame = page()->mainFrame();
    frame->evaluateJavaScript(code);
}

void MapView::set_model(Model &model)
{
    if (this->model)
        disconnect(this->model, 0, this, 0);
    this->model = &model;
/*
    MapScene::MapScene(Model& model, QObject *parent)
        : QObject(parent),
          model(model),
          coastline_group(0)
    {
    */
    connect(&model, SIGNAL(next_filter_changed()), this, SLOT(update_stations()));
    connect(&model.highlight, SIGNAL(changed()), this, SLOT(update_highlight()));
}

QSize MapView::sizeHint() const
{
    return QSize(200, 200);
}


void MapView::update_stations()
{
    qDebug() << "update stations";
    QString set_stations("set_stations([");
    const auto& all_stations = model->explorer.global_summary().stations();
    const auto& cur_stations = model->explorer.active_summary().stations();
    for (const auto& si : all_stations)
    {
        bool selected = cur_stations.has(si.station);
        bool hidden = false; // FIXME: reimplement somehow?
        set_stations += QString("[%1,%2,%3,%4,%5],")
                .arg(si.station.id).arg(si.station.coords.dlat()).arg(si.station.coords.dlon())
                .arg(selected ? "true" : "false")
                .arg(hidden ? "true" : "false");
    }
    set_stations += "]);";
    run_javascript(set_stations);

    // TODO: update selection

    update_highlight();
}

void MapView::station_selected(int id)
{
    qDebug() << "station selected" << id;
    model->select_station_id(id);
}

void MapView::area_selected(double latmin, double latmax, double lonmin, double lonmax)
{
    qDebug() << "area selected" << latmin << latmax << lonmin << lonmax;

    unsigned count = 0;
    int selected_id = MISSING_INT;
    const auto& all_stations = model->explorer.global_summary().stations();
    for (const auto& si : all_stations)
    {
        if (si.station.coords.dlat() < latmin || si.station.coords.dlat() > latmax) continue;
        if (si.station.coords.dlon() < lonmin || si.station.coords.dlon() > lonmax) continue;
        qDebug() << "Found" << si.station.id << si.station.coords.dlat() << si.station.coords.dlon();
        selected_id = si.station.id;
        ++count;
    }

    switch (count)
    {
        case 0:
            // FIXME: rather than unselect (i.e. select all), select none
            qDebug() << "Unselect stations";
            model->unselect_station();
            break;
        case 1:
            qDebug() << "Select station" << selected_id;
            model->select_station_id(selected_id);
            break;
        default:
            qDebug() << "Select bounds" << latmin << latmax << lonmin << lonmax;
            model->select_station_bounds(latmin, latmax, lonmin, lonmax);
            break;
    }
}

void MapView::area_unselected()
{
    qDebug() << "Unselect stations";
    model->unselect_station();
}


void MapView::update_highlight()
{
    QString highlight_station = QString("set_highlighted_station(%1);").arg(model->highlight.station_id());
    run_javascript(highlight_station);
}

}
