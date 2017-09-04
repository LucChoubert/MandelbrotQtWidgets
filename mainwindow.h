#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <mandelbrotzonecalculatorthread.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_quitButton_clicked();
    void on_computeButton_clicked();
    void renderMandelbrot();

private:
    Ui::MainWindow *ui;
    MandelbrotZoneCalculatorThread *_mandelbrotZoneCalculatorThread;

};


#endif // MAINWINDOW_H
