#-------------------------------------------------
#
# Project created by QtCreator 2017-05-16T01:14:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = daq
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    recthread.cpp \
    global.cpp

HEADERS  += mainwindow.h \
    recthread.h \
    global.h

FORMS    += mainwindow.ui

LIBS +=-lwiringPi

QT += network

