#include "cairoline.h"
#include <math.h>
#include <QPoint>


CairoLine::CairoLine()
{

}

double distancePoints(double x1, double x2, double y1, double y2)
{
    return sqrt(pow((x2-x1),2)+pow((y2-y1),2));
}

CairoLine::CairoLine(double x1, double y1, double x2, double y2)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;

    this->length = distancePoints(x1,x2,y1,y2);
}

double CairoLine::getLength()
{
    return this->length;
}

double CairoLine::getX1()
{
    return this->x1;
}

double CairoLine::getX2()
{
    return this->x2;
}

double CairoLine::getY1()
{
    return this->y1;
}

double CairoLine::getY2()
{
    return this->y2;
}

QPoint CairoLine::toQPoint1()
{
  return QPoint((int)(this->x1+.5),(int)(this->y1+.5));
}

QPoint CairoLine::toQPoint2()
{
    return QPoint((int)(this->x2+.5),(int)(this->y2+.5));
}
