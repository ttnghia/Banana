QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

TARGET = OpenGLHelpers

TEMPLATE = lib

CONFIG += staticlib

INCLUDEPATH += ./Include
INCLUDEPATH += $$PWD/../BananaCore/Include
INCLUDEPATH += $$PWD/../QtAppHelpers/Include
INCLUDEPATH += $$PWD/../Externals/glm


CONFIG(debug, debug|release) {
    message("Debug")
    win32 {
         QMAKE_CXXFLAGS += /DEBUG /INCREMENTAL:NO /Zi /D "_DEBUG"
    }
    macx {
        QMAKE_CXXFLAGS += -g
    }
    DESTDIR = $$PWD/../Build/Debug
    LIBS += ../Build/Debug
}else {
    message("Release")
    win32 {
         QMAKE_CXXFLAGS += /O3 /Ob2 /GL /Qpar
    }
    macx {
        QMAKE_CXXFLAGS += -O3
    }
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
    Include/OpenGLHelpers/Lights.h \
    Include/OpenGLHelpers/Material.h \
    Include/OpenGLHelpers/MeshObject.h \
    Include/OpenGLHelpers/OpenGLBuffer.h \
    Include/OpenGLHelpers/OpenGLMacros.h \
    Include/OpenGLHelpers/OpenGLTexture.h \
    Include/OpenGLHelpers/RenderObjects.h \
    Include/OpenGLHelpers/ShaderProgram.h \
    Include/OpenGLHelpers/SphereObject.h \
    Include/OpenGLHelpers/WireFrameBoxObject.h

SOURCES += \
    Source/Camera.cpp \
    Source/Lights.cpp \
    Source/Material.cpp \
    Source/MeshObject.cpp \
    Source/OpenGLBuffer.cpp \
    Source/OpenGLTexture.cpp \
    Source/RenderObjects.cpp \
    Source/ShaderProgram.cpp

DISTFILES += \
    OpenGLHelpers.pri \
    OpenGLHelpers.vcxproj.filters \
    OpenGLHelpers.licenseheader \
    OpenGLHelpers.vcxproj

