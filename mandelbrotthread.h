#ifndef MANDELBROTTHREAD_H
#define MANDELBROTTHREAD_H

#include <QObject>
#include <QThread>
#include "mandelbrotzonecalculator.h"

class MandelbrotThread : public QThread
{
    Q_OBJECT

public:
    MandelbrotThread(float ix_min, float ix_max, float iy_min, float iy_max, int in_pixel, int iiter_max);
    ~MandelbrotThread();
    std::vector<std::vector<QPair<bool, int>>> getComputedZone();

protected:
    void run();

signals:
    void zoneComputed();

private:
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    int iter_max;
    int n_pixel;
    MandelbrotZoneCalculator *_mandelbrotZoneCalculator;
};

#endif // MANDELBROTTHREAD_H
