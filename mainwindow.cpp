#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mandelbrotzonecalculatorthread.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _mandelbrotZoneCalculatorThread=NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_quitButton_clicked()
{
    qDebug("Exit Button pushed");
    QApplication::exit();
}

void MainWindow::on_computeButton_clicked()
{
    qDebug("Compute Button pushed");
    bool ok1, ok2, ok3, ok4;
    int iter_max = ui->iterationsLineEdit->text().toInt(&ok1);
    float zoom = ui->zoomLineEdit->text().toFloat(&ok2);
    float x0 = ui->x0LineEdit->text().toFloat(&ok3);
    float y0 = ui->y0LineEdit->text().toFloat(&ok4);
    if (!ok1 || !ok2 || !ok3 || !ok4)
    {
        QMessageBox messageBox;
        //messageBox.setIconPixmap(QPixmap(":/icon.png"));
        messageBox.setText("One of your entries is not a valid number.");
        messageBox.setWindowTitle("Error");
        messageBox.exec();
    }
    else {
        float half_range = pow(2.0,(-zoom-1));
        float x_min = x0 - half_range;
        float x_max = x0 + half_range;
        float y_min = y0 - half_range;
        float y_max = y0 + half_range;
        qDebug() << "======== Mandelbrot Set Area: ========";
        qDebug() << " Max Iteration: " << iter_max;
        qDebug() << "        Center: " << x0 << " " << y0;
        qDebug() << "  Width (2^-p): " << 2*half_range;
        qDebug() << " Height (2^-p): " << 2*half_range;
        qDebug() << "             X: " << x_min << " " << x_max;
        qDebug() << "             Y: " << y_min << " " << y_max;
        qDebug() << "======================================";

        if (_mandelbrotZoneCalculatorThread!=NULL){
            delete _mandelbrotZoneCalculatorThread;
            _mandelbrotZoneCalculatorThread=NULL;
        }

        _mandelbrotZoneCalculatorThread = new MandelbrotZoneCalculatorThread(x_min,x_max,y_min,y_max,600,600,iter_max);
        QObject::connect(_mandelbrotZoneCalculatorThread, &MandelbrotZoneCalculatorThread::zoneComputationCompleted, this, &MainWindow::renderMandelbrot);
        _mandelbrotZoneCalculatorThread->start();
    }
}

void MainWindow::renderMandelbrot()
{
    qDebug() << "Signal caught - Calculation Thread Completed" ;

    //Draw Mandelbrot Set from newly computed area
    //QImage myImage;
    //myImage.load("/home/luc/QtProjects/MandelbrotQtWidgets/images/mandelbrot-1.png");
    QImage myImage(600, 600, QImage::Format_RGB32);
    QRgb valueIN = qRgb(189, 149, 39); // 0xffbd9527
    QRgb valueOFF = qRgb(0, 0, 0); // 0xffbd9527

    std::vector<std::vector<QPair<bool, int>>> myZone = _mandelbrotZoneCalculatorThread->getComputedZone();

    for(int i=0; i<600; i++) {
        for(int j=0; j<600; j++) {
            if (myZone[i][j].first) {
                myImage.setPixel(i, j, valueIN);
            }
            else {
                myImage.setPixel(i, j, valueOFF);
            }
        }
    }

//    /* initialize random seed: */
//    srand (time(NULL));
//    /* generate secret number between 1 and 10: */
//    for(int i=0;i<10000;i++) {
//        int x = rand() % 600;
//        int y = rand() % 600;
//        myImage.setPixel(x, y, value);
//    }
//    value = qRgb(122, 163, 39); // 0xff7aa327
//    image.setPixel(0, 1, value);
//    image.setPixel(1, 0, value);

//    value = qRgb(237, 187, 51); // 0xffedba31
//    image.setPixel(2, 1, value);

    ui->mandelbrotZoneLabel->setPixmap(QPixmap::fromImage(myImage));

}
