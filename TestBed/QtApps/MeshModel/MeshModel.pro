#-------------------------------------------------
#
# Project created by QtCreator 2016-09-03T10:53:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MeshModel
TEMPLATE = app



SOURCES +=\
    Main.cpp \
    TestMeshModelWindow.cpp \
    MeshRenderWidget.cpp

HEADERS  += \
    TestMeshModelWindow.h \
    MeshRenderWidget.h


RESOURCES += \
    shaders.qrc


include (../../General.pri)

CONFIG += warn_off
