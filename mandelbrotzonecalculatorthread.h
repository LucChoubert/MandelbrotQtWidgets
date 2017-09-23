#ifndef MANDELBROTZONECALCULATORTHREAD_H
#define MANDELBROTZONECALCULATORTHREAD_H

#include <basictypes.h>
#include <QObject>
#include <QThread>


class MandelbrotZoneCalculatorThread : public QThread
{
    Q_OBJECT

public:
    MandelbrotZoneCalculatorThread();
    MandelbrotZoneCalculatorThread(long double ix_min, long double ix_max, long double iy_min, long double iy_max, int iwidth_pixel, int iheight_pixel, int iiter_max, int ioffset_pixel=0);
    ~MandelbrotZoneCalculatorThread();
    void setCalculationDetails(long double ix_min, long double ix_max, long double iy_min, long double iy_max, int iwidth_pixel, int iheight_pixel, int iiter_max, int ioffset_pixel=0);
    std::vector<std::vector<MandelbrotSetPoint>> getComputedZone();
    int getWidth();
    int getHeight();
    int getOffset();
    int getIter_max();
    long double getX_min();
    long double getX_max();
    long double getY_min();
    long double getY_max();
    void computeZone();

private:
    MandelbrotSetPoint compute(long double c_x, long double c_y);

protected:
    void run();

signals:
    void zoneComputationCompleted(MandelbrotZoneCalculatorThread * iThread);

private:
    long double x_min;
    long double x_max;
    long double y_min;
    long double y_max;
    int iter_max;
    int width_pixel;
    int height_pixel;
    int offset_pixel;
    std::vector<std::vector<MandelbrotSetPoint>> outputZone;
};

#endif // MANDELBROTZONECALCULATORTHREAD_H
