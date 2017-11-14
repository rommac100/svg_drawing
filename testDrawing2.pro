 #-------------------------------------------------
#
# Project created by QtCreator 2016-04-17T10:13:44
#
#-------------------------------------------------

QT += core
QT += widgets
QT += svg
QT += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = testDrawing2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    renderarea.cpp \
    qcairopaintdevice.cpp \
    qcairopaintengine.cpp \
    cairorect.cpp \
    cairoline.cpp \
    cairotri.cpp \
    cairoround.cpp \
    holecairo.cpp

HEADERS  += mainwindow.h \
    renderarea.h \
    qcairopaintdevice.h \
    qcairopaintengine.h \
    cairorect.h \
    cairoline.h \
    cairotri.h \
    cairoround.h \
    holecairo.h

FORMS    += \
    mainwindow.ui

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += cairomm-1.0

unix: PKGCONFIG += cairo
