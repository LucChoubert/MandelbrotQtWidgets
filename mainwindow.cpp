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
    computeMandelbrot();
}

void MainWindow::updateMandelbrotZoneCursorPosition(QPointF position)
{
    //qDebug() << "updateMandelbrotZoneCursorPosition:" << position;
    QString stringX, stringY;
    stringX.setNum(position.x());
    stringY.setNum(position.y());
    ui->statusBar->showMessage(QString("Cursor:(") + stringX + QString(",") + stringY + QString(")"),2500);

}

void MainWindow::updateMandelbrotZoneCenter(QPointF position)
{
    //qDebug() << "updateMandelbrotZoneCenter:" << position;
    QString stringX, stringY;
    stringX.setNum(position.x());
    stringY.setNum(position.y());
    ui->x0LineEdit->setText(stringX);
    ui->y0LineEdit->setText(stringY);
}

void MainWindow::updateMandelbrotZoneZoomAndCenter(QPointF position, int zoomFactor)
{
    qDebug() << "updateMandelbrotZoneZoomAndCenter:" << position << zoomFactor;
    QString stringX, stringY, stringZoom;
    stringX.setNum(position.x());
    stringY.setNum(position.y());
    float zoom = ui->zoomLineEdit->text().toFloat();
    stringZoom.setNum(zoom+zoomFactor*0.5);
    ui->x0LineEdit->setText(stringX);
    ui->y0LineEdit->setText(stringY);
    ui->zoomLineEdit->setText(stringZoom);
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
        bool activeMultithread = true;

        float half_range = pow(2.0,(-zoom-1));
        float x_min = x0 - half_range;
        float x_max = x0 + half_range;
        float y_min = y0 - half_range;
        float y_max = y0 + half_range;
        QSize mySize = ui->mandelbrotZoneLabel->size();

        if (ui->mandelbrotZoneLabel->isSameZone(x_min,x_max,y_min,y_max,mySize.width(),mySize.height(),iter_max)) {
            qDebug() << "SKIPPED CALCULATION - Same Area";
            return;
        }

        if (threadRunning) {
            qDebug() << "SKIPPED CALCULATION - Calculation Threads already running";
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
        qDebug() << "        Center: " << x0 << " " << y0;
        qDebug() << "  Width (2^-p): " << 2*half_range;
        qDebug() << " Height (2^-p): " << 2*half_range;
        qDebug() << "             X: " << x_min << " " << x_max;
        qDebug() << "             Y: " << y_min << " " << y_max;
        qDebug() << "========     Window Area:     ========";
        qDebug() << "Mandelbrot Widget Size" << mySize;
        qDebug() << "======================================";

        //Status bar management
        statusMessage.setText(QString("Calculation Running"));

        //Reset rendering time value
        renderingTime = 0;

        //Start measuring calculation time
        timer.start();

        if (!activeMultithread) {
            //MultiThread NOT activated
            if (_listMandelbrotZoneCalculatorThread.empty()) {
                MandelbrotZoneCalculatorThread mandelbrotZoneCalculatorThread;
                //_listMandelbrotZoneCalculatorThread.push_back(mandelbrotZoneCalculatorThread);
            }

            // New calculation needed, let's destroy and create a new thread
            //if (_mandelbrotZoneCalculatorThread!=NULL && _mandelbrotZoneCalculatorThread->isFinished()){
            if (_mandelbrotZoneCalculatorThread!=NULL) {
                delete _mandelbrotZoneCalculatorThread;
                _mandelbrotZoneCalculatorThread=NULL;
            }

            // Start the thread
            nbThreadRunning = 1;
            _mandelbrotZoneCalculatorThread = new MandelbrotZoneCalculatorThread(x_min,x_max,y_min,y_max,mySize.width(),mySize.height(),iter_max);
            QObject::connect(_mandelbrotZoneCalculatorThread, &MandelbrotZoneCalculatorThread::zoneComputationCompleted, this, &MainWindow::renderMandelbrot);
            _mandelbrotZoneCalculatorThread->start();
        }
        else {
            //MultiThread activated
            // Allocate thread manager objects if not done already

            //int nBThreads = QThread::idealThreadCount();
            int nBThreads = 4;

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
                float zone_x_min = x_min + i * (x_max - x_min) / nBThreads;
                float zone_x_max = x_min + (i + 1) * (x_max - x_min) / nBThreads;
                int zone_width = std::min(pixel_range, mySize.width() - zone_offset) ;
                int zone_height = mySize.height();
                _listMandelbrotZoneCalculatorThread[i]->setCalculationDetails(zone_x_min,zone_x_max,y_min,y_max,zone_width,zone_height,iter_max,zone_offset);
                _listMandelbrotZoneCalculatorThread[i]->start();
                zone_offset += zone_width;
            }

            //

        }
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

    std::vector<std::vector<QPair<bool, int>>> myZone = iThread->getComputedZone();
    std::vector<std::vector<MandelbrotPoint>> myZone2 = iThread->getComputedZone2();

    for(int i=0; i<iThread->getWidth(); i++) {
        for(int j=0; j<iThread->getHeight(); j++) {
            if (myZone2[i][j].isInM) {
                myImage.setPixel(iThread->getOffset() + i, j, valueIN);
            }
            else {
                float xn = myZone2[i][j].xn;
                float yn = myZone2[i][j].yn;
                float zn = sqrt( xn * xn + yn * yn );
                float hue = myZone2[i][j].n - log((log(zn)))/log(2);
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

        float x_min = 1000000, x_max = -1000000, y_min = 1000000, y_max = -1000000;
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

        qDebug() << "User perception - TOTAL zone computed:" << x_min << x_max << y_min << y_max << width << height << iter_max;

        ui->mandelbrotZoneLabel->setZone(x_min,x_max,y_min,y_max,width,height);
        ui->mandelbrotZoneLabel->setIter_max(iter_max);
    }


}
