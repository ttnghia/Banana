#-------------------------------------------------
#
# Project created by QtCreator 2016-09-01T19:13:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenGLWidget
TEMPLATE = app


SOURCES +=\
    Main.cpp \
    TestFixedGLWidget.cpp \
    TestGLSLWidget.cpp \
    TestGLWidgetWindow.cpp

HEADERS  += \
    TestFixedGLWidget.h \
    TestGLSLWidget.h \
    TestGLWidgetWindow.h


RESOURCES += \
    shaders.qrc

include (../../General.pri)

win32 {
    LIBS += -lGLU32 -lopengl32
}
