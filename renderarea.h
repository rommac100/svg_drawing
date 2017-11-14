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

#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QString>
#include <QColor>
#include <QSvgWidget>
#include <QPaintDevice>
#include <string>
#include <cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>
#include "cairorect.h"
#include "cairoline.h"
#include <QPointF>
#include <iostream>
#include "holecairo.h"
#include <vector>
class RenderArea : public QWidget
{
    Q_OBJECT

public:
    enum Shape { Rect, TriPrism, RoundRect};
    enum Units { Inches, Centimeters};

    RenderArea(QWidget *parent = 0);

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
    void setShape(Shape shape);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setUnits(Units units);
    void setPenColour(QString colour);
    void setAntialiased(bool antialiased);
    void setTransformed(bool transformed);
    void setWidth(long double Width);
    void setHeight(long double Height);
    void setLength(long double Length);
    void paint(QPainter &painter);
    int* windowSize();
    void paintCairo(std::string path);
    void setFingWidth(double fing_slot_width);
    void cairoRectangle(Cairo::RefPtr<Cairo::Context> crTemp, CairoRect rectArr[6]);
    std::vector<CairoLine> fingerGenerator(CairoRect rectTemp);
    void setMatThick(double thick);
    void setDepth(double depthArr);
    void setSide(int index);
    void renderSide(QPainter &painter, CairoRect rectTemp);
    void fingerSlotChecked(bool tempCheck);
    QPointF scalePoint(QPointF tempPoint);
    void addHole(double x, double y, double radius);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    Shape shape;
    Units unitsMain;
    QPen pen;
    QBrush brush;
    bool antialiased;
    bool transformed;
    QPixmap pixmap;
    char *user;
    double height2;
    double width2;
    double length;
    QColor penColour;
    double fingWidth;
    double materialWidth;
    double depth;
    bool fingerSlotBool;

    int sideIndex;

    //needed for weird issue w/ CairoTriangle
    double scalingFactor;
    //vector for the Rectangles
    CairoRect rectsForRect[6];


    int windowLength;
    int windowHeight;
};

#endif // RENDERAREA_H
