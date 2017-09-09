#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mandelbrotzonecalculatorthread.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _mandelbrotZoneCalculatorThread(NULL),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{

}

void MainWindow::paintEvent(QPaintEvent *event)
{
    qDebug("Paint Event received");
    computeMandelbrot();
}

void MainWindow::on_quitButton_clicked()
{
    qDebug("Exit Button pushed");
    QApplication::exit();
}

void MainWindow::on_computeButton_clicked()
{
    qDebug("Compute Button pushed");
    computeMandelbrot();
}

void MainWindow::computeMandelbrot()
{
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
        qDebug() << "========     Window Area:     ========";
        qDebug() << "test" << ui->mandelbrotZoneLabel->size();
        qDebug() << "======================================";

        if ((_mandelbrotZoneCalculatorThread!=NULL) &&
                ((iter_max==_mandelbrotZoneCalculatorThread->getIter_max()) &&
                 (x_min==_mandelbrotZoneCalculatorThread->getX_min()) &&
                 (x_max==_mandelbrotZoneCalculatorThread->getX_max()) &&
                 (y_min==_mandelbrotZoneCalculatorThread->getY_min()) &&
                 (y_max==_mandelbrotZoneCalculatorThread->getY_max()) &&
                 (ui->mandelbrotZoneLabel->size().width()==_mandelbrotZoneCalculatorThread->getWidth()) &&
                 (ui->mandelbrotZoneLabel->size().height()==_mandelbrotZoneCalculatorThread->getHeight()) )) {
            qDebug() << "SKIPPED CALCULATION";
            return;
        }

        if (_mandelbrotZoneCalculatorThread!=NULL && _mandelbrotZoneCalculatorThread->isFinished()){
            delete _mandelbrotZoneCalculatorThread;
            _mandelbrotZoneCalculatorThread=NULL;
        }

        _mandelbrotZoneCalculatorThread = new MandelbrotZoneCalculatorThread(x_min,x_max,y_min,y_max,ui->mandelbrotZoneLabel->size().width(),ui->mandelbrotZoneLabel->size().height(),iter_max);
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
    QImage myImage(_mandelbrotZoneCalculatorThread->getWidth(), _mandelbrotZoneCalculatorThread->getHeight(), QImage::Format_RGB32);
    QRgb valueIN = qRgb(189, 149, 39); // 0xffbd9527
    QRgb valueOFF = qRgb(0, 0, 0); // 0xffbd9527

    std::vector<std::vector<QPair<bool, int>>> myZone = _mandelbrotZoneCalculatorThread->getComputedZone();

    for(int i=0; i<_mandelbrotZoneCalculatorThread->getWidth(); i++) {
        for(int j=0; j<_mandelbrotZoneCalculatorThread->getHeight(); j++) {
            if (myZone[i][j].first) {
                myImage.setPixel(i, j, valueOFF);
            }
            else {
                float zoom = ui->zoomLineEdit->text().toFloat();
                float x0 = ui->x0LineEdit->text().toFloat();
                float y0 = ui->y0LineEdit->text().toFloat();
                float half_range = pow(2.0,(-zoom-1));
                float x_min = x0 - half_range;
                float x_max = x0 + half_range;
                float y_min = y0 - half_range;
                float y_max = y0 + half_range;
                float x = x_min + i * (x_max - x_min)/_mandelbrotZoneCalculatorThread->getWidth();
                float y = y_min + j * (y_max - y_min)/_mandelbrotZoneCalculatorThread->getHeight();
                float zn = sqrt( x * x + y * y );
                float hue = myZone[i][j].second + 1.0 - log(1+abs(log(zn))) / log(2);
                hue = 0.95 + 80.0 * hue;
                //qDebug() << "Hue:" << x << y << zn << myZone[i][j].second << log(1+abs(log(zn))) << hue ;
                hue=(int)hue%360;
                QColor color;
                color.setHsv(hue,200,200);
                myImage.setPixelColor(i, j, color);
//                myImage.setPixel(i, j, valueIN);
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
