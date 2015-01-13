#include "mapscene.h"
#include <QFile>
#include <QTextStream>
#include <QGraphicsPathItem>
#include <QPointF>
#include <QPainter>
#include <QDebug>
#include <memory>

using namespace std;

MapScene::MapScene(Model& model, QObject *parent)
    : QObject(parent),
      model(model),
      coastline_group(0)
{
    connect(&model, SIGNAL(active_filter_changed()), this, SLOT(update_stations()));
    const QList<QGraphicsItem *> empty_group;

    coastline_group = scene.createItemGroup(empty_group);
    coastline_group->setActive(false);

    coastline_pen.setColor(Qt::gray);
    station_fixed_pen.setColor(QColor(64, 128, 64));
    station_fixed_pen.setWidth(10);
    station_fixed_pen.setCosmetic(true);
    station_mobile_pen.setColor(QColor(96, 192, 96));
    station_mobile_pen.setWidth(10);
    station_mobile_pen.setCosmetic(true);
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
        QGraphicsRectItem* i = new QGraphicsRectItem(center.x()-0.0001, center.y()-0.0001, 0.0002, 0.0002);
        scene.addItem(i);
        if (si->second.ident.empty())
            i->setPen(station_fixed_pen);
        else
            i->setPen(station_mobile_pen);
    }
}

void MapScene::add_coastline_path(const QPainterPath &p)
{
    QAbstractGraphicsShapeItem* i = new QGraphicsPathItem(p);
    coastline_group->addToGroup(i);
    i->setPen(coastline_pen);
}
