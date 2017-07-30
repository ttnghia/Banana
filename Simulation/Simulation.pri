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

macx {
  QMAKE_MAC_SDK = macosx10.12
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12
}

#CONFIG += warn_off
CONFIG += c++14

win32 {
    CONFIG(debug, debug|release) {
        message("Simulation -- Debug")
        QMAKE_CXXFLAGS += /DEBUG /Zi /D "_DEBUG" /wd"4305"
#        LIBS += $$PWD/../Build/Debug/Simulation.lib
    }else {
        message("Simulation -- Release")
        QMAKE_CXXFLAGS += /O2 /Ob2 /GL /Qpar /wd"4305"
#        LIBS += $$PWD/../Build/Release/Simulation.lib
    }
}

macx {
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

    CONFIG(debug, debug|release) {
        message("Simulation -- Debug")
        QMAKE_CXXFLAGS += -O0 -g
        LIBS += $$PWD/../Build/Debug/libSimulation.a
    }else {
        message("Simulation -- Release")
        QMAKE_CXXFLAGS += -O3
        LIBS += $$PWD/../Build/Release/libSimulation.a
    }
}


INCLUDEPATH += $$PWD/Include
