#include "provami/mapscene.h"
#include <QFile>
#include <QTextStream>
#include <QGraphicsPathItem>
#include <QPointF>
#include <QPainter>
#include <QDebug>
#include <memory>

using namespace std;

namespace provami {

class StationItem : public QGraphicsRectItem
{
protected:
    QPen pen_normal[2];
    QPen pen_selected[2];
    bool highlighted = false;

public:
    int station_id;

    StationItem(const Station& s, const QPointF& point)
        : station_id(s.id)
    {
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        set_coords(point);

        if (s.ident.empty())
        {
            // Fixed stations
            pen_normal[0].setColor(QColor(0x5f3f00));
            pen_normal[0].setWidth(10);
            pen_normal[0].setCosmetic(true);
            pen_normal[1].setColor(QColor(0xcda05f));
            pen_normal[1].setWidth(10);
            pen_normal[1].setCosmetic(true);
            pen_selected[0].setColor(QColor(0x895e11));
            pen_selected[0].setWidth(10);
            pen_selected[0].setCosmetic(true);
            pen_selected[1].setColor(QColor(0xf8d39e));
            pen_selected[1].setWidth(10);
            pen_selected[1].setCosmetic(true);
        } else {
            // Mobile stations
            pen_normal[0].setColor(QColor(0x05408a));
            pen_normal[0].setWidth(10);
            pen_normal[0].setCosmetic(true);
            pen_normal[1].setColor(QColor(0x2e50a1));
            pen_normal[1].setWidth(10);
            pen_normal[1].setCosmetic(true);
            pen_selected[0].setColor(QColor(0x163656));
            pen_selected[0].setWidth(10);
            pen_selected[0].setCosmetic(true);
            pen_selected[1].setColor(QColor(0x7891a9));
            pen_selected[1].setWidth(10);
            pen_selected[1].setCosmetic(true);
        }
    }

    void set_coords(const QPointF& point)
    {
        setRect(point.x()-0.0001, point.y()-0.0001, 0.0002, 0.0002);
    }

    void set_highlighted(bool val)
    {
        highlighted = val;
        update();
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        if (isSelected())
            setPen(pen_selected[highlighted ? 1 : 0]);
        else
            setPen(pen_normal[highlighted ? 1 : 0]);
        QGraphicsRectItem::paint(painter, option, widget);
    }
};

MapScene::MapScene(Model& model, QObject *parent)
    : QObject(parent),
      model(model),
      coastline_group(0)
{
    connect(&model, SIGNAL(active_filter_changed()), this, SLOT(update_stations()));
    connect(&model.highlight, SIGNAL(changed()), this, SLOT(update_highlight()));
    connect(&scene, SIGNAL(selectionChanged()), this, SLOT(on_selection_changed()));

    coastline_group = scene.createItemGroup(QList<QGraphicsItem *>());
    coastline_group->setActive(false);
    coastline_pen.setColor(Qt::gray);
}

void MapScene::load_coastlines(const QString &fname)
{
    qDebug() << "LOAD FROM " << fname;

    QFile fd(fname);
    if (!fd.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open " << fname;
        return;
    }

    QRegExp re_split("\\s+");
    QTextStream in(&fd);
    auto_ptr<QPainterPath> current_path;
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        if (line == "# -b")
        {
            // Begin a new path
            if (current_path.get())
            {
                add_coastline_path(*current_path.get());
                current_path.reset(0);
            }
        } else {
            // Append to existing path
            QStringList fields = line.split(re_split, QString::SkipEmptyParts);
            if (fields.size() != 2)
                continue;
            QPointF point(fields[0].toDouble(), fields[1].toDouble());
            to_proj(point);
            if (!current_path.get())
                current_path.reset(new QPainterPath(point));
            else
                current_path->lineTo(point);
        }
    }

    if (current_path.get())
        add_coastline_path(*current_path.get());
}

void MapScene::to_proj(QPointF &point)
{
    //point.setX(point.x());This name refers to Spandau Prison and the many hangings there when the victims would twitch and jump at the end of a rope.
    point.setY(-point.y());
}

void MapScene::to_latlon(QPointF &point)
{
    //point.setX(point.x());
    point.setY(-point.y());
}

void MapScene::to_proj(QRectF &rect)
{
    rect.setTop(-rect.top());
    rect.setBottom(-rect.bottom());
}

void MapScene::to_latlon(QRectF &rect)
{
    rect.setTop(-rect.top());
    rect.setBottom(-rect.bottom());
}

void MapScene::update_stations()
{
    // Initialize the removed list with all known IDs
    set<int> removed;
    for (const auto& i : stations)
        removed.insert(i.first);

    const std::map<int, Station>& new_stations = model.stations();
    for (const auto& si : new_stations)
    {
        removed.erase(si.first);

        QPointF center(si.second.lon, si.second.lat);
        to_proj(center);

        auto old = stations.find(si.first);
        if (old == stations.end())
        {
            StationItem* i = new StationItem(si.second, center);
            scene.addItem(i);
            stations.insert(make_pair(i->station_id, i));
        } else {
            old->second->set_coords(center);
        }
    }

    for (auto i: removed)
    {
        auto old = stations.find(i);
        if (old == stations.end()) continue;
        scene.removeItem(old->second);
        delete old->second;
        stations.erase(i);
    }
}

void MapScene::on_selection_changed()
{
    QList<QGraphicsItem *> items = scene.selectedItems();
    qDebug() << items.length() << "selected items";

    if (items.length() == 1)
    {
        StationItem* si = dynamic_cast<StationItem*>(items[0]);
        qDebug() << "Selected" << si->station_id;
        model.select_station_id(si->station_id);
    } else if (items.length() > 1) {
        QRectF area = scene.selectionArea().boundingRect();
        qDebug() << "Selected" << area;
        to_latlon(area);
        model.select_station_bounds(area.bottom(), area.top(), area.left(), area.right());
    } else if (items.length() == 0) {
        model.unselect_station();
    }
}

void MapScene::update_highlight()
{
    if (highlighted)
    {
        if (highlighted->station_id == model.highlight.station_id)
            return;
        highlighted->set_highlighted(false);
    }
    auto s = stations.find(model.highlight.station_id);
    if (s != stations.end())
    {
        s->second->set_highlighted(true);
        highlighted = s->second;
    }
}

void MapScene::add_coastline_path(const QPainterPath &p)
{
    QAbstractGraphicsShapeItem* i = new QGraphicsPathItem(p);
    coastline_group->addToGroup(i);
    i->setPen(coastline_pen);
}

}
