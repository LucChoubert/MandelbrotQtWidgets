#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_inputsfieldsdialog.h"
#include "mandelbrotzonecalculatorthread.h"
#include <QDebug>
#include <QMessageBox>
#include <QStatusBar>
#include <QPlainTextEdit>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <algorithm>
#include <cmath>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    performanceBenchmark(false),
    threadRunning(false),
    nbThreadRunning(0),
    statusMessage(QString("Welcome into Mandelbrot set beauty")),
    timer(),
    _mandelbrotZoneCalculatorThread(NULL),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->addPermanentWidget(&statusMessage);
    ui->InputFieldsWidget->setVisible(ui->actionDisplay_input_fields->isChecked());
    QObject::connect(ui->mandelbrotZoneLabel, &MandelbrotLabel::mouseMoveHappened, this, &MainWindow::updateMandelbrotZoneCursorPosition);
//    QObject::connect(ui->mandelbrotZoneLabel, &MandelbrotLabel::mouseClickHappened, this, &MainWindow::updateMandelbrotZoneCenter);
//    QObject::connect(ui->mandelbrotZoneLabel, &MandelbrotLabel::mouseWheelHappened, this, &MainWindow::updateMandelbrotZoneZoomAndCenter);
    QObject::connect(ui->mandelbrotZoneLabel, &MandelbrotLabel::mouseClickHappened, this, &MainWindow::updateMandelbrotZoneZoomAndCenter);
    QObject::connect(ui->mandelbrotZoneLabel, &MandelbrotLabel::mouseWheelHappened, this, &MainWindow::updateMandelbrotZoneZoom);
    QObject::connect(ui->actionDisplay_input_fields, &QAction::triggered, this, &MainWindow::actionDisplay_input_fields);
    QObject::connect(ui->actionInput_fields_Popup, &QAction::triggered, this, &MainWindow::actionInput_fields_Popup);

    mandelbrotSetDefinition.x0 = ui->x0LineEdit->text().toDouble();
    mandelbrotSetDefinition.y0 = ui->y0LineEdit->text().toDouble();
    mandelbrotSetDefinition.iter_max = ui->iterationsLineEdit->text().toInt();
    mandelbrotSetDefinition.zoom = ui->zoomLineEdit->text().toFloat();

    mandelbrotSetRenderingMethod.rendering_method_name = ui->coloringComboBox->currentText();
    //TODO - specifc parameter for coloring
    mandelbrotSetRenderingMethod.parameter1 = 0 ;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::actionDisplay_input_fields()
{
    ui->InputFieldsWidget->setVisible(ui->actionDisplay_input_fields->isChecked());
}

void MainWindow::actionInput_fields_Popup()
{
    qDebug() << "Popup Opening... ";
    QDialog *aPopup = new QDialog();
    Ui::InputFieldsDialog aInputFieldsDialog;
    aInputFieldsDialog.setupUi(aPopup);
    aPopup->show();
}

QString MainWindow::getStringFromLongDouble(const long double iLongDouble)
{
  std::stringstream ss;
  ss.precision(50);
  ss << iLongDouble;

  return QString::fromStdString(ss.str());
}

void MainWindow::updateMandelbrotSetDefinitionPanel() {
    QString stringX, stringY, stringZoom, stringIter;

    //stringX.setNum((float)mandelbrotSetDefinition.x0);
    stringX = getStringFromLongDouble(mandelbrotSetDefinition.x0);
    ui->x0LineEdit->setText(stringX);

    //stringY.setNum((float)mandelbrotSetDefinition.y0);
    stringY = getStringFromLongDouble(mandelbrotSetDefinition.y0);
    ui->y0LineEdit->setText(stringY);

    stringZoom.setNum((float)mandelbrotSetDefinition.zoom);
    ui->zoomLineEdit->setText(stringZoom);

    stringIter.setNum((int)mandelbrotSetDefinition.iter_max);
    ui->iterationsLineEdit->setText(stringIter);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "Key pressed - " << event->key();
    switch (event->key()) {
    case Qt::Key_Left:
        qDebug() << "Key pressed - " << "LEFT";
        moveMandelbrotZoneHorizontaly(-10);
        break;

    case Qt::Key_Right:
        qDebug() << "Key pressed - " << "RIGHT";
        moveMandelbrotZoneHorizontaly(10);
        break;

    case Qt::Key_Up:
        qDebug() << "Key pressed - " << "UP";
        moveMandelbrotZoneVerticaly(-10);
        break;

    case Qt::Key_Down:
        qDebug() << "Key pressed - " << "DOWN";
        moveMandelbrotZoneVerticaly(10);
        break;

    case Qt::Key_Plus:
        //qDebug() << "Key pressed - " << "+";
        updateMandelbrotZoneZoom(1);
        break;

    case Qt::Key_Minus:
        //qDebug() << "Key pressed - " << "-";
        updateMandelbrotZoneZoom(-1);
        break;

    case Qt::Key_PageUp:
        //qDebug() << "Key pressed - " << "Key_PageUp";
        updateMandelbrotZoneIterMax(1);
        break;

    case Qt::Key_PageDown:
        //qDebug() << "Key pressed - " << "Key_PageDown";
        updateMandelbrotZoneIterMax(-1);
        break;
    }

    // now call parent event handler, necessary?
    QMainWindow::keyPressEvent(event);
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

void MainWindow::on_coloringComboBox_currentIndexChanged(const QString &iText)
{
    //qDebug() << "Coloring method updated:" << iText;

    mandelbrotSetRenderingMethod.rendering_method_name = ui->coloringComboBox->currentText();
    //TODO - specifc parameter for coloring
    mandelbrotSetRenderingMethod.parameter1 = 0 ;

    //ui->mandelbrotZoneLabel->repaint();
    renderMandelbrot();
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
    mandelbrotSetDefinition.x0 = ui->x0LineEdit->text().toDouble(&ok1);
    mandelbrotSetDefinition.y0 = ui->y0LineEdit->text().toDouble(&ok2);
    mandelbrotSetDefinition.iter_max = ui->iterationsLineEdit->text().toInt(&ok3);
    mandelbrotSetDefinition.zoom = ui->zoomLineEdit->text().toFloat(&ok4);

    if (!ok1 || !ok2 || !ok3 || !ok4)
    {
        QMessageBox messageBox;
        messageBox.setText("One of your entries is not a valid number.");
        messageBox.setWindowTitle("Error");
        messageBox.exec();
    }

    updateMandelbrotSetDefinitionPanel();
    ui->mandelbrotZoneLabel->repaint();
}

void MainWindow::updateMandelbrotZoneCursorPosition(PrecisionPoint position)
{
    //qDebug() << "updateMandelbrotZoneCursorPosition:" << position;
    QString stringX, stringY, stringN;
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

void MainWindow::updateMandelbrotZoneZoom(int zoomFactor)
{
    //qDebug() << "updateMandelbrotZoneZoom:" << zoomFactor;

    mandelbrotSetDefinition.zoom += zoomFactor*0.5;

    updateMandelbrotSetDefinitionPanel();
    ui->mandelbrotZoneLabel->repaint();
}

void MainWindow::updateMandelbrotZoneZoomAndCenter(PrecisionPoint position, int zoomFactor)
{
    //qDebug() << "updateMandelbrotZoneZoomAndCenter:" << (float)position.x << (float)position.y << zoomFactor;

    mandelbrotSetDefinition.x0 = position.x;
    mandelbrotSetDefinition.y0 = position.y;
    mandelbrotSetDefinition.zoom += zoomFactor*0.5;

    updateMandelbrotSetDefinitionPanel();
    ui->mandelbrotZoneLabel->repaint();
}

void MainWindow::updateMandelbrotZoneIterMax(int iterFactor)
{
    //qDebug() << "updateMandelbrotZoneIterMax:" << iterFactor;

    mandelbrotSetDefinition.iter_max += iterFactor*1000;
    if (mandelbrotSetDefinition.iter_max < 500) {mandelbrotSetDefinition.iter_max = 500;}

    updateMandelbrotSetDefinitionPanel();
    ui->mandelbrotZoneLabel->repaint();
}

void MainWindow::moveMandelbrotZoneHorizontaly(int factor)
{
    long double delta = pow(2.0,(-mandelbrotSetDefinition.zoom));
    mandelbrotSetDefinition.x0 += (delta*factor)/100;

    updateMandelbrotSetDefinitionPanel();
    ui->mandelbrotZoneLabel->repaint();
}

void MainWindow::moveMandelbrotZoneVerticaly(int factor)
{
    long double delta = pow(2.0,(-mandelbrotSetDefinition.zoom));
    mandelbrotSetDefinition.y0 += (delta*factor)/100;

    updateMandelbrotSetDefinitionPanel();
    ui->mandelbrotZoneLabel->repaint();
}

QString MainWindow::getStatusMessagePrefix() {
    QString loggingText, statusText;
    statusText = statusText + QString("iter_max=") + loggingText.setNum(mandelbrotSetDefinition.iter_max) + QString("/");
    statusText = statusText + QString("zoom=") + loggingText.setNum((float)mandelbrotSetDefinition.zoom) + QString("/");
    //statusText = statusText + QString("x0=") + loggingText.setNum((float)mandelbrotSetDefinition.x0) + QString("/");
    //statusText = statusText + QString("y0=") + loggingText.setNum((float)mandelbrotSetDefinition.y0) + QString("/");
    statusText = statusText + QString("x0=") + getStringFromLongDouble(mandelbrotSetDefinition.x0) + QString("/");
    statusText = statusText + QString("y0=") + getStringFromLongDouble(mandelbrotSetDefinition.y0) + QString("/");
    return statusText;
}

void MainWindow::computeMandelbrot()
{
    QSize mySize = ui->mandelbrotZoneLabel->size();

    long double x0 = mandelbrotSetDefinition.x0;
    long double y0 = mandelbrotSetDefinition.y0;
    float zoom = mandelbrotSetDefinition.zoom;
    int iter_max = mandelbrotSetDefinition.iter_max;

    long double half_range = pow(2.0,(-zoom-1));
    long double x_half_range = half_range * (mySize.width()) / std::min(mySize.width(), mySize.height());
    long double y_half_range = half_range * (mySize.height()) / std::min(mySize.width(), mySize.height());
    long double x_min = x0 - x_half_range;
    long double x_max = x0 + x_half_range;
    long double y_min = y0 - y_half_range;
    long double y_max = y0 + y_half_range;

    if (ui->mandelbrotZoneLabel->isSameZone(x_min,x_max,y_min,y_max,mySize.width(),mySize.height(),iter_max) &&
        ui->mandelbrotZoneLabel->isSameRendering_method(mandelbrotSetRenderingMethod)    )
    {
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

    updateMandelbrotSetDefinitionPanel();

    //mandelbrotZoneLabel doest not have the right pixmap size so let's recreate one with the appropriate size
    if (!ui->mandelbrotZoneLabel->isSameSize(mySize.width(),mySize.height())) {
        QImage myImage(mySize, QImage::Format_RGB32);
        ui->mandelbrotZoneLabel->setPixmap(QPixmap::fromImage(myImage));
    }

    qDebug() << "============= Mandelbrot Set Area: =============";
    qDebug() << " Max Iteration: " << iter_max;
    qDebug() << "        Center: " << getStringFromLongDouble(x0) << " " << getStringFromLongDouble(y0);
    qDebug() << "   Zoom factor: " << (float)(zoom);
    qDebug() << "  Width (2^-p): " << (float)(2*half_range);
    qDebug() << " Height (2^-p): " << (float)(2*half_range);
    qDebug() << "=============     Window Area:     =============";
    qDebug() << "Mandelbrot Widget Size" << mySize;
    qDebug() << "======== Mandelbrot ADJUSTED Set Area: ========";
    qDebug() << "             X: " << getStringFromLongDouble(x_min) << " " << getStringFromLongDouble(x_max);
    qDebug() << "             Y: " << getStringFromLongDouble(y_min) << " " << getStringFromLongDouble(y_max);
    qDebug() << "================================================";

    //Status bar management
    //statusText+=getStatusMessagePrefix();
    //statusText+=QString(" - Calculation Running");
    //statusMessage.setText(loggingText.setNum(iter_max) + QString(" ") + loggingText.setNum((float)zoom) + QString(" ") + loggingText.setNum((float)x0) + QString(" ") + loggingText.setNum((float)y0) + QString(" - Calculation Running"));
    //statusMessage.setText(QString("Calculation Running"));
    statusMessage.setText(getStatusMessagePrefix() + QString(" - Calculation Running"));

    //Reset rendering time value
    renderingTime = 0;

    //Start measuring calculation time
    timer.start();

    // Allocate thread manager objects if not done already

    int nBThreads = QThread::idealThreadCount();

    if (_listMandelbrotZoneCalculatorThread.empty()) {
        qDebug() << "Creating" << nBThreads << "threads";
        for (int i=0; i<nBThreads; i++) {
            MandelbrotZoneCalculatorThread * myThread = new MandelbrotZoneCalculatorThread();
            _listMandelbrotZoneCalculatorThread.push_back(myThread);
            QObject::connect(myThread, &MandelbrotZoneCalculatorThread::zoneComputationCompleted, this, &MainWindow::receiveZoneComputationCompleted);
        }
    }

    // split the zone in smaller zones based on the number of available threads, set configuration of the threads, and start them
    nBThreads = _listMandelbrotZoneCalculatorThread.size();
    qDebug() << "Running on" << nBThreads << "threads";
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

void MainWindow::receiveZoneComputationCompleted(MandelbrotZoneCalculatorThread * iThread)
{
    //Count reference of active threads
    nbThreadRunning--;

    //qDebug() << "Signal received - Calculation Thread Completed for widget zone size " << iThread->getOffset() << iThread->getWidth() << iThread->getHeight()  ;

    if (nbThreadRunning==0) {
        renderMandelbrot();
    }
}

void MainWindow::renderMandelbrot()
{

    //qDebug() << "User perception - TOTAL Calculation completed in (sec):" << float(timer.elapsed())/1000;

    //Compute and prepare display in status bar of total time of calculation
    QString loggingText, stringTimer;
    stringTimer.setNum(float(timer.elapsed())/1000);
    loggingText = QString(" - Calculated:") + stringTimer + QString("s");

    //We now have all the thread completed, we can start rendering
    //Start measuring rendering time
    QElapsedTimer renderingTimer;
    renderingTimer.start();


    // Make sure pixmap is ready. May no more be usefull due to preliminary preparation done in paint event
    const QPixmap * myPixmap = ui->mandelbrotZoneLabel->pixmap();
    if ( !myPixmap )
    {
        qDebug() << "ERROR - NO IMAGE";
        QSize mySize = ui->mandelbrotZoneLabel->size();
        qDebug() << "        - Create Image of size:" << mySize;
        QImage myImage(mySize, QImage::Format_RGB32);
        ui->mandelbrotZoneLabel->setPixmap(QPixmap::fromImage(myImage));
    }
    myPixmap = ui->mandelbrotZoneLabel->pixmap();
    QImage myImage( myPixmap->toImage() );
    qDebug() << "Creating Image of size:" << myImage.size();
    QRgb valueOFF = qRgb(189, 149, 39); // 0xffbd9527
    QRgb valueIN = qRgb(0, 0, 0); // 0xffbd9527

    //Go through the full data set to get max iteration number
    int real_iter_max = 0;
    long long int avg_iter_max = 0;
    long long int stddev_iter_max = 0;
    long long int nb_iter_max = 0;
    for (int k=0; k< _listMandelbrotZoneCalculatorThread.size(); k++) {
        //Draw zone of the Mandelbrot Set from newly computed area
        MandelbrotZoneCalculatorThread *aThread = _listMandelbrotZoneCalculatorThread[k];
        std::vector<std::vector<MandelbrotSetPoint>> myZone = aThread->getComputedZone();
        for(int i=0; i<aThread->getWidth(); i++) {
            for(int j=0; j<aThread->getHeight(); j++) {
                //get the max only
                if (!myZone[i][j].isInM && real_iter_max < myZone[i][j].n) {
                    real_iter_max = myZone[i][j].n;
                }
                //prepare for average calculation
                nb_iter_max++;
                avg_iter_max+=myZone[i][j].n;
            }
        }
    }
    avg_iter_max/=nb_iter_max;
    nb_iter_max = 0;
    for (int k=0; k< _listMandelbrotZoneCalculatorThread.size(); k++) {
        //Draw zone of the Mandelbrot Set from newly computed area
        MandelbrotZoneCalculatorThread *aThread = _listMandelbrotZoneCalculatorThread[k];
        std::vector<std::vector<MandelbrotSetPoint>> myZone = aThread->getComputedZone();
        for(int i=0; i<aThread->getWidth(); i++) {
            for(int j=0; j<aThread->getHeight(); j++) {
                //get the max only
                if (!myZone[i][j].isInM && real_iter_max < myZone[i][j].n) {
                    real_iter_max = myZone[i][j].n;
                }
                //prepare for stddev calculation
                nb_iter_max++;
                stddev_iter_max+=pow((myZone[i][j].n - avg_iter_max),2);
            }
        }
    }
    stddev_iter_max=sqrt(stddev_iter_max/nb_iter_max);
    qDebug() << "real, average, stddev iter_max:" << real_iter_max << avg_iter_max << stddev_iter_max;

    // Render each pixel
    for (int k=0; k< _listMandelbrotZoneCalculatorThread.size(); k++) {
        //Draw zone of the Mandelbrot Set from newly computed area
        MandelbrotZoneCalculatorThread *aThread = _listMandelbrotZoneCalculatorThread[k];
        std::vector<std::vector<MandelbrotSetPoint>> myZone = aThread->getComputedZone();
        for(int i=0; i<aThread->getWidth(); i++) {
            for(int j=0; j<aThread->getHeight(); j++) {
                if (myZone[i][j].isInM) {
                    myImage.setPixel(aThread->getOffset() + i, j, valueIN);
                }
                else {
                    long double xn = myZone[i][j].xn;
                    long double yn = myZone[i][j].yn;
                    int n = myZone[i][j].n;
                    //QColor color = computeColor(xn, yn, n, mandelbrotSetDefinition.iter_max);
                    QColor color = computeColor(xn, yn, n, real_iter_max);
                    //QColor color = computeColor(xn, yn, std::min((long long int)real_iter_max, n/avg_iter_max*real_iter_max/2), real_iter_max);
                    myImage.setPixelColor(aThread->getOffset() + i, j, color);
                }
            }
        }
    }

    //Display now the image completely built
    qDebug() << "Having created Image of size:" << myImage.size();
    ui->mandelbrotZoneLabel->setPixmap(QPixmap::fromImage(myImage));
    //qDebug() << "User perception - TOTAL Rendering completed in (sec):" << float(renderingTimer.elapsed())/1000;

    // Compute total rendering time and finalize preparation of text for status bar
    stringTimer.setNum(float(renderingTimer.elapsed())/1000);
    loggingText = loggingText + QString(" + Rendered:") + stringTimer + QString("s");

    //Display all calculation time in Status bar
    statusMessage.setText(getStatusMessagePrefix() + loggingText);

    //Store that all threads have now completed
    threadRunning = false;

    //Aggregate the n zones areas to store that and to use that to compare with new repaint event
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
    //qDebug() << "User perception - TOTAL zone computed:" << (float)x_min << (float)x_max << (float)y_min << (float)y_max << width << height << iter_max;
    ui->mandelbrotZoneLabel->setZone(x_min,x_max,y_min,y_max,width,height);
    ui->mandelbrotZoneLabel->setIter_max(iter_max);
    ui->mandelbrotZoneLabel->setRendering_method(mandelbrotSetRenderingMethod);

    if (performanceBenchmark) {
        benchmarkPerformance();
    }
}

QColor MainWindow::computeColor(long double ix, long double iy, int in, int in_max) {

    QString selectedMode = ui->coloringComboBox->currentText();

    if (selectedMode == "Continuous") {
        long double log_zn = log( ix * ix + iy * iy ) / 2;
        //long double zn = sqrt( ix * ix + iy * iy );
        float hue = in + 1 - log( log_zn/log(2) ) / log(2);
        hue=(int)hue%360;
        QColor color;
        color.setHsv(hue,255,255);
        return color;
    }


    if (selectedMode == "Continuous 2") {
        long double log_zn = log( ix * ix + iy * iy ) / 2;
        long double nu = in + 1 - log( log_zn / log(2) );
        double fractpart, intpart;
        fractpart = modf((double)(nu*3), &intpart);

        float hue1 = ((int)(intpart))%360;
        float hue2 = ((int)(intpart+1))%360;
        float hue = hue1 + (hue2 - hue1)*fractpart;
        QColor color;
        color.setHsv(hue,255,255);
        return color;
    }

    if (selectedMode == "Continuous 3") {
        long double log_zn = log( ix * ix + iy * iy ) / 2;
        long double hue = in + 1 - log( (log_zn/log(2) )) / log(2);
        hue = 0.95 + 20.0 * hue;
        hue=(int)hue%360;
        QColor color;
        color.setHsv(hue,255,255);
        return color;
    }

    if (selectedMode == "Escape time") {
        float hue = (float)in/in_max;
        hue = (int)floor(hue*359);
        //qDebug() << in << mandelbrotSetDefinition.iter_max << hue;
        QColor color;
        color.setHsv(hue,255,255);
        return color;
    }

    if (selectedMode == "Histogram") {
        float hue = (float)in/in_max;
        hue = (int)floor(hue*359);
        //qDebug() << in << mandelbrotSetDefinition.iter_max << hue;
        QColor color;
        color.setHsv(hue,255,255);
        return color;
    }

    QColor color;
    color.setHsv(0,0,0);
    return color;

}

void MainWindow::benchmarkPerformance() {
    if (mandelbrotSetDefinition.zoom >= 33) {
        QApplication::exit();
    }

    performanceBenchmark = true;
    mandelbrotSetDefinition.x0 = -0.91659909584151357729532236251479559996369061991572;
    mandelbrotSetDefinition.y0 = 0.31500558725038855641619427172539502635117969475687;
    mandelbrotSetDefinition.iter_max = 5000;
    updateMandelbrotZoneZoom(1);
    computeMandelbrot();
}
