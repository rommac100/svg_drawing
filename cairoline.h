#ifndef CAIROLINE_H
#define CAIROLINE_H
#include <iostream>
#include <vector>
#include "cairorect.h"
#include <QPoint>

class CairoLine
{
public:
    CairoLine();
    CairoLine(double x1, double y1, double x2, double y2);
    double getLength();
    double getX1();
    double getX2();
    double getY1();
    double getY2();
    //std::vector<CairoLine> fingerGenerator(CairoRect rectTemp, double length,double height,double width, double materialWidth);
    QPoint toQPoint1();
    QPoint toQPoint2();
private:
    double x1;
    double x2;
    double y1;
    double y2;
    double length;
};

#endif // CAIROLINE_H
