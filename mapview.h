#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>

class MapView : public QGraphicsView
{
    Q_OBJECT

protected:
    // Implement wheel zooming
    virtual void wheelEvent(QWheelEvent* event);
    // Selection on shift+drag
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

public:
    explicit MapView(QWidget *parent = 0);
    
signals:
    
public slots:
    
};

#endif // MAPVIEW_H
