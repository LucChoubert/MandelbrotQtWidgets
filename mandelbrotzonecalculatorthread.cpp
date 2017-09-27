#include "mandelbrotzonecalculatorthread.h"
#include <QDebug>
#include <QElapsedTimer>

MandelbrotZoneCalculatorThread::MandelbrotZoneCalculatorThread()
{
    setCalculationDetails(0,0,0,0,0,0,0);
}

MandelbrotZoneCalculatorThread::MandelbrotZoneCalculatorThread(long double ix_min, long double ix_max, long double iy_min, long double iy_max, int iwidth_pixel, int iheight_pixel, int iiter_max, int ioffset_pixel)
{
    setCalculationDetails(ix_min, ix_max, iy_min, iy_max, iwidth_pixel, iheight_pixel, iiter_max, ioffset_pixel);
}

void MandelbrotZoneCalculatorThread::setCalculationDetails(long double ix_min, long double ix_max, long double iy_min, long double iy_max, int iwidth_pixel, int iheight_pixel, int iiter_max, int ioffset_pixel)
{
    x_min = ix_min;
    x_max = ix_max;
    y_min = iy_min;
    y_max = iy_max;
    width_pixel = iwidth_pixel;
    height_pixel = iheight_pixel;
    offset_pixel = ioffset_pixel;
    iter_max = iiter_max;
    outputZone.resize(width_pixel);
    for(int i = 0; i < width_pixel; i++) {
        outputZone[i].resize(height_pixel);
    }
}

MandelbrotZoneCalculatorThread::~MandelbrotZoneCalculatorThread()
{
    //qDebug() << "Calculation Thread :" << (double)x_min << "," << (double)x_max << "," << (double)y_min << "," << (double)y_max << "," << width_pixel << "," << height_pixel << "," << iter_max << "," << offset_pixel << " - ...Destroyed";
}

std::vector<std::vector<MandelbrotSetPoint>> MandelbrotZoneCalculatorThread::getComputedZone(){
    return outputZone;
}

int MandelbrotZoneCalculatorThread::getOffset()
{
    return offset_pixel;
}

int MandelbrotZoneCalculatorThread::getWidth()
{
    return width_pixel;
}

int MandelbrotZoneCalculatorThread::getHeight()
{
    return height_pixel;
}

int MandelbrotZoneCalculatorThread::getIter_max()
{
    return iter_max;
}

long double MandelbrotZoneCalculatorThread::getX_min()
{
    return x_min;
}

long double MandelbrotZoneCalculatorThread::getX_max()
{
    return x_max;
}

long double MandelbrotZoneCalculatorThread::getY_min()
{
    return y_min;
}

long double MandelbrotZoneCalculatorThread::getY_max()
{
    return y_max;
}


void MandelbrotZoneCalculatorThread::run()
{
    //qDebug() << "Calculation Thread :" << (double)x_min << "," << (double)x_max << "," << (double)y_min << "," << (double)y_max << "," << width_pixel<< "," << height_pixel << "," << iter_max << "," << offset_pixel << " - Starting...";
    QElapsedTimer timer;
    timer.start();

    computeZone();

    //qDebug() << "Calculation Thread - Completed in (sec):" << float(timer.elapsed())/1000;

    // Calculation completed, send signal to main UI for rendering
    emit zoneComputationCompleted(this);
}

MandelbrotSetPoint MandelbrotZoneCalculatorThread::compute(long double c_x, long double c_y)
{
    long double z_x = 0;
    long double z_y = 0;
    long double module = 0;
    int n = 0;
    while ((module<4) && (n<iter_max)) {
        long double zz_x = z_x * z_x - z_y * z_y + c_x;
        z_y = 2 * z_x * z_y + c_y;
        z_x = zz_x;
        module = z_x*z_x + z_y*z_y;
        n++;
    }
    bool in_M = (module<4);
    //return QPair<bool, int>(in_M,n);
    MandelbrotSetPoint myMandelbrotSetPoint;
    myMandelbrotSetPoint.x = c_x;
    myMandelbrotSetPoint.y = c_y;
    myMandelbrotSetPoint.n = n;
    myMandelbrotSetPoint.xn = z_x;
    myMandelbrotSetPoint.yn = z_y;
    myMandelbrotSetPoint.isInM = in_M;
    return myMandelbrotSetPoint;
}

void MandelbrotZoneCalculatorThread::computeZone()
{
    for(int i=0; i<width_pixel; i++) {
//        qDebug() << "   Line:" << i;
        for(int j=0; j<height_pixel; j++) {
            long double c_x = x_min + (x_max-x_min) * i / width_pixel ;
            long double c_y = y_min + (y_max-y_min) * j / height_pixel ;
            MandelbrotSetPoint myMandelbrotSetPoint = compute(c_x,c_y);
            QPair<bool, int> myBit(myMandelbrotSetPoint.isInM,myMandelbrotSetPoint.n);
//            if (myMandelbrotSetPoint.n==0) {
//                qDebug() << "Point:(" << c_x << "," << c_y << ") -> " << myMandelbrotSetPoint.n << myMandelbrotSetPoint.xn << myMandelbrotSetPoint.yn << myMandelbrotSetPoint.isInM;
//            }
            outputZone[i][j] = myMandelbrotSetPoint;
        }
    }
}
