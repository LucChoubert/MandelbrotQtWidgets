#include "mandelbrotthread.h"
#include <QDebug>
#include <QElapsedTimer>

MandelbrotThread::MandelbrotThread(float ix_min, float ix_max, float iy_min, float iy_max, int in_pixel, int iiter_max)
{
    x_min = ix_min;
    x_max = ix_max;
    y_min = iy_min;
    y_max = iy_max;
    n_pixel = in_pixel;
    iter_max = iiter_max;
    _mandelbrotZoneCalculator = NULL;
}

MandelbrotThread::~MandelbrotThread()
{

}

std::vector<std::vector<QPair<bool, int>>> MandelbrotThread::getComputedZone(){
    return _mandelbrotZoneCalculator->getComputedZone();
}

void MandelbrotThread::run()
{
    qDebug() << "Calculation Thread :" << x_min << "," << x_max << "," << y_min << "," << y_max << "," << n_pixel << "," << iter_max<< " - Starting...";
    QElapsedTimer timer;
    timer.start();
    if (_mandelbrotZoneCalculator!=NULL) {
        delete _mandelbrotZoneCalculator;
    }
    _mandelbrotZoneCalculator = new MandelbrotZoneCalculator(x_min,x_max,y_min,y_max,n_pixel,iter_max);
    _mandelbrotZoneCalculator->computeZone();
    qDebug() << "Calculation Thread - Completed in (sec):" << float(timer.elapsed())/1000;

    // Calculation completed, send signal to main UI for rendering
    emit zoneComputed();
}
