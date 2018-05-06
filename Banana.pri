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

INCLUDEPATH += $$PWD/BananaCore
INCLUDEPATH += $$PWD/MachineLearning
INCLUDEPATH += $$PWD/OpenGLHelpers/Include
INCLUDEPATH += $$PWD/QtAppHelpers/Include
INCLUDEPATH += $$PWD/RayTracing/Include
INCLUDEPATH += $$PWD/Simulation


INCLUDEPATH += $$PWD/Externals/vdb-win
INCLUDEPATH += $$PWD/Externals/json/single_include/nlohmann
INCLUDEPATH += $$PWD/Externals/catch/single_include
INCLUDEPATH += $$PWD/Externals/tinyply/source
INCLUDEPATH += $$PWD/Externals/tinyobjloader
INCLUDEPATH += $$PWD/Externals/spdlog/include
INCLUDEPATH += $$PWD/Externals/glm
INCLUDEPATH += $$PWD/Externals/tbb_win/include

QT += core
CONFIG += c++17
CONFIG += force_debug_info

win32 {
    CONFIG(debug, debug|release) {
        message("Banana -- Debug")
        QMAKE_CXXFLAGS += /D "_DEBUG" /wd"4305"
    } else {
        message("Banana -- Release")
        QMAKE_CXXFLAGS += /Zo /Qpar /GL /W3 /Gy /Gm- /O2 /Ob2 /fp:precise /D "NDEBUG" /fp:except /Oi /EHsc /Ot /wd"4305"
    }

    QMAKE_CXXFLAGS += /std:c++latest
}

macx {
    CONFIG(debug, debug|release) {
    }else {
    }
}
