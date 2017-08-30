#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mandelbrotthread.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
        MandelbrotThread *myThread = new MandelbrotThread(x_min,x_max,y_min,y_max,600,iter_max);
        myThread->start();
    }
}
