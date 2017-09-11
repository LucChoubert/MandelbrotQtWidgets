#ifndef MANDELBROTLABEL_H
#define MANDELBROTLABEL_H

#include <QLabel>

class MandelbrotLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MandelbrotLabel(QWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

signals:

public slots:
};

#endif // MANDELBROTLABEL_H
