/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderarea.h"

#include <QPainter>
#include <QPaintEngine>
#include <QSvgGenerator>
#include <math.h>
#include <iostream>
#include <QRectF>
#include <iomanip>
#include <sstream>
#include <string>
#include <cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <QTemporaryDir>
#include <QSvgRenderer>
#include "qcairopaintdevice.h"
#include <QPaintDevice>
#include "cairorect.h"
#include "cairoline.h"
#include <stdio.h>
#include "cairotri.h"
#include <QPointF>
#include "holecairo.h"

//wrong nomenclature but should be fine. Pixels here means dots because of cairo
const double pixelInch = 72;
const double defaultThickness = .125;
const int arrayOddNums[13] = {3,5,7,9,11,13,15,17,19,21,23,25,27};

const int widthExportSize = 24;
const int heightExportSize = 18;

int calcHighestOdd(double lengthArr, double fingWidthArr)
{
    int i = 0;
    double largeFinFactor = 1;

    while (largeFinFactor > 0 && fingWidthArr != NULL)
    {
        largeFinFactor = (lengthArr-(arrayOddNums[i]*fingWidthArr)-(4*fingWidthArr)/(4*fingWidthArr));
        if (largeFinFactor < 0)
        {

            largeFinFactor = (lengthArr-(arrayOddNums[i]*fingWidthArr)-(4*fingWidthArr)/(4*fingWidthArr));
            break;
        }
        else{
        i++;
        }
    }
    return arrayOddNums[(i-2)];
}

double calcLargeFingWidth(double lengthArr, double fingWidthArr, double numberOfFingers)
{
    double largeFinFactor = 0;

    double largeFinFactor1 = (lengthArr-(numberOfFingers*fingWidthArr)-(4*fingWidthArr));
    largeFinFactor = largeFinFactor1/(fingWidthArr*4);
    return largeFinFactor;
}

int hexR(std::string hex)
{
   std::string temp = hex.substr(1,4);
   std::stringstream hexConvert;
   hexConvert << std::hex <<temp;
   int tempR;
   hexConvert >> tempR;
    return tempR;
}

int hexG(std::string hex)
{
   std::string temp = hex.substr(3,6);
   std::stringstream hexConvert;
   hexConvert << std::hex <<temp;
   int tempG;
   hexConvert >> tempG;
   return tempG;
}

int hexB(std::string hex)
{
   std::string temp = hex.substr(5,8);
   std::stringstream hexConvert;
   hexConvert << std::hex <<temp;
   int tempB;
   hexConvert >> tempB;
   return tempB;
}

int hexA(std::string hex)
{
   std::string temp = hex.substr(7,10);
   std::stringstream hexConvert;
   hexConvert << std::hex <<temp;
   int tempA;
   hexConvert >> tempA;
   return tempA;

}
RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    shape = Rect;
    antialiased = false;
    transformed = false;
    unitsMain = Inches;
    pen.setColor(QColor(255, 0, 0, 255));
    pen.setWidth(1);
    windowHeight = 0;
    windowLength = 0;
    fingWidth = 0;
    materialWidth = defaultThickness*pixelInch;
    depth = 0;
    sideIndex = 0;
    scalingFactor = pixelInch;


    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void RenderArea::setSide(int index)
{
    if (index <= 6)
    {
        sideIndex = index;
    }
    update();
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderArea::sizeHint() const
{
    return QSize(600, 300);
}

void RenderArea::setMatThick(double thick)
{
    this->materialWidth = thick;
}

void RenderArea::setShape(Shape shape)
{
    this->shape = shape;
    update();
}

void RenderArea::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void RenderArea::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void RenderArea::fingerSlotChecked(bool tempCheck)
{
    this->fingerSlotBool = tempCheck;
    update();
}

void RenderArea::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void RenderArea::setTransformed(bool transformed)
{
    this->transformed = transformed;
    update();
}

void RenderArea::setUnits(Units units)
{
    this->unitsMain = units;

    if (unitsMain == Units::Inches)
    {
        scalingFactor = 2.54*pixelInch;
    }
    else
    {
        scalingFactor = pixelInch;
    }
    update();
}

void RenderArea::setPenColour(QString colour)
{
   pen.setColor(QColor(colour));
   update();
}

void RenderArea::setWidth(long double Width)
{
    if (unitsMain != Inches)
    {
        this->width2 = Width*scalingFactor;
    }
    else
    {
    this->width2 = Width*scalingFactor;
    }
    update();
}

void RenderArea::setHeight(long double Height)
{
    if (unitsMain != Inches)
    {
        this->height2= Height*scalingFactor;
    }
    else
    {
    this->height2 = Height*scalingFactor;
    }
    update();
}

void RenderArea::setLength(long double Length)
{
    if (unitsMain != Inches)
    {
        this->length= Length*scalingFactor;
    }
    else
    {
    this->length = Length*scalingFactor;
    }
    update();
}

void RenderArea::setFingWidth(double fing_slot_width)
{
    this->fingWidth = fing_slot_width*pixelInch;
    update();
}

void RenderArea::setDepth(double depthArr)
{
    this->depth = depthArr;
}

void RenderArea::addHole(double x, double y, double radius, int sideIndex)
{
    return new HoleCairo(x,y,radius);
}

void RenderArea::paintCairo(std::string path)
{
    double windowWidth = widthExportSize*pixelInch;
    double windowHeight = heightExportSize*pixelInch;

    std::string tempDir = path;
    Cairo::RefPtr<Cairo::SvgSurface> surface = Cairo::SvgSurface::create(tempDir, windowWidth, windowHeight);

    Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);

    cr->save();

    if (shape == Rect)
    {
        CairoRect rect1(100,100, length, width2, CairoRect::WL);
        CairoRect rect5(rect1.getXPos()+materialWidth, rect1.getYPos()+rect1.getHeight()+10, length, width2, CairoRect::WL);
        CairoRect rect2(rect5.getXPos()+rect5.getWidth()+10+materialWidth, rect5.getYPos(), length, height2, CairoRect::LH);
        CairoRect rect6(rect2.getXPos()+materialWidth, rect2.getYPos()+rect2.getHeight()+50, length, height2, CairoRect::LH);
        CairoRect rect3(rect6.getXPos()+rect6.getWidth()+50+materialWidth, rect6.getYPos(), height2, width2, CairoRect::WH);
        CairoRect rect4(rect3.getXPos()+materialWidth, rect3.getYPos()+rect3.getHeight()+10, height2, width2, CairoRect::WH);

        this->rectsForRect = {rect1,rect2,rect3,rect4,rect5,rect6};


    if (fingWidth !=0 && fingWidth != NULL)
    {

    for (CairoRect rect : rectsForRect)
    {

    std::vector<CairoLine> fingerSlots = fingerGenerator(rect);

    for (CairoLine temp : fingerSlots)
    {
        double tempX1 = temp.getX1();
        double tempX2 = temp.getX2();
        double tempY1 = temp.getY1();
        double tempY2 = temp.getY2();

        std::cout<<"Y1 Value";
        std::cout<<tempY1<<std::endl;
        cr->set_source_rgb(255,0,0);
        cr->move_to(tempX1,tempY1);
        cr->line_to(tempX2,tempY2);
        cr->stroke();
    }
    }
    }
}
    else if (shape == TriPrism)
    {

    }
    cr->restore();
    cr->show_page();
}

std::vector<CairoLine> RenderArea::fingerGenerator(CairoRect rectTemp)
{
    std::vector<CairoLine> tempVect;

    if (rectTemp.side == CairoRect::WL)
    {
        double currentX = rectTemp.getXPos();
        double currentY = rectTemp.getYPos();
       for (int i = 0; i < 4; i++)
       {
           //going by side 0 = top, 1 = right, 2 = bottom, 3 = left
           if (i == 0)
           {
               int numberSmTabs = calcHighestOdd((rectTemp.getWidth())/pixelInch,this->fingWidth/pixelInch);
               double largeTabScale = calcLargeFingWidth((rectTemp.getWidth())/pixelInch,this->fingWidth/pixelInch, numberSmTabs);
               double tabWidthLarge = (largeTabScale+1)*fingWidth;

               //determine whether the vert line is down or up
               bool down = true;
               bool downSide = false; //second osscilation
               unsigned int linesofLength = numberSmTabs*2+7;
               unsigned int lastLarge = linesofLength-1;
               unsigned int secondlastLarge = linesofLength-2;
               for (unsigned int j = 1; j <= linesofLength; j++)
               {
                   if (j%2 == 0)
                   {
                       if (down == true)
                       {
                        CairoLine temp(currentX,currentY,currentX,currentY+materialWidth);
                        currentX = temp.getX2();
                        currentY = temp.getY2();
                        if (j == 2)
                        {
                        std::printf("currentY");
                        std::cout<<currentY<<std::endl;
                        }
                        tempVect.push_back(temp);
                        down = false;
                       }
                       else if (down==false);
                       {
                           CairoLine temp(currentX,currentY,currentX,currentY-materialWidth);
                           currentX = temp.getX2();
                           currentY = temp.getY2();
                           tempVect.push_back(temp);
                           down = true;
                       }
                   }
                   //weird bug here the third or will not ru but it runs if turned into else statement
                   //nor will the second else if runs
                   //also a variably defined index will not used but the integer equiv will?
                   else{
                         if (j == 1 || j ==0)
                         {
                             CairoLine temp(currentX,currentY,currentX+tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == 3)
                         {
                             std::cout<<"currentY j=3: ";
                             std::cout<<currentY<<std::endl;
                             CairoLine temp(currentX,currentY+materialWidth,currentX+tabWidthLarge,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY2()-materialWidth;
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == secondlastLarge)
                         {
                             CairoLine temp(currentX,currentY+materialWidth,currentX+tabWidthLarge,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY2()-materialWidth;
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                             std::cout<<"secondLastValue";
                             std::cout<<secondlastLarge;
                         }
                         else if (j == linesofLength)
                         {
                             CairoLine temp(currentX,currentY,currentX+tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else
                         {
                             if (downSide)
                             {
                             CairoLine temp(currentX,currentY+materialWidth,currentX+fingWidth,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY1()-materialWidth;
                             std::printf("currentY horz: ");
                             std::cout<<currentY<<std::endl;
                             tempVect.push_back(temp);
                             downSide = false;
                             }
                             else if(!downSide)
                             {
                                 CairoLine temp(currentX,currentY,currentX+fingWidth,currentY);
                                 currentX = temp.getX2();
                                 currentY = temp.getY2();
                                 std::printf("currentY horz: ");
                                 std::cout<<currentY<<std::endl;
                                 tempVect.push_back(temp);
                                 downSide = true;
                             }
                         }
                         }
           }
           }


           else if(i ==1)
           {
               int numberSmTabs = calcHighestOdd((rectTemp.getHeight())/pixelInch,this->fingWidth/pixelInch);
               double largeTabScale = calcLargeFingWidth((rectTemp.getHeight())/pixelInch,this->fingWidth/pixelInch, numberSmTabs);
               double tabWidthLarge = (largeTabScale+1)*fingWidth;

               //determine whether the vert line is down or up
               bool down = true;
               bool downSide = true; //second osscilation
               unsigned int linesofLength = numberSmTabs*2+7;
               unsigned int lastLarge = linesofLength-1;
               unsigned int secondlastLarge = linesofLength-2;
               for (unsigned int j = 1; j <= linesofLength; j++)
               {
                   if (j%2 == 0)
                   {
                       if (down == false)
                       {
                        CairoLine temp(currentX,currentY,currentX+materialWidth,currentY);
                        currentX = temp.getX2();
                        currentY = temp.getY2();
                        if (j == 2)
                        {
                        std::printf("currentY");
                        std::cout<<currentY<<std::endl;
                        }
                        tempVect.push_back(temp);
                        down = true;
                       }
                       else if (down==true);
                       {
                           CairoLine temp(currentX,currentY,currentX-materialWidth,currentY);
                           currentX = temp.getX2();
                           currentY = temp.getY2();
                           tempVect.push_back(temp);
                           down = false;
                       }
                   }
                   //weird bug here the third or will not ru but it runs if turned into else statement
                   //nor will the second else if runs
                   //also a variably defined index will not used but the integer equiv will?
                   else{
                         if (j == 1 || j ==0)
                         {
                             CairoLine temp(currentX,currentY,currentX,currentY+tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == 3)
                         {
                             std::cout<<"currentY j=3: ";
                             std::cout<<currentY<<std::endl;
                             CairoLine temp(currentX,currentY,currentX,currentY+tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == secondlastLarge)
                         {
                             CairoLine temp(currentX,currentY,currentX,currentY+tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                             std::cout<<"secondLastValue";
                             std::cout<<secondlastLarge;
                         }
                         else if (j == linesofLength)
                         {
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY+tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else
                         {
                             if (downSide)
                             {
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY+fingWidth);
                             currentX = temp.getX2()-materialWidth;
                             currentY = temp.getY2();
                             std::printf("currentY horz: ");
                             std::cout<<currentY<<std::endl;
                             tempVect.push_back(temp);
                             downSide = false;
                             }
                             else if(!downSide)
                             {
                                 CairoLine temp(currentX,currentY,currentX,currentY+fingWidth);
                                 currentX = temp.getX2();
                                 currentY = temp.getY2();
                                 std::printf("currentY horz: ");
                                 std::cout<<currentY<<std::endl;
                                 tempVect.push_back(temp);
                                 downSide = true;
                             }
                         }
                         }
           }
           }
           else if (i==2)
           {

               int numberSmTabs = calcHighestOdd((rectTemp.getWidth())/pixelInch,this->fingWidth/pixelInch);
               double largeTabScale = calcLargeFingWidth((rectTemp.getWidth())/pixelInch,this->fingWidth/pixelInch, numberSmTabs);
               double tabWidthLarge = (largeTabScale+1)*fingWidth;

               //determine whether the vert line is down or up
               bool down = false;
               bool downSide = true; //second osscilation
               unsigned int linesofLength = numberSmTabs*2+7;
               unsigned int lastLarge = linesofLength-1;
               unsigned int secondlastLarge = linesofLength-2;
               for (unsigned int j = 1; j <= linesofLength; j++)
               {
                   if (j%2 == 0)
                   {
                       if (down == true)
                       {
                        CairoLine temp(currentX,currentY,currentX,currentY+materialWidth);
                        currentX = temp.getX2();
                        currentY = temp.getY2();
                        if (j == 2)
                        {
                        std::printf("currentY");
                        std::cout<<currentY<<std::endl;
                        }
                        tempVect.push_back(temp);
                        down = false;
                       }
                       else if (down==false);
                       {
                           CairoLine temp(currentX,currentY,currentX,currentY-materialWidth);
                           currentX = temp.getX2();
                           currentY = temp.getY2();
                           tempVect.push_back(temp);
                           down = true;
                       }
                   }
                   //weird bug here the third or will not ru but it runs if turned into else statement
                   //nor will the second else if runs
                   //also a variably defined index will not used but the integer equiv will?
                   else{
                         if (j == 1 || j ==0)
                         {
                             CairoLine temp(currentX,currentY,currentX-tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == 3)
                         {
                             std::cout<<"currentY j=3: ";
                             std::cout<<currentY<<std::endl;
                             CairoLine temp(currentX,currentY,currentX-tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == secondlastLarge)
                         {
                             CairoLine temp(currentX,currentY,currentX-tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                             std::cout<<"secondLastValue";
                             std::cout<<secondlastLarge;
                         }
                         else if (j == linesofLength)
                         {
                             CairoLine temp(currentX,currentY+materialWidth,currentX-tabWidthLarge,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else
                         {
                             if (downSide)
                             {
                             CairoLine temp(currentX,currentY+materialWidth,currentX-fingWidth,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY1()-materialWidth;
                             std::printf("currentY horz: ");
                             std::cout<<currentY<<std::endl;
                             tempVect.push_back(temp);
                             downSide = false;
                             }
                             else if(!downSide)
                             {
                                 CairoLine temp(currentX,currentY,currentX-fingWidth,currentY);
                                 currentX = temp.getX2();
                                 currentY = temp.getY2();
                                 std::printf("currentY horz: ");
                                 std::cout<<currentY<<std::endl;
                                 tempVect.push_back(temp);
                                 downSide = true;
                             }
                         }
                         }
           }
           }
           else if (i==3)
           {
               int numberSmTabs = calcHighestOdd((rectTemp.getHeight())/pixelInch,this->fingWidth/pixelInch);
               double largeTabScale = calcLargeFingWidth((rectTemp.getHeight())/pixelInch,this->fingWidth/pixelInch, numberSmTabs);
               double tabWidthLarge = (largeTabScale+1)*fingWidth;

               //determine whether the vert line is down or up
               bool down = false;
               bool downSide = false; //second osscilation
               unsigned int linesofLength = numberSmTabs*2+7;
               unsigned int lastLarge = linesofLength-1;
               unsigned int secondlastLarge = linesofLength-2;
               for (unsigned int j = 1; j <= linesofLength; j++)
               {
                   if (j%2 == 0)
                   {
                       if (down == false)
                       {
                        CairoLine temp(currentX,currentY,currentX+materialWidth,currentY);
                        currentX = temp.getX2();
                        currentY = temp.getY2();
                        if (j == 2)
                        {
                        std::printf("currentY");
                        std::cout<<currentY<<std::endl;
                        }
                        tempVect.push_back(temp);
                        down = true;
                       }
                       else if (down==true);
                       {
                           CairoLine temp(currentX,currentY,currentX-materialWidth,currentY);
                           currentX = temp.getX2();
                           currentY = temp.getY2();
                           tempVect.push_back(temp);
                           down = false;
                       }
                   }
                   //weird bug here the third or will not ru but it runs if turned into else statement
                   //nor will the second else if runs
                   //also a variably defined index will not used but the integer equiv will?
                   else{
                         if (j == 1 || j ==0)
                         {
                             CairoLine temp(currentX,currentY,currentX,currentY-tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == 3)
                         {
                             std::cout<<"currentY j=3: ";
                             std::cout<<currentY<<std::endl;
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY-tabWidthLarge);
                             currentX = temp.getX2()-materialWidth;
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == secondlastLarge)
                         {
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY-tabWidthLarge);
                             currentX = temp.getX2()-materialWidth;
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                             std::cout<<"secondLastValue";
                             std::cout<<secondlastLarge;
                         }
                         else if (j == linesofLength)
                         {
                             CairoLine temp(currentX,currentY,currentX,currentY-tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else
                         {
                             if (downSide)
                             {
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY-fingWidth);
                             currentX = temp.getX2()-materialWidth;
                             currentY = temp.getY2();
                             std::printf("currentY horz: ");
                             std::cout<<currentY<<std::endl;
                             tempVect.push_back(temp);
                             downSide = false;
                             }
                             else if(!downSide)
                             {
                                 CairoLine temp(currentX,currentY,currentX,currentY-fingWidth);
                                 currentX = temp.getX2();
                                 currentY = temp.getY2();
                                 std::printf("currentY horz: ");
                                 std::cout<<currentY<<std::endl;
                                 tempVect.push_back(temp);
                                 downSide = true;
                             }
                         }
                         }
           }
           }
       }
    
    


}
    else if (rectTemp.side == CairoRect::LH)
    {
        double currentX = rectTemp.getXPos();
        double currentY = rectTemp.getYPos();
       for (int i = 0; i < 4; i++)
       {
           //going by side 0 = top, 1 = right, 2 = bottom, 3 = left
           if (i == 0)
           {
               int numberSmTabs = calcHighestOdd((rectTemp.getWidth())/pixelInch,this->fingWidth/pixelInch);
               double largeTabScale = calcLargeFingWidth((rectTemp.getWidth())/pixelInch,this->fingWidth/pixelInch, numberSmTabs);
               double tabWidthLarge = (largeTabScale+1)*fingWidth;

               //determine whether the vert line is down or up
               bool down = false;
               bool downSide = true; //second osscilation
               unsigned int linesofLength = numberSmTabs*2+7;
               unsigned int lastLarge = linesofLength-1;
               unsigned int secondlastLarge = linesofLength-2;
               for (unsigned int j = 1; j <= linesofLength; j++)
               {
                   if (j%2 == 0)
                   {
                       if (down == true)
                       {
                        CairoLine temp(currentX,currentY,currentX,currentY+materialWidth);
                        currentX = temp.getX2();
                        currentY = temp.getY2();
                        if (j == 2)
                        {
                        std::printf("currentY");
                        std::cout<<currentY<<std::endl;
                        }
                        tempVect.push_back(temp);
                        down = false;
                       }
                       else if (down==false);
                       {
                           CairoLine temp(currentX,currentY,currentX,currentY-materialWidth);
                           currentX = temp.getX2();
                           currentY = temp.getY2();
                           tempVect.push_back(temp);
                           down = true;
                       }
                   }
                   //weird bug here the third or will not ru but it runs if turned into else statement
                   //nor will the second else if runs
                   //also a variably defined index will not used but the integer equiv will?
                   else{
                         if (j == 3)
                         {
                             CairoLine temp(currentX,currentY,currentX+tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j== 1)
                         {
                             CairoLine temp(currentX,currentY,currentX+tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == secondlastLarge)
                         {
                             CairoLine temp(currentX,currentY,currentX+tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                             std::cout<<"secondLastValue";
                             std::cout<<secondlastLarge;
                         }
                         else if (j == linesofLength)
                         {
                             CairoLine temp(currentX,currentY+materialWidth,currentX+tabWidthLarge,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else
                         {
                             if (downSide)
                             {
                             CairoLine temp(currentX,currentY+materialWidth,currentX+fingWidth,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY1()-materialWidth;
                             std::printf("currentY horz: ");
                             std::cout<<currentY<<std::endl;
                             tempVect.push_back(temp);
                             downSide = false;
                             }
                             else if(!downSide)
                             {
                                 CairoLine temp(currentX,currentY,currentX+fingWidth,currentY);
                                 currentX = temp.getX2();
                                 currentY = temp.getY2();
                                 std::printf("currentY horz: ");
                                 std::cout<<currentY<<std::endl;
                                 tempVect.push_back(temp);
                                 downSide = true;
                             }
                         }
                         }
           }
           }
           else if(i ==1)
           {
               int numberSmTabs = calcHighestOdd((rectTemp.getHeight())/pixelInch,this->fingWidth/pixelInch);
               double largeTabScale = calcLargeFingWidth((rectTemp.getHeight())/pixelInch,this->fingWidth/pixelInch, numberSmTabs);
               double tabWidthLarge = (largeTabScale+1)*fingWidth;

               //determine whether the vert line is down or up
               bool down = false;
               bool downSide = false; //second osscilation
               unsigned int linesofLength = numberSmTabs*2+7;
               unsigned int lastLarge = linesofLength-1;
               unsigned int secondlastLarge = linesofLength-2;
               for (unsigned int j = 1; j <= linesofLength; j++)
               {
                   if (j%2 == 0)
                   {
                       if (down == false)
                       {
                        CairoLine temp(currentX,currentY,currentX+materialWidth,currentY);
                        currentX = temp.getX2();
                        currentY = temp.getY2();
                        if (j == 2)
                        {
                        std::printf("currentY");
                        std::cout<<currentY<<std::endl;
                        }
                        tempVect.push_back(temp);
                        down = true;
                       }
                       else if (down==true);
                       {
                           CairoLine temp(currentX,currentY,currentX-materialWidth,currentY);
                           currentX = temp.getX2();
                           currentY = temp.getY2();
                           tempVect.push_back(temp);
                           down = false;
                       }
                   }
                   //weird bug here the third or will not ru but it runs if turned into else statement
                   //nor will the second else if runs
                   //also a variably defined index will not used but the integer equiv will?
                   else{
                         if (j == 1 || j ==0)
                         {
                             CairoLine temp(currentX,currentY,currentX,currentY+tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == 3)
                         {
                             std::cout<<"currentY j=3: ";
                             std::cout<<currentY<<std::endl;
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY+tabWidthLarge);
                             currentX = temp.getX2()-materialWidth;
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == secondlastLarge)
                         {
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY+tabWidthLarge);
                             currentX = temp.getX2()-materialWidth;
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                             std::cout<<"secondLastValue";
                             std::cout<<secondlastLarge;
                         }
                         else if (j == linesofLength)
                         {
                             CairoLine temp(currentX,currentY,currentX,currentY+tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else
                         {
                             if (downSide)
                             {
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY+fingWidth);
                             currentX = temp.getX2()-materialWidth;
                             currentY = temp.getY2();
                             std::printf("currentY horz: ");
                             std::cout<<currentY<<std::endl;
                             tempVect.push_back(temp);
                             downSide = false;
                             }
                             else if(!downSide)
                             {
                                 CairoLine temp(currentX,currentY,currentX,currentY+fingWidth);
                                 currentX = temp.getX2();
                                 currentY = temp.getY2();
                                 std::printf("currentY horz: ");
                                 std::cout<<currentY<<std::endl;
                                 tempVect.push_back(temp);
                                 downSide = true;
                             }
                         }
                         }
           }
           }
           else if (i==2)
           {
               int numberSmTabs = calcHighestOdd((rectTemp.getWidth())/scalingFactor,this->fingWidth/pixelInch);
               double largeTabScale = calcLargeFingWidth((rectTemp.getWidth())/pixelInch,this->fingWidth/pixelInch, numberSmTabs);
               double tabWidthLarge = (largeTabScale+1)*fingWidth;

               //determine whether the vert line is down or up
               bool down = true;
               bool downSide = false; //second osscilation
               unsigned int linesofLength = numberSmTabs*2+7;
               unsigned int lastLarge = linesofLength-1;
               unsigned int secondlastLarge = linesofLength-2;
               for (unsigned int j = 1; j <= linesofLength; j++)
               {
                   if (j%2 == 0)
                   {
                       if (down == true)
                       {
                        CairoLine temp(currentX,currentY,currentX,currentY+materialWidth);
                        currentX = temp.getX2();
                        currentY = temp.getY2();
                        if (j == 2)
                        {
                        std::printf("currentY");
                        std::cout<<currentY<<std::endl;
                        }
                        tempVect.push_back(temp);
                        down = false;
                       }
                       else if (down==false);
                       {
                           CairoLine temp(currentX,currentY,currentX,currentY-materialWidth);
                           currentX = temp.getX2();
                           currentY = temp.getY2();
                           tempVect.push_back(temp);
                           down = true;
                       }
                   }
                   //weird bug here the third or will not run but it runs if turned into else statement
                   //nor will the second else if runs
                   //also a variably defined index will not used but the integer equiv will?
                   else{
                         if (j == 1 || j ==0)
                         {
                             CairoLine temp(currentX,currentY,currentX-tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == 3)
                         {
                             std::cout<<"currentY j=3: ";
                             std::cout<<currentY<<std::endl;
                             CairoLine temp(currentX,currentY+materialWidth,currentX-tabWidthLarge,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY2()-materialWidth;
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == secondlastLarge)
                         {
                             CairoLine temp(currentX,currentY+materialWidth,currentX-tabWidthLarge,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY2()-materialWidth;
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                             std::cout<<"secondLastValue";
                             std::cout<<secondlastLarge;
                         }
                         else if (j == linesofLength)
                         {
                             CairoLine temp(currentX,currentY,currentX-tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else
                         {
                             if (downSide)
                             {
                             CairoLine temp(currentX,currentY+materialWidth,currentX-fingWidth,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY1()-materialWidth;
                             std::printf("currentY horz: ");
                             std::cout<<currentY<<std::endl;
                             tempVect.push_back(temp);
                             downSide = false;
                             }
                             else if(!downSide)
                             {
                                 CairoLine temp(currentX,currentY,currentX-fingWidth,currentY);
                                 currentX = temp.getX2();
                                 currentY = temp.getY2();
                                 std::printf("currentY horz: ");
                                 std::cout<<currentY<<std::endl;
                                 tempVect.push_back(temp);
                                 downSide = true;
                             }
                         }
                         }
           }
           }
           else if (i==3)
           {
               int numberSmTabs = calcHighestOdd((rectTemp.getHeight())/scalingFactor,this->fingWidth/pixelInch);
               double largeTabScale = calcLargeFingWidth((rectTemp.getHeight())/scalingFactor,this->fingWidth/pixelInch, numberSmTabs);
               double tabWidthLarge = (largeTabScale+1)*fingWidth;

               //determine whether the vert line is down or up
               bool down = true;
               bool downSide = true; //second osscilation
               unsigned int linesofLength = numberSmTabs*2+7;
               unsigned int lastLarge = linesofLength-1;
               unsigned int secondlastLarge = linesofLength-2;
               for (unsigned int j = 1; j <= linesofLength; j++)
               {
                   if (j%2 == 0)
                   {
                       if (down == false)
                       {
                        CairoLine temp(currentX,currentY,currentX+materialWidth,currentY);
                        currentX = temp.getX2();
                        currentY = temp.getY2();
                        if (j == 2)
                        {
                        std::printf("currentY");
                        std::cout<<currentY<<std::endl;
                        }
                        tempVect.push_back(temp);
                        down = true;
                       }
                       else if (down==true);
                       {
                           CairoLine temp(currentX,currentY,currentX-materialWidth,currentY);
                           currentX = temp.getX2();
                           currentY = temp.getY2();
                           tempVect.push_back(temp);
                           down = false;
                       }
                   }
                   //weird bug here the third or will not ru but it runs if turned into else statement
                   //nor will the second else if runs
                   //also a variably defined index will not used but the integer equiv will?
                   else{
                         if (j ==  1 || j ==0 || j == 3)
                         {
                             CairoLine temp(currentX,currentY,currentX,currentY-tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == secondlastLarge)
                         {
                             CairoLine temp(currentX,currentY,currentX,currentY-tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                             std::cout<<"secondLastValue";
                             std::cout<<secondlastLarge;
                         }
                         else if (j == linesofLength)
                         {
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY-tabWidthLarge);
                             currentX = temp.getX2()-materialWidth;
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else
                         {
                             if (downSide)
                             {
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY-fingWidth);
                             currentX = temp.getX2()-materialWidth;
                             currentY = temp.getY2();
                             std::printf("currentY horz: ");
                             std::cout<<currentY<<std::endl;
                             tempVect.push_back(temp);
                             downSide = false;
                             }
                             else if(!downSide)
                             {
                                 CairoLine temp(currentX,currentY,currentX,currentY-fingWidth);
                                 currentX = temp.getX2();
                                 currentY = temp.getY2();
                                 std::printf("currentY horz: ");
                                 std::cout<<currentY<<std::endl;
                                 tempVect.push_back(temp);
                                 downSide = true;
                             }
                         }
                         }
           }
           }
       }
    }
    else if (rectTemp.side == CairoRect::WH)
    {
        double currentX = rectTemp.getXPos();
        double currentY = rectTemp.getYPos();
       for (int i = 0; i < 4; i++)
       {
           //going by side 0 = top, 1 = right, 2 = bottom, 3 = left
           if (i == 0)
           {
               int numberSmTabs = calcHighestOdd((rectTemp.getWidth())/pixelInch,this->fingWidth/pixelInch);
               double largeTabScale = calcLargeFingWidth((rectTemp.getWidth())/pixelInch,this->fingWidth/pixelInch, numberSmTabs);
               double tabWidthLarge = (largeTabScale+1)*fingWidth;

               //determine whether the vert line is down or up
               bool down = true;
               bool downSide = false; //second osscilation
               unsigned int linesofLength = numberSmTabs*2+7;
               unsigned int lastLarge = linesofLength-1;
               unsigned int secondlastLarge = linesofLength-2;
               for (unsigned int j = 1; j <= linesofLength; j++)
               {
                   if (j%2 == 0)
                   {
                       if (down == true)
                       {
                        CairoLine temp(currentX,currentY,currentX,currentY+materialWidth);
                        currentX = temp.getX2();
                        currentY = temp.getY2();
                        if (j == 2)
                        {
                        std::printf("currentY");
                        std::cout<<currentY<<std::endl;
                        }
                        tempVect.push_back(temp);
                        down = false;
                       }
                       else if (down==false);
                       {
                           CairoLine temp(currentX,currentY,currentX,currentY-materialWidth);
                           currentX = temp.getX2();
                           currentY = temp.getY2();
                           tempVect.push_back(temp);
                           down = true;
                       }
                   }
                   //weird bug here the third or will not ru but it runs if turned into else statement
                   //nor will the second else if runs
                   //also a variably defined index will not used but the integer equiv will?
                   else{
                         if (j ==0 || j==1)
                         {
                             CairoLine temp(currentX,currentY,currentX+tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j== 3)
                         {
                             CairoLine temp(currentX,currentY+materialWidth,currentX+tabWidthLarge,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY2()-materialWidth;
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == secondlastLarge)
                         {
                             CairoLine temp(currentX,currentY+materialWidth,currentX+tabWidthLarge,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY2()-materialWidth;
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                             std::cout<<"secondLastValue";
                             std::cout<<secondlastLarge;
                         }
                         else if (j == linesofLength)
                         {
                             CairoLine temp(currentX,currentY,currentX+tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else
                         {
                             if (downSide)
                             {
                             CairoLine temp(currentX,currentY+materialWidth,currentX+fingWidth,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY1()-materialWidth;
                             std::printf("currentY horz: ");
                             std::cout<<currentY<<std::endl;
                             tempVect.push_back(temp);
                             downSide = false;
                             }
                             else if(!downSide)
                             {
                                 CairoLine temp(currentX,currentY,currentX+fingWidth,currentY);
                                 currentX = temp.getX2();
                                 currentY = temp.getY2();
                                 std::printf("currentY horz: ");
                                 std::cout<<currentY<<std::endl;
                                 tempVect.push_back(temp);
                                 downSide = true;
                             }
                         }
                         }
           }
           }


           else if(i ==1)
           {
               int numberSmTabs = calcHighestOdd((rectTemp.getHeight())/pixelInch,this->fingWidth/pixelInch);
               double largeTabScale = calcLargeFingWidth((rectTemp.getHeight())/pixelInch,this->fingWidth/pixelInch, numberSmTabs);
               double tabWidthLarge = (largeTabScale+1)*fingWidth;

               //determine whether the vert line is down or up
               bool down = false;
               bool downSide = false; //second osscilation
               unsigned int linesofLength = numberSmTabs*2+7;
               unsigned int lastLarge = linesofLength-1;
               unsigned int secondlastLarge = linesofLength-2;
               for (unsigned int j = 1; j <= linesofLength; j++)
               {
                   if (j%2 == 0)
                   {
                       if (down == false)
                       {
                        CairoLine temp(currentX,currentY,currentX+materialWidth,currentY);
                        currentX = temp.getX2();
                        currentY = temp.getY2();
                        if (j == 2)
                        {
                        std::printf("currentY");
                        std::cout<<currentY<<std::endl;
                        }
                        tempVect.push_back(temp);
                        down = true;
                       }
                       else if (down==true);
                       {
                           CairoLine temp(currentX,currentY,currentX-materialWidth,currentY);
                           currentX = temp.getX2();
                           currentY = temp.getY2();
                           tempVect.push_back(temp);
                           down = false;
                       }
                   }
                   //weird bug here the third or will not ru but it runs if turned into else statement
                   //nor will the second else if runs
                   //also a variably defined index will not used but the integer equiv will?
                   else{
                         if (j == 1 || j ==0)
                         {
                             CairoLine temp(currentX,currentY,currentX,currentY+tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j== 3)
                         {
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY+tabWidthLarge);
                             currentX = temp.getX2()-materialWidth;
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == secondlastLarge)
                         {
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY+tabWidthLarge);
                             currentX = temp.getX2()-materialWidth;
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                             std::cout<<"secondLastValue";
                             std::cout<<secondlastLarge;
                         }
                         else if (j == linesofLength)
                         {
                             CairoLine temp(currentX,currentY,currentX,currentY+tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else
                         {
                             if (downSide)
                             {
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY+fingWidth);
                             currentX = temp.getX2()-materialWidth;
                             currentY = temp.getY2();
                             std::printf("currentY horz: ");
                             std::cout<<currentY<<std::endl;
                             tempVect.push_back(temp);
                             downSide = false;
                             }
                             else if(!downSide)
                             {
                                 CairoLine temp(currentX,currentY,currentX,currentY+fingWidth);
                                 currentX = temp.getX2();
                                 currentY = temp.getY2();
                                 std::printf("currentY horz: ");
                                 std::cout<<currentY<<std::endl;
                                 tempVect.push_back(temp);
                                 downSide = true;
                             }
                         }
                         }
           }
           }
           else if (i==2)
           {
               int numberSmTabs = calcHighestOdd((rectTemp.getWidth())/pixelInch,this->fingWidth/pixelInch);
               double largeTabScale = calcLargeFingWidth((rectTemp.getWidth())/pixelInch,this->fingWidth/pixelInch, numberSmTabs);
               double tabWidthLarge = (largeTabScale+1)*fingWidth;

               //determine whether the vert line is down or up
               bool down = false;
               bool downSide = true; //second osscilation
               unsigned int linesofLength = numberSmTabs*2+7;
               unsigned int lastLarge = linesofLength-1;
               unsigned int secondlastLarge = linesofLength-2;
               for (unsigned int j = 1; j <= linesofLength; j++)
               {
                   if (j%2 == 0)
                   {
                       if (down == true)
                       {
                        CairoLine temp(currentX,currentY,currentX,currentY+materialWidth);
                        currentX = temp.getX2();
                        currentY = temp.getY2();
                        if (j == 2)
                        {
                        std::printf("currentY");
                        std::cout<<currentY<<std::endl;
                        }
                        tempVect.push_back(temp);
                        down = false;
                       }
                       else if (down==false);
                       {
                           CairoLine temp(currentX,currentY,currentX,currentY-materialWidth);
                           currentX = temp.getX2();
                           currentY = temp.getY2();
                           tempVect.push_back(temp);
                           down = true;
                       }
                   }
                   //weird bug here the third or will not run but it runs if turned into else statement
                   //nor will the second else if runs
                   //also a variably defined index will not used but the integer equiv will?
                   else{
                         if (j == 1 || j ==0)
                         {
                             CairoLine temp(currentX,currentY,currentX-tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == 3)
                         {
                             std::cout<<"currentY j=3: ";
                             std::cout<<currentY<<std::endl;
                             CairoLine temp(currentX,currentY,currentX-tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == secondlastLarge)
                         {
                             CairoLine temp(currentX,currentY,currentX-tabWidthLarge,currentY);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                             std::cout<<"secondLastValue";
                             std::cout<<secondlastLarge;
                         }
                         else if (j == linesofLength)
                         {
                             CairoLine temp(currentX,currentY+materialWidth,currentX-tabWidthLarge,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else
                         {
                             if (downSide)
                             {
                             CairoLine temp(currentX,currentY+materialWidth,currentX-fingWidth,currentY+materialWidth);
                             currentX = temp.getX2();
                             currentY = temp.getY1()-materialWidth;
                             std::printf("currentY horz: ");
                             std::cout<<currentY<<std::endl;
                             tempVect.push_back(temp);
                             downSide = false;
                             }
                             else if(!downSide)
                             {
                                 CairoLine temp(currentX,currentY,currentX-fingWidth,currentY);
                                 currentX = temp.getX2();
                                 currentY = temp.getY2();
                                 std::printf("currentY horz: ");
                                 std::cout<<currentY<<std::endl;
                                 tempVect.push_back(temp);
                                 downSide = true;
                             }
                         }
                         }
           }
           }
           else if (i==3)
           {
               int numberSmTabs = calcHighestOdd((rectTemp.getHeight())/pixelInch,this->fingWidth/pixelInch);
               double largeTabScale = calcLargeFingWidth((rectTemp.getHeight())/pixelInch,this->fingWidth/pixelInch, numberSmTabs);
               double tabWidthLarge = (largeTabScale+1)*fingWidth;

               //determine whether the vert line is down or up
               bool down = true;
               bool downSide = true; //second osscilation
               unsigned int linesofLength = numberSmTabs*2+7;
               unsigned int lastLarge = linesofLength-1;
               unsigned int secondlastLarge = linesofLength-2;
               for (unsigned int j = 1; j <= linesofLength; j++)
               {
                   if (j%2 == 0)
                   {
                       if (down == false)
                       {
                        CairoLine temp(currentX,currentY,currentX+materialWidth,currentY);
                        currentX = temp.getX2();
                        currentY = temp.getY2();
                        if (j == 2)
                        {
                        std::printf("currentY");
                        std::cout<<currentY<<std::endl;
                        }
                        tempVect.push_back(temp);
                        down = true;
                       }
                       else if (down==true);
                       {
                           CairoLine temp(currentX,currentY,currentX-materialWidth,currentY);
                           currentX = temp.getX2();
                           currentY = temp.getY2();
                           tempVect.push_back(temp);
                           down = false;
                       }
                   }
                   //weird bug here the third or will not ru but it runs if turned into else statement
                   //nor will the second else if runs
                   //also a variably defined index will not used but the integer equiv will?
                   else{
                         if (j ==  1 || j ==0)
                         {
                             CairoLine temp(currentX,currentY,currentX,currentY-tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == 3)
                         {
                             CairoLine temp(currentX,currentY,currentX,currentY-tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else if (j == linesofLength)
                         {
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY-tabWidthLarge);
                             currentX = temp.getX2()-materialWidth;
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                             std::cout<<"secondLastValue";
                             std::cout<<secondlastLarge;
                         }
                         else if (j == secondlastLarge)
                         {
                             CairoLine temp(currentX,currentY,currentX,currentY-tabWidthLarge);
                             currentX = temp.getX2();
                             currentY = temp.getY2();
                             tempVect.push_back(temp);
                             std::cout<<"longTab"<<std::endl;
                             std::cout<<j<<std::endl;
                         }
                         else
                         {
                             if (downSide)
                             {
                             CairoLine temp(currentX+materialWidth,currentY,currentX+materialWidth,currentY-fingWidth);
                             currentX = temp.getX2()-materialWidth;
                             currentY = temp.getY2();
                             std::printf("currentY horz: ");
                             std::cout<<currentY<<std::endl;
                             tempVect.push_back(temp);
                             downSide = false;
                             }
                             else if(!downSide)
                             {
                                 CairoLine temp(currentX,currentY,currentX,currentY-fingWidth);
                                 currentX = temp.getX2();
                                 currentY = temp.getY2();
                                 std::printf("currentY horz: ");
                                 std::cout<<currentY<<std::endl;
                                 tempVect.push_back(temp);
                                 downSide = true;
                             }
                         }
                         }
           }
           }
       }
    }
    return tempVect;
}

void RenderArea::cairoRectangle(Cairo::RefPtr<Cairo::Context> crTemp, CairoRect rectArr[6])
{
    for(int i = 0; i < 6; i++)
    {
        crTemp->set_source_rgb(pen.color().red(),pen.color().green(),pen.color().blue());
        crTemp->rectangle(rectArr[i].getXPos(),rectArr[i].getYPos(), rectArr[i].getWidth(),rectArr[i].getHeight());
        crTemp->stroke();
    }
}


void RenderArea::renderSide(QPainter &painter, CairoRect rectTemp)
{
    CairoRect temp = rectTemp;
    temp.setPos(10,10);
    std::vector<CairoLine> fingerSlots = fingerGenerator(temp);

    unsigned int fingerSlotsSize = fingerSlots.size();
    for (unsigned int i = 0; i <fingerSlotsSize; i++)
    {
        CairoLine temp = fingerSlots.at(i);
        painter.drawLine(temp.toQPoint1(), temp.toQPoint2());
    }
}

void RenderArea::paint(QPainter &painter)
{
    painter.setPen(pen);
    painter.setBrush(brush);
    if (antialiased)
        painter.setRenderHint(QPainter::Antialiasing, true);

            switch (shape) {
            case Rect:
            {
                /*
                CairoRect rect1(100,100, length, width2, CairoRect::WL);
                CairoRect rect5(rect1.getXPos()+materialWidth, rect1.getYPos()+rect1.getHeight()+10, length, width2, CairoRect::WL);
                CairoRect rect2(rect5.getXPos()+rect5.getWidth()+10+materialWidth, rect5.getYPos(), length, height2, CairoRect::LH);
                CairoRect rect6(rect2.getXPos()+materialWidth, rect2.getYPos()+rect2.getHeight()+50, length, height2, CairoRect::LH);
                CairoRect rect3(rect6.getXPos()+rect6.getWidth()+50+materialWidth, rect6.getYPos(), height2, width2, CairoRect::WH);
                CairoRect rect4(rect3.getXPos()+materialWidth, rect3.getYPos()+rect3.getHeight()+10, height2, width2, CairoRect::WH);
                CairoRect rectArr[6] = {rect1,rect2,rect3,rect4,rect5,rect6};
                */
                if (fingerSlotBool == true)
                {
                if (fingWidth !=0 && fingWidth != NULL)
                {

                    if (sideIndex == 0)
                    {
                    for (CairoRect rect : this->rectsForRect)
                    {
                    std::vector<CairoLine> fingerSlots = fingerGenerator(rect);
                unsigned int fingerSlotsSize = fingerSlots.size();
                for (unsigned int i = 0; i < fingerSlotsSize; i++)
                {
                    CairoLine temp = fingerSlots.at(i);
                    painter.drawLine(temp.toQPoint1(),temp.toQPoint2());
                }
                    }

            }
                else
                {
                    renderSide(painter,rectsForRect[sideIndex-1]);

                }

                }
                }
                else
                { if (sideIndex == 0)
                    {
                    for (int i = 0; i < 5; i++)
                    {
                        painter.drawRect(rectsForRect[i].toQRect());
                    }
                    }
                    else
                    {
                        CairoRect tempRect = rectsForRect[sideIndex-1];
                        tempRect.setPos(50,50);
                        painter.drawRect(tempRect.toQRect());


                }
            }
            }break;

            case TriPrism:
                {
                //needed for weird issue with getting a correct triangle


                std::cout<<"inTri"<<std::endl;
                std::cout<<"testing- scaling factor"<<std::endl;
                scalingFactor = 2.54*pixelInch;
                std::cout<<this->length<<std::endl;
                CairoTri temp = CairoTri(this->length/scalingFactor,this->width2/scalingFactor,this->height2/scalingFactor,this->depth,0,0);
                painter.drawLine(scalePoint(temp.getSide1().toQPoint1()),scalePoint(temp.getSide1().toQPoint2()));
                std::cout<<temp.getSide1().getX2()<<std::endl;
                painter.drawLine(scalePoint(temp.getSide2().toQPoint1()),scalePoint(temp.getSide2().toQPoint2()));
                std::cout<<temp.getSide2().getY2()<<std::endl;
                std::cout<<temp.getSide2().getX2()<<std::endl;
                painter.drawLine(scalePoint(temp.getSide3().toQPoint1()),scalePoint(temp.getSide3().toQPoint2()));

               }break;

            painter.restore();

        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setPen(palette().dark().color());
        painter.setBrush(Qt::NoBrush);
}
}

int * RenderArea::windowSize()
{
    int arr[2] = {windowLength,windowHeight};
    return arr;
}

//scales givenPoint by units
QPointF RenderArea::scalePoint(QPointF tempPoint)
{
    scalingFactor = 2.54*pixelInch;
    QPointF tempPoint2 = QPointF(tempPoint.x()*scalingFactor,tempPoint.y()*scalingFactor);
    return tempPoint2;
}


void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter;
       painter.begin(this);
       painter.setRenderHint(QPainter::Antialiasing);
       paint(painter);
       painter.end();


       QTemporaryDir dir;
       if (dir.isValid())
       {
      paintCairo(dir.path().toStdString()+"tempsvg.svg");

}

}
