#-------------------------------------------------
#
# Project created by QtCreator 2016-09-01T19:13:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenGLWidget
TEMPLATE = app

include (../../../BananaAppCore/BananaAppCore.pri)
include (../../../QtAppHelpers/QtAppHelpers.pri)
include (../../../OpenGLHelpers/OpenGLHelpers.pri)

SOURCES +=\
    Main.cpp \
    MainWindow.cpp \
    ../../../Externals/tinyobjloader/tiny_obj_loader.cc \
    ../../../Externals/tinyply/source/tinyply.cpp

HEADERS += \
    MainWindow.h
