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

TARGET = TeapotRendering
TEMPLATE = app

INCLUDEPATH += $$PWD/Include

#LIBS += -lcuda -lcudart -loptix -loptixu -lsutil -L/usr/local/cuda-6.5/lib64 -L"C:/ProgramData/NVIDIA Corporation/OptiX SDK 4.1.0/lib64"


include (../../../BananaCore/BananaCore.pri)
include (../../../QtAppHelpers/QtAppHelpers.pri)
include (../../../OpenGLHelpers/OpenGLHelpers.pri)
include (../../../RayTracing/RayTracing.pri)


HEADERS += \
    Include/Controller.h \
    Include/MainWindow.h \
    Include/RenderWidget.h \
    Include/Common.h \
    CUDA/Common.cuh \
    CUDA/Helpers.cuh \
    CUDA/IntersectionRefinement.cuh \
    CUDA/Random.cuh \
    CUDA/Diffuse.cu \
    CUDA/Glass.cu \
    CUDA/GradientBG.cu \
    CUDA/ParallelogramIterative.cu \
    CUDA/PathTraceCamera.cu \
    CUDA/TriangleMesh.cu \
    Include/TeapotRayTracer.h

SOURCES += \
    Source/Controller.cpp \
    Source/Main.cpp \
    Source/MainWindow.cpp \
    Source/RenderWidget.cpp \
    Source/TeapotRayTracer.cpp
