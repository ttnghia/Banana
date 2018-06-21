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

QT += core gui widgets

TARGET = HairViewer
TEMPLATE = app
#CONFIG += console

#QMAKE_LFLAGS += /VERBOSE
#CONFIG += static

BANANA_DIR = D:/Programming/Banana
include($$BANANA_DIR/BananaCore/BananaCore.pri)
include($$BANANA_DIR/QtAppHelpers/QtAppHelpers.pri)
include($$BANANA_DIR/OpenGLHelpers/OpenGLHelpers.pri)
include($$BANANA_DIR/Simulation/Simulation.pri)

#macx: ICON = $${PWD}/Resource/Icons/Bananas.icns
win32: RC_ICONS = $$PWD/../../../Assets/Icons/Bananas.ico

INCLUDEPATH += $$PWD/Include

HEADERS += $$files(Source/*.h, true)
SOURCES += $$files(Source/*.cpp, true)

DISTFILES += $$files(Scenes/*.json, true)
DISTFILES += $$files(*.ini, true)
RESOURCES += Shader.qrc
