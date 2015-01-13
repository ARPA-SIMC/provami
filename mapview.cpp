#include "provami/mapview.h"
#include <QWheelEvent>

namespace provami {

MapView::MapView(QWidget *parent) :
    QGraphicsView(parent)
{
}

// From http://www.qtcentre.org/wiki/index.php?title=QGraphicsView:_Smooth_Panning_and_Zooming
void MapView::wheelEvent(QWheelEvent* event) {

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // Scale the view / do the zoom
    double scaleFactor = 1.15;
    if(event->delta() > 0) {
        // Zoom in
        scale(scaleFactor, scaleFactor);
    } else {
        // Zooming out
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }

    // Don't call superclass handler here
    // as wheel is normally used for moving scrollbars
}

void MapView::mousePressEvent(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier)
    {
        setDragMode(QGraphicsView::RubberBandDrag);
    }
    QGraphicsView::mousePressEvent(event);
}

void MapView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

}
