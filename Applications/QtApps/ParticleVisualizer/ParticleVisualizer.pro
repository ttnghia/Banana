#-------------------------------------------------
#
# Project created by QtCreator 2014-11-19T14:51:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ParticleVisualizer
TEMPLATE = app

INCLUDEPATH += $$PWD/Source

include(../../../BananaCore/BananaCore.pri)
include(../../../QtAppHelpers/QtAppHelpers.pri)
include(../../../OpenGLHelpers/OpenGLHelpers.pri)
include(../../../Simulation/Simulation.pri)

RESOURCES += Resources.qrc
macx: ICON = $${PWD}/Icons/Noodle.icns
win32: RC_ICONS = $$PWD/Icons/Noodle.ico

HEADERS += \
    Source/Controller.h \
    Source/MainWindow.h \
    Source/DataManager.h \
    Source/DataReader.h \
    Source/Common.h \
    Source/FRRenderObjects.h \
    Source/RenderWidget.h

SOURCES += \
    Source/Controller.cpp \
    Source/Main.cpp \
    Source/MainWindow.cpp \
    Source/DataManager.cpp \
    Source/DataReader.cpp \
    Source/FRRenderObjects.cpp \
    Source/RenderWidget.cpp

DISTFILES += \
    Shaders/mesh.fs.glsl \
    Shaders/mesh.vs.glsl \
    Shaders/particle.fs.glsl \
    Shaders/particle.vs.glsl \
    PlayList.txt \
    config.ini