#include "mandelbrotzonecalculator.h"
#include <QDebug>

MandelbrotZoneCalculator::MandelbrotZoneCalculator(float ix_min, float ix_max, float iy_min, float iy_max, int in_pixel, int iiter_max)
{
    x_min = ix_min;
    x_max = ix_max;
    y_min = iy_min;
    y_max = iy_max;
    n_pixel = in_pixel;
    iter_max = iiter_max;
}

void MandelbrotZoneCalculator::compute()
{
    for(int i=0; i<n_pixel; i++) {
        //qDebug() << "   Line:" << i;
        for(int j=0; j<n_pixel; j++) {
            float c_x = x_min + (x_max-x_min) * i / n_pixel ;
            float c_y = y_min + (y_max-y_min) * j / n_pixel ;
            float z_x = 0;
            float z_y = 0;
            for(int n=0; n<iter_max; n++) {
                float zz_x = z_x * z_x - z_y * z_y + c_x;
                z_y = 2 * z_x * z_y + c_y;
                z_x = zz_x;
            }
        }
    }
}
