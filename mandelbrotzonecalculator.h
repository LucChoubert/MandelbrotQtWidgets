#ifndef MANDELBROTZONECALCULATOR_H
#define MANDELBROTZONECALCULATOR_H

#include <QPair>

class MandelbrotZoneCalculator
{
public:
    MandelbrotZoneCalculator(float ix_min, float ix_max, float iy_min, float iy_max, int in_pixel, int iiter_max);
    void computeZone();
    QPair<bool, int> compute(float c_x, float c_y);

private:
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    int iter_max;
    int n_pixel;
};

#endif // MANDELBROTZONECALCULATOR_H
