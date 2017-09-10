#include "mandelbrotzonecalculatorthread.h"
#include <QDebug>
#include <QElapsedTimer>

MandelbrotZoneCalculatorThread::MandelbrotZoneCalculatorThread(float ix_min, float ix_max, float iy_min, float iy_max, int iwidth_pixel, int iheight_pixel, int iiter_max)
{
    x_min = ix_min;
    x_max = ix_max;
    y_min = iy_min;
    y_max = iy_max;
    width_pixel = iwidth_pixel;
    height_pixel = iheight_pixel;
    iter_max = iiter_max;
    outputZone.resize(width_pixel);
    outputZone2.resize(width_pixel);
    for(int i = 0; i < width_pixel; i++) {
        outputZone[i].resize(height_pixel);
        outputZone2[i].resize(height_pixel);
    }
}

MandelbrotZoneCalculatorThread::~MandelbrotZoneCalculatorThread()
{
    qDebug() << "Calculation Thread :" << x_min << "," << x_max << "," << y_min << "," << y_max << "," << width_pixel << "," << height_pixel << "," << iter_max<< " - ...Destroyed";
}

std::vector<std::vector<QPair<bool, int>>> MandelbrotZoneCalculatorThread::getComputedZone(){
    return outputZone;
}

std::vector<std::vector<MandelbrotPoint>> MandelbrotZoneCalculatorThread::getComputedZone2(){
    return outputZone2;
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

float MandelbrotZoneCalculatorThread::getX_min()
{
    return x_min;
}

float MandelbrotZoneCalculatorThread::getX_max()
{
    return x_max;
}

float MandelbrotZoneCalculatorThread::getY_min()
{
    return y_min;
}

float MandelbrotZoneCalculatorThread::getY_max()
{
    return y_max;
}


void MandelbrotZoneCalculatorThread::run()
{
    qDebug() << "Calculation Thread :" << x_min << "," << x_max << "," << y_min << "," << y_max << "," << width_pixel<< "," << height_pixel << "," << iter_max<< " - Starting...";
    QElapsedTimer timer;
    timer.start();

    computeZone();

    qDebug() << "Calculation Thread - Completed in (sec):" << float(timer.elapsed())/1000;

    // Calculation completed, send signal to main UI for rendering
    emit zoneComputationCompleted();
}

//QPair<bool, int> MandelbrotZoneCalculatorThread::compute(float c_x, float c_y)
MandelbrotPoint MandelbrotZoneCalculatorThread::compute(float c_x, float c_y)
{
    float z_x = 0;
    float z_y = 0;
    float module = 0;
    int n = 0;
    while ((module<4) && (n<iter_max)) {
        float zz_x = z_x * z_x - z_y * z_y + c_x;
        z_y = 2 * z_x * z_y + c_y;
        z_x = zz_x;
        module = z_x*z_x + z_y*z_y;
        n++;
    }
    bool in_M = (module<4);
    //return QPair<bool, int>(in_M,n);
    MandelbrotPoint myMandelbrotPoint;
    myMandelbrotPoint.x = c_x;
    myMandelbrotPoint.y = c_y;
    myMandelbrotPoint.n = n;
    myMandelbrotPoint.xn = z_x;
    myMandelbrotPoint.yn = z_y;
    myMandelbrotPoint.isInM = in_M;
    return myMandelbrotPoint;
}

void MandelbrotZoneCalculatorThread::computeZone()
{
    for(int i=0; i<width_pixel; i++) {
//        qDebug() << "   Line:" << i;
        for(int j=0; j<height_pixel; j++) {
            float c_x = x_min + (x_max-x_min) * i / width_pixel ;
            float c_y = y_min + (y_max-y_min) * j / height_pixel ;
            MandelbrotPoint myMandelbrotPoint = compute(c_x,c_y);
            QPair<bool, int> myBit(myMandelbrotPoint.isInM,myMandelbrotPoint.n);
//            if (myMandelbrotPoint.n==0) {
//                qDebug() << "Point:(" << c_x << "," << c_y << ") -> " << myMandelbrotPoint.n << myMandelbrotPoint.xn << myMandelbrotPoint.yn << myMandelbrotPoint.isInM;
//            }
            outputZone[i][j] = myBit;
            outputZone2[i][j] = myMandelbrotPoint;
        }
    }
}
