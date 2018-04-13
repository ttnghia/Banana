#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#                                .--,       .--,
#                               ( (  \.---./  ) )
#                                '.__/o   o\__.'
#                                   {=  ^  =}
#                                    >  -  <
#     ___________________________.""`-------`"".____________________________
#    /                                                                      \
#    \    This file is part of Banana - a graphics programming framework    /
#    /                    Created: 2018 by Nghia Truong                     \
#    \                      <nghiatruong.vn@gmail.com>                      /
#    /                      https://ttnghia.github.io                       \
#    \                        All rights reserved.                          /
#    /                                                                      \
#    \______________________________________________________________________/
#                                  ___)( )(___
#                                 (((__) (__)))
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SDFVisualizer
TEMPLATE = app

INCLUDEPATH += $$PWD/Include

include(../../../BananaCore/BananaCore.pri)
include(../../../QtAppHelpers/QtAppHelpers.pri)
include(../../../OpenGLHelpers/OpenGLHelpers.pri)

HEADERS += \
    Source/MainWindow.h \
    Source/Common.h \
    Source/RenderWidget.h \
    Source/Controller.h \
    Source/SDFGrid.h

SOURCES += \
    Source/Main.cpp \
    Source/MainWindow.cpp \
    Source/RenderWidget.cpp \
    Source/Controller.cpp \
    Source/SDFGrid.cpp

RESOURCES += \
    Shader.qrc

CONFIG+= static