#include "mandelbrotlabel.h"
#include <QMouseEvent>
#include <QDebug>

MandelbrotLabel::MandelbrotLabel(QWidget *parent) : QLabel(parent)
{
    setMouseTracking(true);
}

void MandelbrotLabel::setZone(long double ix_min, long double ix_max, long double iy_min, long double iy_max, int iwidth_pixel, int iheight_pixel)
{
    x_min = ix_min;
    x_max = ix_max;
    y_min = iy_min;
    y_max = iy_max;
    width_pixel = iwidth_pixel;
    height_pixel = iheight_pixel;
}

void MandelbrotLabel::setIter_max(int iiter_max)
{
    iter_max = iiter_max;
}

void MandelbrotLabel::setRendering_method(MandelbrotSetRenderingMethod irendering_method)
{
    rendering_method = irendering_method;
}

bool MandelbrotLabel::isSameZone(long double ix_min, long double ix_max, long double iy_min, long double iy_max, int iwidth_pixel, int iheight_pixel, int iiter_max)
{
    //qDebug() << "DEBUG LOOP" << (ix_min==x_min) << (ix_max==x_max) << (iy_min==y_min) << (iy_max==y_max) << (iwidth_pixel==width_pixel) << (iheight_pixel==height_pixel) << (iiter_max==iter_max);
    return (ix_min==x_min && ix_max==x_max && iy_min==y_min && iy_max==y_max && iwidth_pixel==width_pixel && iheight_pixel==height_pixel && iiter_max==iter_max);
}

bool MandelbrotLabel::isSameSize(int iwidth_pixel, int iheight_pixel)
{
    return (iwidth_pixel==width_pixel && iheight_pixel==height_pixel);
}

bool MandelbrotLabel::isSameRendering_method(MandelbrotSetRenderingMethod irendering_method)
{
    return (rendering_method.rendering_method_name==irendering_method.rendering_method_name && rendering_method.parameter1==irendering_method.parameter1);
}

PrecisionPoint MandelbrotLabel::convertScreenPositionToRealPosition(QPoint iPosition)
{
    PrecisionPoint myPoint;
    myPoint.x = x_min + iPosition.x()*(x_max - x_min)/width_pixel;
    myPoint.y = y_min + iPosition.y()*(y_max - y_min)/height_pixel;
    return myPoint;
}

void MandelbrotLabel::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "INT MOUSE PRESS EVENT:" << event->globalPos() << event->pos();
    emit mouseMoveHappened(convertScreenPositionToRealPosition(event->pos()));
}

void MandelbrotLabel::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "MOUSE MOVE EVENT:" << event->globalPos() << event->pos();
    //emit mouseClickHappened(convertScreenPositionToRealPosition(event->pos()));
    emit mouseClickHappened(convertScreenPositionToRealPosition(event->pos()), 1);
}

void MandelbrotLabel::wheelEvent(QWheelEvent *event)
{
    qDebug() << "MOUSE WHEEL EVENT:" << event->pos() << event->angleDelta();
    //emit mouseWheelHappened(convertScreenPositionToRealPosition(event->pos()), event->angleDelta().y()/120);
    emit mouseWheelHappened(1);
}
