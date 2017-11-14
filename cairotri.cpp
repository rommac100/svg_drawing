#include "cairotri.h"
#include "cairoline.h"
#include <math.h>

#define PI 3.14159265


//side1 = a, side2 = b, side3 = c
CairoTri::CairoTri()
{

}

CairoTri::CairoTri(double side1Len, double side2Len, double side3Len, double depthPar, double xPar, double yPar)
{
    this->x = xPar;
    this->y = yPar;

    this->side1Len = side1Len;
    std::cout<<side1Len<<std::endl;
    this->side2Len = side2Len;
    std::cout<<side2Len<<std::endl;
    this->side3Len = side3Len;
    std::cout<<side3Len<<std::endl;

    this->side1Line = calcSide1Line();
    this->side2Line = calcSide2Line();
    this->side3Line = calcSide3Line();


}

double CairoTri::getDepth()
{
    return depth;
}

CairoLine CairoTri::getSide1()
{
    return side1Line;
}

CairoLine CairoTri::getSide2()
{
   return side2Line;
}

CairoLine CairoTri::getSide3()
{
   return side3Line;
}

CairoLine CairoTri::calcSide1Line()
{
    double xTemp = this->x;
    double yTemp = this->y;
    return CairoLine(xTemp,yTemp,(xTemp+side1Len),yTemp);
}

CairoLine CairoTri::calcSide2Line()
{
    double xTemp = side1Line.getX2();
    double yTemp = side1Line.getY2();
    double xTemp2 = ((pow(side1Len,2)+pow(side2Len,2)-pow(side3Len,2))/(2*side1Len));
    double yTemp2 = (sqrt(pow(side2Len,2)-pow(xTemp2,2)));

    return CairoLine(xTemp,yTemp,xTemp2,yTemp2);
}

CairoLine CairoTri::calcSide3Line()
{
   double xTemp  = side2Line.getX2();
   double yTemp = side2Line.getY2();

   double xTemp2 = side1Line.getX1();
   double yTemp2 = side1Line.getY1();

   return CairoLine(xTemp,yTemp,xTemp2,yTemp2);
}


double CairoTri::getA()
{
    return this->aAng;
}

double CairoTri::getB()
{
    return this->bAng;
}

double CairoTri::getC()
{
    return this->cAng;
}
