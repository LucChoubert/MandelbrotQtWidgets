#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
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
    void computeMandelbrot();

private slots:
    void on_quitButton_clicked();
    void on_computeButton_clicked();
    void renderMandelbrot(MandelbrotZoneCalculatorThread * iThread);
    void updateMandelbrotZoneCursorPosition(QPointF position);
    void updateMandelbrotZoneCenter(QPointF position);
    void updateMandelbrotZoneZoomAndCenter(QPointF position, int zoomFactor);

private:
    bool threadRunning;
    int nbThreadRunning;
    QLabel statusMessage;
    QElapsedTimer timer;
    qint64 renderingTime;
    MandelbrotZoneCalculatorThread *_mandelbrotZoneCalculatorThread;
    QList<MandelbrotZoneCalculatorThread *> _listMandelbrotZoneCalculatorThread;
    Ui::MainWindow *ui;

};


#endif // MAINWINDOW_H
