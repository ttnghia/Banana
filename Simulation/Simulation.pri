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

CONFIG += c++17

INCLUDEPATH += $$PWD
INCLUDEPATH += $$BANANA_DIR/Externals/PartioBuild/src/lib

win32 {
#    QMAKE_LFLAGS += /WHOLEARCHIVE:Simulation.lib
    CONFIG(debug, debug|release) {
        message("Simulation -- Debug")
        LIBS += $$PWD/../Build/Debug/Simulation.lib
        LIBS += $$BANANA_DIR/Externals/PartioBuild/lib/Debug/partio.lib
        PRE_TARGETDEPS += $$PWD/../Build/Debug/Simulation.lib
    }else {
        message("Simulation -- Release")
        static {
            LIBS += $$PWD/../Build/ReleaseStaticBuild/Simulation.lib
            LIBS += $$BANANA_DIR/Externals/PartioBuild/lib/ReleaseStaticBuild/partio.lib
            PRE_TARGETDEPS += $$PWD/../Build/ReleaseStaticBuild/Simulation.lib
        } else {
            LIBS += $$PWD/../Build/Release/Simulation.lib
            LIBS += $$BANANA_DIR/Externals/PartioBuild/lib/Release/partio.lib
            PRE_TARGETDEPS += $$PWD/../Build/Release/Simulation.lib
        }
    }
}

macx|unix {
    QMAKE_MAC_SDK = macosx10.12
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

    CONFIG(debug, debug|release) {
        message("Simulation -- Debug")
        QMAKE_CXXFLAGS += -O0 -g
        LIBS += $$PWD/../Build/Debug/libSimulation.a
        PRE_TARGETDEPS += $$PWD/../Build/Debug/libSimulation.a
    }else {
        message("Simulation -- Release")
        QMAKE_CXXFLAGS += -O3
        LIBS += $$PWD/../Build/Release/libSimulation.a
        PRE_TARGETDEPS += $$PWD/../Build/Release/libSimulation.a
    }
}
