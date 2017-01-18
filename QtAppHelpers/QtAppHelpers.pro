#-------------------------------------------------
#
# Project created by QtCreator 2016-08-10T23:43:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG+=c++11
CONFIG+=warn_off
win32 {
    QMAKE_CXXFLAGS += /DEBUG /EHsc /O2
}

TARGET = Mango

#TEMPLATE = app
TEMPLATE = lib

CONFIG += staticlib
DESTDIR = $$PWD/Build

INCLUDEPATH += ../../Noodle/Engine/Include
INCLUDEPATH += ./Include

INCLUDEPATH += ./Libs
INCLUDEPATH += ./Libs/glm
INCLUDEPATH += ./Libs/vcg
INCLUDEPATH += ./Libs/AntTweakBar/include


LIBS += -lNoodle -L../../Noodle/Engine/Build/
LIBS += -lAntTweakBar -L./Libs/AntTweakBar/

macx {
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

HEADERS += \
    Include/Mango/Core/Camera.h \
    Include/Mango/Core/OpenGLWidget.h \
    Include/Mango/Core/Light.h \
    Include/Mango/Core/Material.h \
    Include/Mango/Core/ArcBall.h \
    Include/Mango/Core/MeshModel.h \
    Libs/tiny_obj_loader/tiny_obj_loader.h \
    Libs/tinyply/tinyply.h \
    Include/Mango/Core/MangoDefinition.h \
    Include/Mango/Core/OpenGLMainWindow.h \
    Include/Mango/Core/AntTweakBarWrapper.h \
    Include/Mango/Core/Macro.h \
    Include/Mango/Core/Shader.h \
    Include/Mango/Core/AvgTimer.h

SOURCES += \
    Source/Core/Camera.cpp \
    Source/Core/OpenGLWidget.cpp \
    Source/Core/Light.cpp \
    Source/Core/Material.cpp \
    Source/Core/MeshModel.cpp \
    Libs/tiny_obj_loader/tiny_obj_loader.cc \
    Libs/tinyply/example.cpp \
    Libs/tinyply/tinyply.cpp \
    Source/Core/OpenGLMainWindow.cpp \
    Source/Core/AntTweakBarWrapper.cpp \
    Source/Core/Shader.cpp \
    Source/Core/AvgTimer.cpp
