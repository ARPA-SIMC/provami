#include "mapscene.h"
#include <QFile>
#include <QTextStream>
#include <QGraphicsPathItem>
#include <QPointF>
#include <QPainter>
#include <QDebug>
#include <memory>

using namespace std;

class StationItem : public QGraphicsRectItem
{
protected:
    QPen pen_normal;
    QPen pen_highlighted;

public:
    int station_id;

    StationItem(const Station& s, const QPointF& point)
        : QGraphicsRectItem(point.x()-0.0001, point.y()-0.0001, 0.0002, 0.0002), station_id(s.id)
    {
        setFlag(QGraphicsItem::ItemIsSelectable, true);

        if (s.ident.empty())
        {
            // Fixed stations
            pen_normal.setColor(QColor(96, 128, 64));
            pen_normal.setWidth(10);
            pen_normal.setCosmetic(true);
            pen_highlighted.setColor(QColor(255, 128, 128));
            pen_highlighted.setWidth(10);
            pen_highlighted.setCosmetic(true);
        } else {
            // Mobile stations
            pen_normal.setColor(QColor(96, 192, 96));
            pen_normal.setWidth(10);
            pen_normal.setCosmetic(true);
            pen_highlighted.setColor(QColor(255, 192, 192));
            pen_highlighted.setWidth(10);
            pen_highlighted.setCosmetic(true);
        }
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        if (isSelected())
            setPen(pen_highlighted);
        else
            setPen(pen_normal);
        QGraphicsRectItem::paint(painter, option, widget);
    }
};

MapScene::MapScene(Model& model, QObject *parent)
    : QObject(parent),
      model(model),
      coastline_group(0)
{
    connect(&model, SIGNAL(active_filter_changed()), this, SLOT(update_stations()));
    connect(&scene, SIGNAL(selectionChanged()), this, SLOT(on_selection_changed()));
    const QList<QGraphicsItem *> empty_group;

    coastline_group = scene.createItemGroup(empty_group);
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

void MapScene::update_stations()
{
    const std::map<int, Station>& stations = model.stations();
    for (map<int, Station>::const_iterator si = stations.begin(); si != stations.end(); ++si)
    {
        QPointF center(si->second.lon, si->second.lat);
        to_proj(center);
        StationItem* i = new StationItem(si->second, center);
        scene.addItem(i);
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
    } else if (items.length() > 1) {
        QRectF area = scene.selectionArea().boundingRect();
        qDebug() << "Selected" << area;
    }
}

void MapScene::add_coastline_path(const QPainterPath &p)
{
    QAbstractGraphicsShapeItem* i = new QGraphicsPathItem(p);
    coastline_group->addToGroup(i);
    i->setPen(coastline_pen);
}
