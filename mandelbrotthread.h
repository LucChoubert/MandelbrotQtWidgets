#ifndef MANDELBROTTHREAD_H
#define MANDELBROTTHREAD_H

#include <QObject>
#include <QThread>

class MandelbrotThread : public QThread
{
    Q_OBJECT

public:
    MandelbrotThread(float ix_min, float ix_max, float iy_min, float iy_max, int in_pixel, int iiter_max);
    ~MandelbrotThread();

protected:
    void run();

private:
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    int iter_max;
    int n_pixel;
};

#endif // MANDELBROTTHREAD_H
