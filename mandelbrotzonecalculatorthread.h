#ifndef MANDELBROTZONECALCULATORTHREAD_H
#define MANDELBROTZONECALCULATORTHREAD_H

#include <QObject>
#include <QThread>

struct MandelbrotPoint {
    // Coordinate of the point
    float x;
    float y;
    // Iter reach in Mandelbrot suite
    int n;
    // Coordinate of the nth iteration point
    float xn;
    float yn;
    // Boolean value to indicate if point is in or out the MAndelbrot Set
    bool isInM;
};
typedef struct MandelbrotPoint MandelbrotPoint;

class MandelbrotZoneCalculatorThread : public QThread
{
    Q_OBJECT

public:
    MandelbrotZoneCalculatorThread();
    MandelbrotZoneCalculatorThread(float ix_min, float ix_max, float iy_min, float iy_max, int iwidth_pixel, int iheight_pixel, int iiter_max, int ioffset_pixel=0);
    ~MandelbrotZoneCalculatorThread();
    void setCalculationDetails(float ix_min, float ix_max, float iy_min, float iy_max, int iwidth_pixel, int iheight_pixel, int iiter_max, int ioffset_pixel=0);
    std::vector<std::vector<QPair<bool, int>>> getComputedZone();
    std::vector<std::vector<MandelbrotPoint>> getComputedZone2();
    int getWidth();
    int getHeight();
    int getOffset();
    int getIter_max();
    float getX_min();
    float getX_max();
    float getY_min();
    float getY_max();
    void computeZone();

private:
    MandelbrotPoint compute(float c_x, float c_y);

protected:
    void run();

signals:
    void zoneComputationCompleted(MandelbrotZoneCalculatorThread * iThread);

private:
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    int iter_max;
    int width_pixel;
    int height_pixel;
    int offset_pixel;
    std::vector<std::vector<QPair<bool, int>>> outputZone;
    std::vector<std::vector<MandelbrotPoint>> outputZone2;
};

#endif // MANDELBROTZONECALCULATORTHREAD_H
