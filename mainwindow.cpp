#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mandelbrotzonecalculatorthread.h"
#include <QDebug>
#include <QMessageBox>
#include <QStatusBar>
#include <QPlainTextEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    threadRunning(false),
    nbThreadRunning(0),
    statusMessage(QString("Welcome into Mandelbrot set beauty")),
    timer(),
    _mandelbrotZoneCalculatorThread(NULL),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->addPermanentWidget(&statusMessage);
    QObject::connect(ui->mandelbrotZoneLabel, &MandelbrotLabel::mouseMoveHappened, this, &MainWindow::updateMandelbrotZoneCursorPosition);
    QObject::connect(ui->mandelbrotZoneLabel, &MandelbrotLabel::mouseClickHappened, this, &MainWindow::updateMandelbrotZoneCenter);
    QObject::connect(ui->mandelbrotZoneLabel, &MandelbrotLabel::mouseWheelHappened, this, &MainWindow::updateMandelbrotZoneZoomAndCenter);
    mandelbrotSetDefinition.x0 = ui->x0LineEdit->text().toFloat();
    mandelbrotSetDefinition.y0 = ui->y0LineEdit->text().toFloat();
    mandelbrotSetDefinition.iter_max = ui->iterationsLineEdit->text().toInt();
    mandelbrotSetDefinition.zoom = ui->zoomLineEdit->text().toFloat();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateMandelbrotSetDefinitionPanel() {
    QString stringX, stringY, stringZoom, stringIter;

    stringX.setNum((float)mandelbrotSetDefinition.x0);
    ui->x0LineEdit->setText(stringX);

    stringY.setNum((float)mandelbrotSetDefinition.y0);
    ui->y0LineEdit->setText(stringY);

    stringZoom.setNum((float)mandelbrotSetDefinition.zoom);
    ui->zoomLineEdit->setText(stringZoom);

    stringIter.setNum((int)mandelbrotSetDefinition.iter_max);
    ui->iterationsLineEdit->setText(stringIter);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{

}

void MainWindow::paintEvent(QPaintEvent *event)
{
    //qDebug() << QThread::currentThread() <<  "- Paint Event received";
    computeMandelbrot();
    //qDebug() << QThread::currentThread() << "- Paint Event Completed";
}

void MainWindow::on_quitButton_clicked()
{
    //qDebug() << "Exit Button pushed";
    QApplication::exit();
}

void MainWindow::on_computeButton_clicked()
{
    //qDebug() << "Compute Button pushed";
    bool ok1, ok2, ok3, ok4;
    mandelbrotSetDefinition.x0 = ui->x0LineEdit->text().toFloat(&ok1);
    mandelbrotSetDefinition.y0 = ui->y0LineEdit->text().toFloat(&ok2);
    mandelbrotSetDefinition.iter_max = ui->iterationsLineEdit->text().toInt(&ok3);
    mandelbrotSetDefinition.zoom = ui->zoomLineEdit->text().toFloat(&ok4);

    if (!ok1 || !ok2 || !ok3 || !ok4)
    {
        QMessageBox messageBox;
        //messageBox.setIconPixmap(QPixmap(":/icon.png"));
        messageBox.setText("One of your entries is not a valid number.");
        messageBox.setWindowTitle("Error");
        messageBox.exec();
    }

    ui->mandelbrotZoneLabel->repaint();
    //computeMandelbrot();
}

void MainWindow::updateMandelbrotZoneCursorPosition(PrecisionPoint position)
{
    //qDebug() << "updateMandelbrotZoneCursorPosition:" << position;
    QString stringX, stringY;
    stringX.setNum((float)position.x, 'g', 100);
    stringY.setNum((float)position.y, 'g', 100);
    ui->statusBar->showMessage(QString("Cursor:(") + stringX + QString(",") + stringY + QString(")"),10000);
}

void MainWindow::updateMandelbrotZoneCenter(PrecisionPoint position)
{
    //qDebug() << "updateMandelbrotZoneCenter:" << position;
    mandelbrotSetDefinition.x0 = position.x;
    mandelbrotSetDefinition.y0 = position.y;
    updateMandelbrotSetDefinitionPanel();
    ui->mandelbrotZoneLabel->repaint();
}

void MainWindow::updateMandelbrotZoneZoomAndCenter(PrecisionPoint position, int zoomFactor)
{
    qDebug() << "updateMandelbrotZoneZoomAndCenter:" << (float)position.x << (float)position.y << zoomFactor;

    mandelbrotSetDefinition.x0 = position.x;
    mandelbrotSetDefinition.y0 = position.y;
    mandelbrotSetDefinition.zoom += zoomFactor*0.5;

    updateMandelbrotSetDefinitionPanel();
    ui->mandelbrotZoneLabel->repaint();
}

void MainWindow::computeMandelbrot()
{
    long double x0 = mandelbrotSetDefinition.x0;
    long double y0 = mandelbrotSetDefinition.y0;
    float zoom = mandelbrotSetDefinition.zoom;
    int iter_max = mandelbrotSetDefinition.iter_max;

    long double half_range = pow(2.0,(-zoom-1));
    long double x_min = x0 - half_range;
    long double x_max = x0 + half_range;
    long double y_min = y0 - half_range;
    long double y_max = y0 + half_range;
    QSize mySize = ui->mandelbrotZoneLabel->size();

    if (ui->mandelbrotZoneLabel->isSameZone(x_min,x_max,y_min,y_max,mySize.width(),mySize.height(),iter_max)) {
        //qDebug() << "SKIPPED CALCULATION - Same Area";
        return;
    }

    if (threadRunning) {
        //qDebug() << "SKIPPED CALCULATION - Calculation Threads already running";
        return;
    }
    else {
        threadRunning = true;
    }

    //mandelbrotZoneLabel doest not have the right pixmap size so let's recreate one with the appropriate size
    if (!ui->mandelbrotZoneLabel->isSameSize(mySize.width(),mySize.height())) {
        QImage myImage(mySize, QImage::Format_RGB32);
        ui->mandelbrotZoneLabel->setPixmap(QPixmap::fromImage(myImage));
    }

    qDebug() << "======== Mandelbrot Set Area: ========";
    qDebug() << " Max Iteration: " << iter_max;
    qDebug() << "        Center: " << (float) x0 << " " << (float)y0;
    qDebug() << "  Width (2^-p): " << (float)(2*half_range);
    qDebug() << " Height (2^-p): " << (float)(2*half_range);
    qDebug() << "             X: " << (float)x_min << " " << (float)x_max;
    qDebug() << "             Y: " << (float)y_min << " " << (float)y_max;
    qDebug() << "========     Window Area:     ========";
    qDebug() << "Mandelbrot Widget Size" << mySize;
    qDebug() << "======================================";

    //Status bar management
    statusMessage.setText(QString("Calculation Running"));

    //Reset rendering time value
    renderingTime = 0;

    //Start measuring calculation time
    timer.start();

    // Allocate thread manager objects if not done already

    int nBThreads = QThread::idealThreadCount();

    if (_listMandelbrotZoneCalculatorThread.empty()) {
        for (int i=0; i<nBThreads; i++) {
            MandelbrotZoneCalculatorThread * myThread = new MandelbrotZoneCalculatorThread();
            _listMandelbrotZoneCalculatorThread.push_back(myThread);
            QObject::connect(myThread, &MandelbrotZoneCalculatorThread::zoneComputationCompleted, this, &MainWindow::renderMandelbrot);
        }
    }

    // split the zone in smaller zones based on the number of available threads, set configuration of the threads, and start them
    nBThreads = _listMandelbrotZoneCalculatorThread.size();
    nbThreadRunning = nBThreads;
    int pixel_range = ceil((float)mySize.width() / nBThreads);
    int zone_offset = 0;
    for (int i=0; i< nBThreads; i++) {
        long double zone_x_min = x_min + i * (x_max - x_min) / nBThreads;
        long double zone_x_max = x_min + (i + 1) * (x_max - x_min) / nBThreads;
        int zone_width = std::min(pixel_range, mySize.width() - zone_offset) ;
        int zone_height = mySize.height();
        _listMandelbrotZoneCalculatorThread[i]->setCalculationDetails(zone_x_min,zone_x_max,y_min,y_max,zone_width,zone_height,iter_max,zone_offset);
        _listMandelbrotZoneCalculatorThread[i]->start();
        zone_offset += zone_width;

    }

}

void MainWindow::renderMandelbrot(MandelbrotZoneCalculatorThread * iThread)
{
    //Count reference of active threads
    nbThreadRunning--;

    QString loggingText, stringTimer;
    qDebug() << "Signal received - Calculation Thread Completed for widget zone size " << iThread->getOffset() << iThread->getWidth() << iThread->getHeight()  ;

    if (nbThreadRunning==0) {
        qDebug() << "User perception - TOTAL Calculation completed in (sec):" << float(timer.elapsed())/1000;
        stringTimer.setNum(float(timer.elapsed())/1000);
        loggingText = QString("Calculated:") + stringTimer + QString("s");
    }


    //Start measuring rendering time
    QElapsedTimer renderingTimer;
    renderingTimer.start();

    //Draw zone of the Mandelbrot Set from newly computed area

    const QPixmap * myPixmap = ui->mandelbrotZoneLabel->pixmap();
    if ( !myPixmap )
    {
        //QImage myImage(iThread->getWidth(), iThread->getHeight(), QImage::Format_RGB32);
        qDebug() << "ERROR - NO IMAGE";
        QSize mySize = ui->mandelbrotZoneLabel->size();
        qDebug() << "        - Create Image of size:" << mySize;
        QImage myImage(mySize, QImage::Format_RGB32);
        ui->mandelbrotZoneLabel->setPixmap(QPixmap::fromImage(myImage));
    }

    myPixmap = ui->mandelbrotZoneLabel->pixmap();
    QImage myImage( myPixmap->toImage() );
    QRgb valueOFF = qRgb(189, 149, 39); // 0xffbd9527
    QRgb valueIN = qRgb(0, 0, 0); // 0xffbd9527

    std::vector<std::vector<MandelbrotSetPoint>> myZone = iThread->getComputedZone();

    for(int i=0; i<iThread->getWidth(); i++) {
        for(int j=0; j<iThread->getHeight(); j++) {
            if (myZone[i][j].isInM) {
                myImage.setPixel(iThread->getOffset() + i, j, valueIN);
            }
            else {
                long double xn = myZone[i][j].xn;
                long double yn = myZone[i][j].yn;
                long double zn = sqrt( xn * xn + yn * yn );
                float hue = myZone[i][j].n - log((log(zn)))/log(2);
                //hue = 0.95 + 80.0 * hue;
                hue=(int)hue%360;
                QColor color;
                color.setHsv(hue,200,200);
                myImage.setPixelColor(iThread->getOffset() + i, j, color);
            }
        }
    }

    ui->mandelbrotZoneLabel->setPixmap(QPixmap::fromImage(myImage));

    renderingTime+=renderingTimer.elapsed();

    if (nbThreadRunning==0) {
        qDebug() << "User perception - TOTAL Rendering completed in (sec):" << float(renderingTime)/1000;
        stringTimer.setNum(float(renderingTime)/1000);
        loggingText = loggingText + QString(" - Rendered:") + stringTimer + QString("s");

        //Status bar management
        statusMessage.setText(loggingText);
        threadRunning = false;

        long double x_min = 1000000, x_max = -1000000, y_min = 1000000, y_max = -1000000;
        int width = 0, height = 0, iter_max = 0;
        for (int i=0; i< _listMandelbrotZoneCalculatorThread.size(); i++) {
            if (_listMandelbrotZoneCalculatorThread[i]->getX_max() > x_max) {x_max = _listMandelbrotZoneCalculatorThread[i]->getX_max();}
            if (_listMandelbrotZoneCalculatorThread[i]->getX_min() < x_min) {x_min = _listMandelbrotZoneCalculatorThread[i]->getX_min();}
            if (_listMandelbrotZoneCalculatorThread[i]->getY_max() > y_max) {y_max = _listMandelbrotZoneCalculatorThread[i]->getY_max();}
            if (_listMandelbrotZoneCalculatorThread[i]->getY_min() < y_min) {y_min = _listMandelbrotZoneCalculatorThread[i]->getY_min();}
            if ((_listMandelbrotZoneCalculatorThread[i]->getOffset() + _listMandelbrotZoneCalculatorThread[i]->getWidth()) > width) {width = (_listMandelbrotZoneCalculatorThread[i]->getOffset() + _listMandelbrotZoneCalculatorThread[i]->getWidth());}
            if (_listMandelbrotZoneCalculatorThread[i]->getHeight() > height) {height = _listMandelbrotZoneCalculatorThread[i]->getHeight();}
            if (_listMandelbrotZoneCalculatorThread[i]->getIter_max() > iter_max) {iter_max = _listMandelbrotZoneCalculatorThread[i]->getIter_max();}
        }

        qDebug() << "User perception - TOTAL zone computed:" << (float)x_min << (float)x_max << (float)y_min << (float)y_max << width << height << iter_max;

        ui->mandelbrotZoneLabel->setZone(x_min,x_max,y_min,y_max,width,height);
        ui->mandelbrotZoneLabel->setIter_max(iter_max);
    }


}
