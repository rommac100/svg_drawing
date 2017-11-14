#include "cairorect.h"
#include <qrect.h>
#include <QRectF>
#include "holecairo.h"

CairoRect::CairoRect()
{

}


CairoRect::CairoRect(double xTemp, double yTemp, double widthTemp, double heighTemp, Side tempSide)
{
    this->x = xTemp;
    this->y = yTemp;
    this->width = widthTemp;
    this->height = heighTemp;
    this->side = tempSide;
}

CairoRect::CairoRect(double xTemp, double yTemp, double widthTemp, double heighTemp)
{
    this->x = xTemp;
    this->y = yTemp;
    this->width = widthTemp;
    this->height = heighTemp;
}

void CairoRect::setHeight(double heightTemp)
{
    this->height = heightTemp;
}

CairoRect::Side CairoRect::getSide()
{
    this->side;
}

void CairoRect::setPos(double xTemp, double yTemp)
{
    this->x = xTemp;
    this->y = yTemp;
}

void CairoRect::setSize(double widthTemp, double heighTemp)
{
    this->width = widthTemp;
    this->height = heighTemp;
}

void CairoRect::setXPos(double xTemp)
{
    this->x = xTemp;
}

void CairoRect::setYPos(double yTemp)
{
  this->y = yTemp;
}

void CairoRect::setWidth(double widthTemp)
{
    this->width = widthTemp;
}

double CairoRect::getHeight()
{
    return this->height;
}

double CairoRect::getWidth()
{
    return this->width;
}

double CairoRect::getXPos()
{
    return this->x;
}

double CairoRect::getYPos()
{
    return this->y;
}

QRectF CairoRect::toQRect()
{
    return QRectF(QPointF(this->x,this->y),QSizeF(this->width,this->height));
}

void CairoRect::addHole(HoleCairo tempHole)
{
    listHole.push_back(tempHole);
}

HoleCairo CairoRect::getHole(int index)
{
    if (listHole.size > 0)
    {
        return listHole.at(index);
    }
    else {
        return NULL;
    }
}


