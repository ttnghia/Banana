#-------------------------------------------------
#
# Project created by QtCreator 2014-11-19T14:51:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TextureAccess
TEMPLATE = app

CONFIG+=c++11
CONFIG+=warn_off

SOURCES +=\
    MainWindow.cpp \
    Main.cpp \
    FluidRenderer.cpp \
    AvgTimer.cpp

HEADERS  += \
    MainWindow.h \
    FluidRenderer.h \
    AvgTimer.h


RESOURCES += \
    Shaders.qrc

QMAKE_CXXFLAGS_DEBUG *= -O3
QMAKE_CXXFLAGS_RELEASE *= -O3


