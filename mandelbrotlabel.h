#ifndef MANDELBROTLABEL_H
#define MANDELBROTLABEL_H

#include <basictypes.h>
#include <QLabel>

class MandelbrotLabel : public QLabel
{
    Q_OBJECT

public:
    explicit MandelbrotLabel(QWidget *parent = nullptr);
    void setZone(long double ix_min, long double ix_max, long double iy_min, long double iy_max, int iwidth_pixel, int iheight_pixel);
    void setIter_max(int iiter_max);
    void setRendering_method(MandelbrotSetRenderingMethod irendering_method);
    bool isSameZone(long double ix_min, long double ix_max, long double iy_min, long double iy_max, int iwidth_pixel, int iheight_pixel, int iiter_max);
    bool isSameSize(int iwidth_pixel, int iheight_pixel);
    bool isSameRendering_method(MandelbrotSetRenderingMethod irendering_method);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    PrecisionPoint convertScreenPositionToRealPosition(QPoint iPosition);

signals:
    void mouseMoveHappened(PrecisionPoint position);
//    void mouseClickHappened(PrecisionPoint position);
//    void mouseWheelHappened(PrecisionPoint position, int zoomFactor);
    void mouseClickHappened(PrecisionPoint position, int zoomFactor);
    void mouseWheelHappened(int zoomFactor);

public slots:

private:
    long double x_min;
    long double x_max;
    long double y_min;
    long double y_max;
    int width_pixel;
    int height_pixel;
    //temporary until iter_max is fully dynamic
    int iter_max;
    MandelbrotSetRenderingMethod rendering_method;
};

#endif // MANDELBROTLABEL_H
