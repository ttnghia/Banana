#-------------------------------------------------
#
# Project created by QtCreator 2016-08-10T23:43:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG+=c++11

win32 {
CONFIG(debug, debug|release) {
    message("Debug")
    QMAKE_CXXFLAGS += /DEBUG /INCREMENTAL:NO /ZI
}else {
    message("Release")
    QMAKE_CXXFLAGS += /O2
}
}


TARGET = QtAppHelpers

TEMPLATE = lib

CONFIG += staticlib


INCLUDEPATH += ./Include
INCLUDEPATH += ../Externals/glm
INCLUDEPATH += ../Externals/AntTweakBar/include
INCLUDEPATH += ../OpenGLHelpers/Include

LIBS += -lAntTweakBar -L../Externals/AntTweakBar/lib
CONFIG(debug, debug|release) {
    message("Debug")
    DESTDIR = $$PWD/../Build/Debug

    LIBS += ../Build/Debug
}else {
    message("Release")
    DESTDIR = $$PWD/../Build/Release

    LIBS += ../Build/Release
}



macx {
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

HEADERS += \
    Include/QtAppHelpers/AntTweakBarWrapper.h \
    Include/QtAppHelpers/AvgTimer.h \
    Include/QtAppHelpers/DeviceToLogical.h \
    Include/QtAppHelpers/Macro.h \
    Include/QtAppHelpers/OpenGLMainWindow.h \
    Include/QtAppHelpers/OpenGLWidget.h \
    Include/QtAppHelpers/QtAppMacros.h

SOURCES += \
    Source/AntTweakBarWrapper.cpp \
    Source/AvgTimer.cpp \
    Source/OpenGLMainWindow.cpp \
    Source/OpenGLWidget.cpp

