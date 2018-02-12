#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#
#  Copyright (c) 2017 by
#       __      _     _         _____
#    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
#   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
#  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
#  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
#         |___/                                              |___/
#
#  <nghiatruong.vn@gmail.com>
#  All rights reserved.
#
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(../../../BananaCore/BananaCore.pri)
include(../../../QtAppHelpers/QtAppHelpers.pri)
include(../../../OpenGLHelpers/OpenGLHelpers.pri)
include(../../../Simulation/Simulation.pri)

TARGET = ParticleSimulation
TEMPLATE = app
CONFIG += console

#QMAKE_LFLAGS += /VERBOSE
#CONFIG += static

#macx: ICON = $${PWD}/Resource/Icons/Bananas.icns
win32: RC_ICONS = $$PWD/../../../Assets/Icons/Bananas.ico

INCLUDEPATH += $$PWD/Include


HEADERS += \
    Source/MainWindow.h \
    Source/Common.h \
    Source/Simulator.h \
    Source/RenderWidget.h \
    Source/Controller.h \
    Source/ParticleSolverInterface.h

SOURCES += \
    Source/Main.cpp \
    Source/MainWindow.cpp \
    Source/Simulator.cpp \
    Source/RenderWidget.cpp \
    Source/Controller.cpp

RESOURCES += \
    Shader.qrc

DISTFILES += \
    Scenes/_Testing.json \
    Scenes/FLIP3D_2DamBreak.json \
    Scenes/FLIP3D_BallDrop.json \
    Scenes/SPH3D_BallDrop.json \
    Scenes/FLIP3D_RotationalBoxBoundary.json \
    Scenes/MPM2D_2Spheres.json \
    Scenes/MPM3D_2Spheres.json \
    Scenes/MPM3D_2Toruses.json \
    Scenes/MPM3D_TorusBallDrop.json \
    config.ini
