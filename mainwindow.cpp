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
        bool activeMultithread = false;

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
            qDebug() << "SKIPPED CALCULATION - Thread already runing";
            return;
        }
        else {
            threadRunning = true;
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

        //Start measuring calculation time
        timer.start();

        if (!activeMultithread) {

            // New calculation needed, let's destroy and create a new thread
            //if (_mandelbrotZoneCalculatorThread!=NULL && _mandelbrotZoneCalculatorThread->isFinished()){
            if (_mandelbrotZoneCalculatorThread!=NULL) {
                delete _mandelbrotZoneCalculatorThread;
                _mandelbrotZoneCalculatorThread=NULL;
            }

            // Start the thread
            _mandelbrotZoneCalculatorThread = new MandelbrotZoneCalculatorThread(x_min,x_max,y_min,y_max,mySize.width(),mySize.height(),iter_max);
            QObject::connect(_mandelbrotZoneCalculatorThread, &MandelbrotZoneCalculatorThread::zoneComputationCompleted, this, &MainWindow::renderMandelbrot);
            _mandelbrotZoneCalculatorThread->start();
        }
        else {
        //MultiThread activated

        }
    }
}

void MainWindow::renderMandelbrot()
{
    QString loggingText, stringTimer;
    qDebug() << "Signal received - Calculation Thread Completed for widget size " << _mandelbrotZoneCalculatorThread->getWidth() << _mandelbrotZoneCalculatorThread->getHeight()  ;
    qDebug() << "User perception - Calculation completed in (sec):" << float(timer.elapsed())/1000;
    stringTimer.setNum(float(timer.elapsed())/1000);
    loggingText = QString("Calculated:") + stringTimer + QString("s");

    //Start measuring rendering time
    timer.start();

    //Draw Mandelbrot Set from newly computed area
    //QImage myImage;
    //myImage.load("/home/luc/QtProjects/MandelbrotQtWidgets/images/mandelbrot-1.png");
    QImage myImage(_mandelbrotZoneCalculatorThread->getWidth(), _mandelbrotZoneCalculatorThread->getHeight(), QImage::Format_RGB32);
    QRgb valueOFF = qRgb(189, 149, 39); // 0xffbd9527
    QRgb valueIN = qRgb(0, 0, 0); // 0xffbd9527

    std::vector<std::vector<QPair<bool, int>>> myZone = _mandelbrotZoneCalculatorThread->getComputedZone();
    std::vector<std::vector<MandelbrotPoint>> myZone2 = _mandelbrotZoneCalculatorThread->getComputedZone2();

    for(int i=0; i<_mandelbrotZoneCalculatorThread->getWidth(); i++) {
        for(int j=0; j<_mandelbrotZoneCalculatorThread->getHeight(); j++) {
            if (myZone2[i][j].isInM) {
                myImage.setPixel(i, j, valueIN);
            }
            else {
//                float zoom = ui->zoomLineEdit->text().toFloat();
//                float x0 = ui->x0LineEdit->text().toFloat();
//                float y0 = ui->y0LineEdit->text().toFloat();
//                float half_range = pow(2.0,(-zoom-1));
//                float x_min = x0 - half_range;
//                float x_max = x0 + half_range;
//                float y_min = y0 - half_range;
//                float y_max = y0 + half_range;
//                float x = x_min + i * (x_max - x_min)/_mandelbrotZoneCalculatorThread->getWidth();
//                float y = y_min + j * (y_max - y_min)/_mandelbrotZoneCalculatorThread->getHeight();
                float xn = myZone2[i][j].xn;
                float yn = myZone2[i][j].yn;
                float zn = sqrt( xn * xn + yn * yn );
                float hue = myZone2[i][j].n - log((log(zn)))/log(2);
                //hue = 0.95 + 80.0 * hue;
                hue=(int)hue%360;
                QColor color;
                color.setHsv(hue,200,200);
                myImage.setPixelColor(i, j, color);
            }
        }
    }

    ui->mandelbrotZoneLabel->setZone(_mandelbrotZoneCalculatorThread->getX_min(),_mandelbrotZoneCalculatorThread->getX_max(),_mandelbrotZoneCalculatorThread->getY_min(),_mandelbrotZoneCalculatorThread->getY_max(),_mandelbrotZoneCalculatorThread->getWidth(),_mandelbrotZoneCalculatorThread->getHeight());
    ui->mandelbrotZoneLabel->setIter_max(_mandelbrotZoneCalculatorThread->getIter_max());
    ui->mandelbrotZoneLabel->setPixmap(QPixmap::fromImage(myImage));
    threadRunning = false;

    //
    qDebug() << "User perception - Rendering completed in (sec):" << float(timer.elapsed())/1000;
    stringTimer.setNum(float(timer.elapsed())/1000);
    loggingText = loggingText + QString(" - Rendered:") + stringTimer + QString("s");

    //Status bar management
    //ui->statusBar->showMessage(loggingText);
    statusMessage.setText(loggingText);
}
