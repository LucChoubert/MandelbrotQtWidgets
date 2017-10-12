#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <basictypes.h>
#include <QList>
#include <QLabel>
#include <QMainWindow>
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
    void benchmarkPerformance();
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void computeMandelbrot();
    QString getStatusMessagePrefix();
    QString getStringFromLongDouble(const long double iLongDouble);

private slots:
    void on_quitButton_clicked();
    void on_computeButton_clicked();
    void updateMandelbrotSetDefinitionPanel();
    void renderMandelbrot(MandelbrotZoneCalculatorThread * iThread);
    void updateMandelbrotZoneCursorPosition(PrecisionPoint position);
    void updateMandelbrotZoneCenter(PrecisionPoint position);
    void updateMandelbrotZoneZoom(int zoomFactor);
    void updateMandelbrotZoneIterMax(int iterFactor);
    void updateMandelbrotZoneZoomAndCenter(PrecisionPoint position, int zoomFactor);
    void moveMandelbrotZoneHorizontaly(int factor);
    void moveMandelbrotZoneVerticaly(int factor);
    void actionDisplay_input_fields();
    void actionInput_fields_Popup();

private:
    bool performanceBenchmark;
    MandelbrotSetDefinition mandelbrotSetDefinition;
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
