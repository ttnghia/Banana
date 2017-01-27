#-------------------------------------------------
#
# Project created by QtCreator 2016-09-01T19:13:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SkyBox
TEMPLATE = app

include (../../../BananaAppCore/BananaAppCore.pri)
include (../../../QtAppHelpers/QtAppHelpers.pri)
include (../../../OpenGLHelpers/OpenGLHelpers.pri)

SOURCES +=\
    Main.cpp \
    MainWindow.cpp

HEADERS += \
    MainWindow.h
