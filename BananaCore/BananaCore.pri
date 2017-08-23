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

INCLUDEPATH += $$PWD/Include
INCLUDEPATH += $$PWD/../Externals/spdlog/include
INCLUDEPATH += $$PWD/../Externals/json/src

CONFIG += c++14

static {
    CONFIG += static
    DEFINES += STATIC
    message("~~~ static build ~~~") # this is for information, that the static build is done
}
else {
    message("~~~ dynamic build ~~~")
}

win32 {
    #QMAKE_CXXFLAGS += /std:c++latest
    CONFIG(debug, debug|release) {
        LIBS += $$PWD/../Build/Debug/BananaCore.lib
    }else {
        static {
            LIBS += $$PWD/../Build/ReleaseStaticBuild/BananaCore.lib
        } else {
            LIBS += $$PWD/../Build/Release/BananaCore.lib
        }
    }

    INCLUDEPATH += $$PWD/../Externals/tbb_win/include
    LIBS += -ltbb -L$$PWD/../Externals/tbb_win/lib/intel64/vc14
}

macx {
    CONFIG(debug, debug|release) {
        LIBS += $$PWD/../Build/Debug/libBananaCore.a
    }else {
        LIBS += $$PWD/../Build/Release/libBananaCore.a
    }

    INCLUDEPATH += $$PWD/../Externals/tbb_osx/include
    LIBS += -ltbb -L$$PWD/../Externals/tbb_osx/lib
}

