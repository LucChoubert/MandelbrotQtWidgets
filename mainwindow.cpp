#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mandelbrotthread.h"

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
    MandelbrotThread *myThread = new MandelbrotThread;
    myThread->start();
}
