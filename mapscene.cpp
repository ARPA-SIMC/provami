#include "mapscene.h"
#include <QFile>
#include <QTextStream>
#include <QGraphicsPathItem>
#include <QPointF>
#include <stdio.h>
#include <memory>

using namespace std;

MapScene::MapScene(Model& model, QObject *parent)
    : QObject(parent), model(model), coastline_pen(Qt::black)
{
    connect(&model, SIGNAL(active_filter_changed()), this, SLOT(update_stations()));
}

void MapScene::load_coastlines(const std::string &fname)
{
    fprintf(stderr, "LOAD FROM %s\n", fname.c_str());

    QFile fd(fname.c_str());
    if (!fd.open(QIODevice::ReadOnly))
    {
        fprintf(stderr, "Cannot open %s\n", fname.c_str());
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
    //point.setX(point.x());
    point.setY(-point.y());
}

void MapScene::to_latlon(QPointF &point)
{
    //point.setX(point.x());
    point.setY(-point.y());
}

void MapScene::update_stations()
{

}

void MapScene::add_coastline_path(const QPainterPath &p)
{
    QAbstractGraphicsShapeItem* i = new QGraphicsPathItem(p);
    scene.addItem(i);
    i->setPen(coastline_pen);
}
