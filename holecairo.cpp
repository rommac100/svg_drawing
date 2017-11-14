#include "holecairo.h"

HoleCairo::HoleCairo()
{

}

HoleCairo::HoleCairo(double xArr, double yArr, double radius)
{
    this->radius = radius;
    this->xCord = xArr;
    this->yCord = yArr;
}

double HoleCairo::getRadius()
{
    return radius;
}

double HoleCairo::getX()
{
    return xCord;
}

double HoleCairo::getY()
{
    return yCord;
}
