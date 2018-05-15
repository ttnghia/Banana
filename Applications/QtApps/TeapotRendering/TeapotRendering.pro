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

TARGET = TeapotRendering
TEMPLATE = app

INCLUDEPATH += $$PWD/Include

#LIBS += -lcuda -lcudart -loptix -loptixu -lsutil -L/usr/local/cuda-6.5/lib64 -L"C:/ProgramData/NVIDIA Corporation/OptiX SDK 4.1.0/lib64"

BANANA_DIR = D:/Programming/Banana
include($$BANANA_DIR/BananaCore/BananaCore.pri)
include($$BANANA_DIR/QtAppHelpers/QtAppHelpers.pri)
include($$BANANA_DIR/OpenGLHelpers/OpenGLHelpers.pri)
include($$BANANA_DIR/Simulation/Simulation.pri)
include($$BANANA_DIR/RayTracing/RayTracing.pri)


HEADERS += $$files(Include/*.h, true)
HEADERS += $$files(CUDA/*.cuh, true)
HEADERS += $$files(CUDA/*.cu, true)
SOURCES += $$files(Source/*.cpp, true)

RESOURCES += Shader.qrc

DISTFILES += $$files(Scenes/*.json, true)
DISTFILES += $$files(*.ini, true)