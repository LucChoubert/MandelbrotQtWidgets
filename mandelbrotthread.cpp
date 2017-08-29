#include "mandelbrotthread.h"
#include "mandelbrotzonecalculator.h"
#include <QDebug>
#include <QElapsedTimer>

MandelbrotThread::MandelbrotThread()
{

}

MandelbrotThread::~MandelbrotThread()
{

}

void MandelbrotThread::run()
{
    qDebug() << "Calculation Thread - Starting...";
    QElapsedTimer timer;
    timer.start();
    MandelbrotZoneCalculator myMandelbrotZoneCalculator = MandelbrotZoneCalculator(-2,1,-1.5,1.5,600,5000);
    myMandelbrotZoneCalculator.compute();
    qDebug() << "Calculation Thread - Completed in (sec):" << float(timer.elapsed())/1000;
}
