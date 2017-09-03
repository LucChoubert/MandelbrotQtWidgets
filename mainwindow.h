#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <mandelbrotthread.h>

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
    MandelbrotThread *_mandelbrotThread;

};


#endif // MAINWINDOW_H
