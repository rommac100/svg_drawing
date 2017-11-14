#ifndef CairoRect_H
#define CairoRect_H
#include <QRectF>
#include <vector>
#include "holecairo.h"


class CairoRect
{
public:
    enum Side{WH,LH,WL};

    CairoRect();
    CairoRect(double xTemp, double yTemp, double widthTemp, double heighTemp);
    CairoRect(double xTemp, double yTemp, double widthTemp, double heighTemp, Side tempSide);
    void setPos(double xTemp, double yTemp);
    void setWidth(double widthTemp);
    void setHeight(double heightTemp);
    void setSize(double widthTemp, double heighTemp);
    void setXPos(double xTemp);
    void setYPos(double yTemp);
    double getXPos();
    double getYPos();
    double getWidth();
    double getHeight();
    Side getSide();
    QRectF toQRect();
    void addHole(HoleCairo tempHole);
    HoleCairo getHole(int index);

    Side side;
private:
    double x;
    double y;
    double width;
    double height;
    std::vector <HoleCairo> listHole;
};

#endif // CairoRect_H
