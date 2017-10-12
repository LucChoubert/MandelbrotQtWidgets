#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWindow;

    //parse command line options
    QCommandLineParser parser;
    QCommandLineOption benchmarkPerformanceOption("perf", QCoreApplication::translate("main", "start in batch mode and run a performance test"));
    parser.addOption(benchmarkPerformanceOption);
    parser.process(app);

    bool benchmarkPerformance = parser.isSet(benchmarkPerformanceOption);

    if (benchmarkPerformance) {
        qDebug() << "-- Starting Performance Benchmark";
        QElapsedTimer performanceTimer;
        performanceTimer.start();
        mainWindow.benchmarkPerformance();
        app.exec();
        QString stringTimer;
        stringTimer.setNum(float(performanceTimer.elapsed())/1000);
        qDebug() << "-- Performance Benchmark completed in:" << stringTimer << "s";

        return 0;
        //exit(0);
    }
    else {
        //mainWindow.showFullScreen();
        mainWindow.show();
        return app.exec();
    }


}
