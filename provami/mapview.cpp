#include "provami/mapview.h"
#include "provami/mapcontroller.h"
#include "provami/config.h"
#include <dballe/core/defs.h>
#include <QDebug>
#include <QWebFrame>

using namespace dballe;

namespace provami {

class MapPage : public QWebPage
{
public:
    MapController controller;

    MapPage(QObject* parent=0)
        : QWebPage(parent)
    {
        QWebFrame* frame = mainFrame();
        frame->addToJavaScriptWindowObject("provami", &controller);
    }

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
    QWebFrame* frame = page()->mainFrame();
    const std::map<int, Station>& new_stations = model->stations();
    for (const auto& si : new_stations)
    {
        set_stations += QString("[%1,%2,%3,%4],").arg(si.first).arg(si.second.lat).arg(si.second.lon).arg(si.second.hidden ? "true" : "false");
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

    QString select_stations = QString("set_selected_stations([%1]);").arg(id);
    run_javascript(select_stations);
}

void MapView::area_selected(double latmin, double latmax, double lonmin, double lonmax)
{
    qDebug() << "area selected" << latmin << latmax << lonmin << lonmax;

    QString select_stations("set_selected_stations([");

    unsigned count = 0;
    int selected_id = MISSING_INT;
    const std::map<int, Station>& new_stations = model->stations();
    for (const auto& si : new_stations)
    {
        if (si.second.lat < latmin || si.second.lat > latmax) continue;
        if (si.second.lon < lonmin || si.second.lon > lonmax) continue;
        qDebug() << "Found" << si.first << si.second.lat << si.second.lon;
        selected_id = si.first;

        if (count)
            select_stations += ",";
        select_stations += QString::number(si.first);

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

    select_stations += "]);";
    run_javascript(select_stations);
}

void MapView::area_unselected()
{
    qDebug() << "Unselect stations";
    model->unselect_station();
    run_javascript("set_selected_stations([]);");
}


void MapView::update_highlight()
{
    QString highlight_station = QString("set_highlighted_station(%1);").arg(model->highlight.station_id());
    run_javascript(highlight_station);
}

}
