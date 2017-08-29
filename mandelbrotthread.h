#ifndef MANDELBROTTHREAD_H
#define MANDELBROTTHREAD_H

#include <QObject>
#include <QThread>

class MandelbrotThread : public QThread
{
    Q_OBJECT

public:
    MandelbrotThread();
    ~MandelbrotThread();

protected:
    void run();

};

#endif // MANDELBROTTHREAD_H
