#include "mandelbrotlabel.h"
#include <QDebug>

MandelbrotLabel::MandelbrotLabel(QWidget *parent) : QLabel(parent)
{
    setMouseTracking(true);
}

void MandelbrotLabel::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "MOUSE MOVE EVENT";
}
