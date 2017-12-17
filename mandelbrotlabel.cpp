#include "mandelbrotlabel.h"
#include <QMouseEvent>
#include <QDebug>
#include <sstream>

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
//    bool aValue = (ix_min==x_min && ix_max==x_max && iy_min==y_min && iy_max==y_max && iwidth_pixel==width_pixel && iheight_pixel==height_pixel && iiter_max==iter_max);
// Testing in String to avoid the floating point comparison difficulties. Epsilon method more complex to code an no need for performance here
    bool aValue = (convertLongDoubleToString(ix_min)==convertLongDoubleToString(x_min) &&
                   convertLongDoubleToString(ix_max)==convertLongDoubleToString(x_max) &&
                   convertLongDoubleToString(iy_min)==convertLongDoubleToString(y_min) &&
                   convertLongDoubleToString(iy_max)==convertLongDoubleToString(y_max) &&
                   iwidth_pixel==width_pixel &&
                   iheight_pixel==height_pixel &&
                   iiter_max==iter_max);
//    if (!aValue) {
//        qDebug() << "DEBUG SameZone" << (ix_min==x_min) << (ix_max==x_max) << (iy_min==y_min) << (iy_max==y_max) << (iwidth_pixel==width_pixel) << (iheight_pixel==height_pixel) << (iiter_max==iter_max);
//        qDebug() << "DEBUG SameZone" << convertLongDoubleToString(ix_min)  << convertLongDoubleToString(x_min);
//        qDebug() << "DEBUG SameZone" << convertLongDoubleToString(ix_max) << convertLongDoubleToString(x_max);
//        qDebug() << "DEBUG SameZone" << convertLongDoubleToString(iy_min) << convertLongDoubleToString(y_min);
//        qDebug() << "DEBUG SameZone" << convertLongDoubleToString(iy_max) << convertLongDoubleToString(y_max);
//        qDebug() << "DEBUG SameZone" << convertLongDoubleToString(iwidth_pixel) << convertLongDoubleToString(width_pixel);
//        qDebug() << "DEBUG SameZone" << convertLongDoubleToString(iheight_pixel) << convertLongDoubleToString(height_pixel);
//        qDebug() << "DEBUG SameZone" << convertLongDoubleToString(iiter_max) << convertLongDoubleToString(iter_max);
//    }
        return aValue;
}

bool MandelbrotLabel::isSameSize(int iwidth_pixel, int iheight_pixel)
{
    return (iwidth_pixel==width_pixel && iheight_pixel==height_pixel);
}

bool MandelbrotLabel::isSameRendering_method(MandelbrotSetRenderingMethod irendering_method)
{
    bool aValue = (rendering_method.rendering_method_name==irendering_method.rendering_method_name && rendering_method.parameter1==irendering_method.parameter1);
//    if (!aValue) {
//        qDebug() << "DEBUG SameRendering" << (rendering_method.rendering_method_name==irendering_method.rendering_method_name) << (rendering_method.parameter1==irendering_method.parameter1);
//    }
    return aValue;
}

PrecisionPoint MandelbrotLabel::convertScreenPositionToRealPosition(QPoint iPosition)
{
    PrecisionPoint myPoint;
    myPoint.x = x_min + iPosition.x()*(x_max - x_min)/width_pixel;
    myPoint.y = y_min + iPosition.y()*(y_max - y_min)/height_pixel;
    return myPoint;
}

QString MandelbrotLabel::convertLongDoubleToString(const long double iLongDouble)
{
  std::stringstream ss;
  ss << iLongDouble;

  return QString::fromStdString(ss.str());
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
    emit mouseWheelHappened(event->angleDelta().y()/120);
}
