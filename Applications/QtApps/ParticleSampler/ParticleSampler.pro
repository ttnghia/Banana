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

include(../../../BananaCore/BananaCore.pri)
include(../../../QtAppHelpers/QtAppHelpers.pri)
include(../../../OpenGLHelpers/OpenGLHelpers.pri)
include(../../../Simulation/Simulation.pri)

TARGET = ParticleSampler
TEMPLATE = app
CONFIG += console

#CONFIG += static

#macx: ICON = $${PWD}/Resource/Icons/Bananas.icns
win32: RC_ICONS = $$PWD/../../../Assets/Icons/Bananas.ico

INCLUDEPATH += $$PWD/Include

HEADERS = $$files(Source/*.h, true)
SOURCES = $$files(Source/*.cpp, true)

RESOURCES += \
    Shader.qrc

DISTFILES += \
    Scenes/Sphere2D.json \
    Scenes/Sphere3D.json \
    Scenes/Torus3D.json \
    config.ini
