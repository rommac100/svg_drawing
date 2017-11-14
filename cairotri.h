#ifndef CAIROTRI_H
#define CAIROTRI_H
#include "cairoline.h"

// the coord parameter is equal to the point between side1 and side 2
class CairoTri
{
public:
    CairoTri();
    CairoTri(double side1Len, double side2Len, double side3Len, double depthPar, double xPar, double yPar);

    CairoLine getSide1();
    CairoLine getSide2();
    CairoLine getSide3();

    double getA();
    double getB();
    double getC();
    double getDepth();

private:
    CairoLine side1Line;
    CairoLine side2Line;
    CairoLine side3Line;

    CairoLine calcSide1Line();
    CairoLine calcSide2Line();
    CairoLine calcSide3Line();

    double side1Len;
    double side2Len;
    double side3Len;

    double depth;
    double x;
    double y;

    double aAng;
    double bAng;
    double cAng;

};

#endif // CAIROTRI_H
