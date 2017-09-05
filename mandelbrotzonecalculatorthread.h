#ifndef MANDELBROTZONECALCULATORTHREAD_H
#define MANDELBROTZONECALCULATORTHREAD_H

#include <QObject>
#include <QThread>

class MandelbrotZoneCalculatorThread : public QThread
{
    Q_OBJECT

public:
    MandelbrotZoneCalculatorThread(float ix_min, float ix_max, float iy_min, float iy_max, int iwidth_pixel, int iheight_pixel, int iiter_max);
    ~MandelbrotZoneCalculatorThread();
    std::vector<std::vector<QPair<bool, int>>> getComputedZone();
    int getWidth();
    int getHeight();
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
    int width_pixel;
    int height_pixel;
    std::vector<std::vector<QPair<bool, int>>> outputZone;
};

#endif // MANDELBROTZONECALCULATORTHREAD_H
