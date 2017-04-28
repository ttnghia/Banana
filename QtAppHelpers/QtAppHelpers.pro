#-------------------------------------------------
#
# Project created by QtCreator 2016-08-10T23:43:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG+=c++14

TARGET = QtAppHelpers

TEMPLATE = lib

CONFIG += staticlib

include($$[QT_INSTALL_EXAMPLES]/widgets/painting/shared/shared.pri)
INCLUDEPATH += ./Include
INCLUDEPATH += $$PWD/../BananaAppcore/Include
INCLUDEPATH += $$PWD/../OpenGLHelpers/Include

INCLUDEPATH += $$PWD/../Externals/glm
INCLUDEPATH += $$PWD/../Externals/tinyobjloader
INCLUDEPATH += $$PWD/../Externals/tinyply/source
INCLUDEPATH += $$PWD/../Externals/AntTweakBar/include

LIBS += -lAntTweakBar -L../Externals/AntTweakBar/lib


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
    Include/QtAppHelpers/AntTweakBarWrapper.h \
    Include/QtAppHelpers/AvgTimer.h \
    Include/QtAppHelpers/BrowsePathWidget.h \
    Include/QtAppHelpers/ClipPlaneEditor.h \
    Include/QtAppHelpers/ColorPicker.h \
    Include/QtAppHelpers/CustomSplashScreen.h \
    Include/QtAppHelpers/DataList.h \
    Include/QtAppHelpers/DeviceToLogical.h \
    Include/QtAppHelpers/EnhancedComboBox.h \
    Include/QtAppHelpers/EnhancedSlider.h \
    Include/QtAppHelpers/FPSCounter.h \
    Include/QtAppHelpers/MaterialEditor.h \
    Include/QtAppHelpers/MaterialSelector.h \
    Include/QtAppHelpers/OpenGLMainWindow.h \
    Include/QtAppHelpers/OpenGLWidget.h \
    Include/QtAppHelpers/OpenGLWidgetTestRender.h \
    Include/QtAppHelpers/PointLightEditor.h \
    Include/QtAppHelpers/QtAppMacros.h \
    Include/QtAppHelpers/QtAppShaderProgram.h


SOURCES += \
    Source/AntTweakBarWrapper.cpp \
    Source/AvgTimer.cpp \
    Source/BrowsePathWidget.cpp \
    Source/ClipPlaneEditor.cpp \
    Source/ColorPicker.cpp \
    Source/CustomSplashScreen.cpp \
    Source/DataList.cpp \
    Source/EnhancedComboBox.cpp \
    Source/EnhancedSlider.cpp \
    Source/FPSCounter.cpp \
    Source/MaterialEditor.cpp \
    Source/MaterialSelector.cpp \
    Source/OpenGLMainWindow.cpp \
    Source/OpenGLWidget.cpp \
    Source/OpenGLWidgetTestRender.cpp \
    Source/PointLightEditor.cpp \
    Source/QtAppShaderProgram.cpp

