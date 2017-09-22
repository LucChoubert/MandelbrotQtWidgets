#ifndef MANDELBROTLABEL_H
#define MANDELBROTLABEL_H

#include <QLabel>

class MandelbrotLabel : public QLabel
{
    Q_OBJECT

public:
    explicit MandelbrotLabel(QWidget *parent = nullptr);
    void setZone(float ix_min, float ix_max, float iy_min, float iy_max, int iwidth_pixel, int iheight_pixel);
    void setIter_max(int iiter_max);
    bool isSameZone(float ix_min, float ix_max, float iy_min, float iy_max, int iwidth_pixel, int iheight_pixel, int iiter_max);
    bool isSameSize(int iwidth_pixel, int iheight_pixel);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    QPointF convertScreenPositionToRealPosition(QPoint iPosition);

signals:
    void mouseMoveHappened(QPointF position);
    void mouseClickHappened(QPointF position);
    void mouseWheelHappened(QPointF position, int zoomFactor);

public slots:

private:
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    int width_pixel;
    int height_pixel;
    //temporary until iter_max is fully dynamic
    int iter_max;
};

#endif // MANDELBROTLABEL_H
