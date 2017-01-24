#-------------------------------------------------
#
# Project created by QtCreator 2016-08-10T23:43:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG+=c++11

TARGET = QtAppHelpers

TEMPLATE = lib

CONFIG += staticlib

INCLUDEPATH += ./Include

CONFIG(debug, debug|release) {
    message("Debug")
    QMAKE_CXXFLAGS += /DEBUG /INCREMENTAL:NO /Zi  /D "_DEBUG"
    DESTDIR = $$PWD/../Build/Debug

    LIBS += ../Build/Debug
}else {
    message("Release")
    QMAKE_CXXFLAGS += /O2 /Ob2 /GL /Qpar
    DESTDIR = $$PWD/../Build/Release

    LIBS += ../Build/Release
}



macx {
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

HEADERS += \
    Include/OpenGLHelpers/ArcBall.h \
    Include/OpenGLHelpers/Camera.h \
    Include/OpenGLHelpers/CubeObject.h \
    Include/OpenGLHelpers/GridObject.h \
    Include/OpenGLHelpers/Light.h \
    Include/OpenGLHelpers/Material.h \
    Include/OpenGLHelpers/MeshLoader.h \
    Include/OpenGLHelpers/MeshObject.h \
    Include/OpenGLHelpers/OpenGLBuffer.h \
    Include/OpenGLHelpers/OpenGLMacros.h \
    Include/OpenGLHelpers/Shader.h \
    Include/OpenGLHelpers/SphereObject.h

SOURCES += \
    Source/Camera.cpp \
    Source/Light.cpp \
    Source/Material.cpp \
    Source/MeshLoader.cpp \
    Source/MeshObject.cpp \
    Source/OpenGLBuffer.cpp \
    Source/Shader.cpp

