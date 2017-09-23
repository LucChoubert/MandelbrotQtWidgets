#ifndef BASICTYPES_H
#define BASICTYPES_H

struct PrecisionPoint {
    // Coordinate of the point
    long double x;
    long double y;
};
typedef struct MandelbrotPoint MandelbrotPoint;

struct MandelbrotSetPoint {
    // Coordinate of the point
    long double x;
    long double y;
    // Iter reach in Mandelbrot suite
    int n;
    // Coordinate of the nth iteration point
    long double xn;
    long double yn;
    // Boolean value to indicate if point is in or out the MAndelbrot Set
    bool isInM;
};
typedef struct MandelbrotSetPoint MandelbrotSetPoint;


#endif // BASICTYPES_H
