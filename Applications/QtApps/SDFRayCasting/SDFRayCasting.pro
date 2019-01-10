#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#        __  __        _        __  ___ ____   __  ___
#       / / / /____ _ (_)_____ /  |/  // __ \ /  |/  /
#      / /_/ // __ `// // ___// /|_/ // /_/ // /|_/ /
#     / __  // /_/ // // /   / /  / // ____// /  / /
#    /_/ /_/ \__,_//_//_/   /_/  /_//_/    /_/  /_/
#
#    This file is part of HairMPM - Material Point Method for Hair Simulation.
#    Created: 2018. All rights reserved.
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

QT += core gui widgets

TARGET = SDFRayCasting
TEMPLATE = app

include($$PWD/../../Common.pri)
include($$PWD/../Apps.pri)

INCLUDEPATH += $$PWD/Source

HEADERS += $$files(Source/*.h, true)
SOURCES += $$files(Source/*.cpp, true)

DISTFILES += $$files(Shaders/*.glsl, true)
DISTFILES += $$files(*.txt, false)
DISTFILES += $$files(*.ini, false)
DISTFILES += $$files(Scenes/*.json, true)

RESOURCES += Shader.qrc
