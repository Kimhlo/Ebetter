#-------------------------------------------------
#
# Project created by QtCreator 2018-01-17T21:03:16
#
#-------------------------------------------------

QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = senceTest2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    philipshue.cpp \
    scene.cpp \
    curtainnew.cpp \
    dm836ii.cpp

HEADERS  += mainwindow.h \
    philipshue.h \
    scene.h \
    curtainnew.h \
    dm836ii.h

FORMS    += mainwindow.ui
LIBS     += -lwiringPi
