#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T08:25:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lqwd
TEMPLATE = app


SOURCES += main.cpp\
        hq.cpp \
    widgets/console/bkons.cpp \
    lib/engine.cpp \
    lib/errormachine.cpp

HEADERS  += hq.h \
    widgets/console/bkons.h \
    lib/engine.h \
    lib/errormachine.h

FORMS    += hq.ui
