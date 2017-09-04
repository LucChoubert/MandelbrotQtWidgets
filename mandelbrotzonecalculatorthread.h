#ifndef MANDELBROTZONECALCULATORTHREAD_H
#define MANDELBROTZONECALCULATORTHREAD_H

#include <QObject>
#include <QThread>

class MandelbrotZoneCalculatorThread : public QThread
{
    Q_OBJECT

public:
    MandelbrotZoneCalculatorThread(float ix_min, float ix_max, float iy_min, float iy_max, int iheight_pixel, int iwidth_pixel, int iiter_max);
    ~MandelbrotZoneCalculatorThread();
    std::vector<std::vector<QPair<bool, int>>> getComputedZone();
    void computeZone();
    QPair<bool, int> compute(float c_x, float c_y);

protected:
    void run();

signals:
    void zoneComputationCompleted();

private:
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    int iter_max;
    int height_pixel;
    int width_pixel;
    std::vector<std::vector<QPair<bool, int>>> outputZone;
};

#endif // MANDELBROTZONECALCULATORTHREAD_H
