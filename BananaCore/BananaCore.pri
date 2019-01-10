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
    QMAKE_CXXFLAGS += /std:c++17
    QMAKE_CXXFLAGS += /MP /W3 /Zc:wchar_t /Zi /Gm- /fp:precise /Zp16 /FC /EHsc
    QMAKE_CXXFLAGS += /D "_WINDOWS" /D "WIN32" /D "WIN64" /D "_MBCS" /D "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS"

    CONFIG(debug, debug|release) {
        message("Banana -- Debug")
        QMAKE_CXXFLAGS += /D "_DEBUG"
        QMAKE_CXXFLAGS += /Od /MDd
        LIBS += $$PWD/../Build/Debug/BananaCore.lib
        PRE_TARGETDEPS += $$PWD/../Build/Debug/BananaCore.lib
    } else {
        message("Banana -- Release")
        QMAKE_CXXFLAGS += /Zo /Qpar /GL /Gy /O2 /Ob2 /Oi /Ot /GF
        QMAKE_CXXFLAGS += /D "NDEBUG"
        QMAKE_LFLAGS += /LTCG:INCREMENTAL
        static {
            QMAKE_CXXFLAGS += /MT
            LIBS += $$PWD/../Build/ReleaseStaticBuild/BananaCore.lib
            PRE_TARGETDEPS += $$PWD/../Build/ReleaseStaticBuild/BananaCore.lib
        } else {
            QMAKE_CXXFLAGS += /MD
            LIBS += $$PWD/../Build/Release/BananaCore.lib
            PRE_TARGETDEPS += $$PWD/../Build/Release/BananaCore.lib
        }
    }

    INCLUDEPATH += $$PWD/../Externals/tbb_win/include
    LIBS += -ltbb -L$$PWD/../Externals/tbb_win/lib/intel64/vc14
}

macx|unix {
    CONFIG(debug, debug|release) {
        LIBS += $$PWD/../Build/Debug/libBananaCore.a
        PRE_TARGETDEPS += $$PWD/../Build/Debug/libBananaCore.a
    }else {
        LIBS += $$PWD/../Build/Release/libBananaCore.a
        PRE_TARGETDEPS += $$PWD/../Build/Release/libBananaCore.a
    }
}

macx {
    INCLUDEPATH += $$PWD/../Externals/tbb_osx/include
    QMAKE_LFLAGS += -Wl,-rpath=$$PWD/../Externals/tbb_osx/lib
    LIBS += -ltbb -L$$PWD/../Externals/tbb_osx/lib
}

unix {
    INCLUDEPATH += $$PWD/../Externals/tbb_linux/include
    QMAKE_LFLAGS += -Wl,-rpath=$$PWD/../Externals/tbb_linux/lib/intel64/gcc4.7
    LIBS += -ltbb -L$$PWD/../Externals/tbb_linux/lib/intel64/gcc4.7
}
