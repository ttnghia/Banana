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

include($$[QT_INSTALL_EXAMPLES]/widgets/painting/shared/shared.pri)
INCLUDEPATH += ./Include
INCLUDEPATH += ../Externals/glm
INCLUDEPATH += ../Externals/AntTweakBar/include
INCLUDEPATH += ../OpenGLHelpers/Include

LIBS += -lAntTweakBar -L../Externals/AntTweakBar/lib
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
    Include/QtAppHelpers/AntTweakBarWrapper.h \
    Include/QtAppHelpers/AvgTimer.h \
    Include/QtAppHelpers/DeviceToLogical.h \
    Include/QtAppHelpers/OpenGLMainWindow.h \
    Include/QtAppHelpers/OpenGLWidget.h \
    Include/QtAppHelpers/QtAppMacros.h \
    Include/QtAppHelpers/ColorSelector.h \
    Include/QtAppHelpers/QtAppShader.h \
    Include/QtAppHelpers/BrowsePathWidget.h \
    Include/QtAppHelpers/EnhancedComboBox.h \
    Include/QtAppHelpers/EnhancedSlider.h \
    Include/QtAppHelpers/FPSCounter.h

SOURCES += \
    Source/AntTweakBarWrapper.cpp \
    Source/AvgTimer.cpp \
    Source/BrowsePathWidget.cpp \
    Source/ColorSelector.cpp \
    Source/EnhancedComboBox.cpp \
    Source/EnhancedSlider.cpp \
    Source/FPSCounter.cpp \
    Source/OpenGLMainWindow.cpp \
    Source/OpenGLWidget.cpp \
    Source/QtAppShader.cpp
