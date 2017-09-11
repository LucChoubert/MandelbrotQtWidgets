#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QElapsedTimer>
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

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void on_quitButton_clicked();
    void on_computeButton_clicked();
    void computeMandelbrot();
    void renderMandelbrot();

private:
    bool threadRunning;
    QLabel statusMessage;
    QElapsedTimer timer;
    MandelbrotZoneCalculatorThread *_mandelbrotZoneCalculatorThread;
    Ui::MainWindow *ui;

};


#endif // MAINWINDOW_H
