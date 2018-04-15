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

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../Externals/spdlog/include
INCLUDEPATH += $$PWD/../Externals/json/single_include/nlohmann

CONFIG += c++17
CONFIG += force_debug_info

static {
    CONFIG += static
    DEFINES += STATIC
    message("~~~ Static build ~~~") # this is for information, that the static build is done
} else {
    message("~~~ Dynamic build ~~~")
}

win32 {
    CONFIG(debug, debug|release) {
        message("Banana -- Debug")
        QMAKE_CXXFLAGS += /D "_DEBUG" /wd"4305"
        LIBS += $$PWD/../Build/Debug/BananaCore.lib
        PRE_TARGETDEPS += $$PWD/../Build/Debug/BananaCore.lib
    } else {
        message("Banana -- Release")
        QMAKE_CXXFLAGS += /Zo /Qpar /GL /W3 /Gy /Gm- /O2 /Ob2 /fp:precise /D "NDEBUG" /fp:except /Oi /EHsc /Ot /wd"4305"
        QMAKE_LFLAGS += /LTCG:INCREMENTAL
        static {
            LIBS += $$PWD/../Build/ReleaseStaticBuild/BananaCore.lib
            PRE_TARGETDEPS += $$PWD/../Build/ReleaseStaticBuild/BananaCore.lib
        } else {
            LIBS += $$PWD/../Build/Release/BananaCore.lib
            PRE_TARGETDEPS += $$PWD/../Build/Release/BananaCore.lib
        }
    }

    QMAKE_CXXFLAGS += /std:c++latest
    INCLUDEPATH += $$PWD/../Externals/tbb_win/include
    LIBS += -ltbb -L$$PWD/../Externals/tbb_win/lib/intel64/vc14
}

macx {
    CONFIG(debug, debug|release) {
        LIBS += $$PWD/../Build/Debug/libBananaCore.a
        PRE_TARGETDEPS += $$PWD/../Build/Debug/libBananaCore.a
    }else {
        LIBS += $$PWD/../Build/Release/libBananaCore.a
        PRE_TARGETDEPS += $$PWD/../Build/Release/libBananaCore.a
    }

    INCLUDEPATH += $$PWD/../Externals/tbb_osx/include
    LIBS += -ltbb -L$$PWD/../Externals/tbb_osx/lib
}
