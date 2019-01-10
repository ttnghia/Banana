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

win32 {
    CONFIG(debug, debug|release) {
        message("QtAppHelpers -- Debug")
        LIBS += $$PWD/../Build/Debug/QtAppHelpers.lib
        PRE_TARGETDEPS += $$PWD/../Build/Debug/QtAppHelpers.lib
    } else {
        message("QtAppHelpers -- Release")
        static {
            LIBS += $$PWD/../Build/ReleaseStaticBuild/QtAppHelpers.lib
            PRE_TARGETDEPS += $$PWD/../Build/ReleaseStaticBuild/QtAppHelpers.lib
        } else {
            LIBS += $$PWD/../Build/Release/QtAppHelpers.lib
            PRE_TARGETDEPS += $$PWD/../Build/Release/QtAppHelpers.lib
        }
    }
}

macx|unix {
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

    CONFIG(debug, debug|release) {
        message("QtAppHelpers -- Debug")
        QMAKE_CXXFLAGS += -O0 -g
        LIBS += $$PWD/../Build/Debug/libQtAppHelpers.a
        PRE_TARGETDEPS += $$PWD/../Build/Debug/libQtAppHelpers.a
    }
    else {
        message("QtAppHelpers -- Release")
        QMAKE_CXXFLAGS += -O3
        LIBS += $$PWD/../Build/Release/libQtAppHelpers.a
        PRE_TARGETDEPS += $$PWD/../Build/Release/libQtAppHelpers.a
    }
}


#include(C:\Qt\Qt5.11.2\Examples\Qt-5.11.2\widgets/painting/shared/shared.pri)
include($$PWD/ArthurStyle/ArthurStyle.pri)

INCLUDEPATH += $$PWD/ArthurStyle
INCLUDEPATH += $$PWD/Include
INCLUDEPATH += $$PWD/../Externals/glm
#INCLUDEPATH += $$PWD/../Externals/AntTweakBar/include

#win32 {
#    LIBS += -lAntTweakBar64 -L$$PWD/../Externals/AntTweakBar/lib
#}
#macx {
#    LIBS += -lAntTweakBar -L$$PWD/../Externals/AntTweakBar/lib
#}

#message(Qt version: $$[QT_VERSION])
#message(Qt is installed in $$[QT_INSTALL_PREFIX])
#message(Qt resources can be found in the following locations:)
#message(Documentation: $$[QT_INSTALL_DOCS])
#message(Header files: $$[QT_INSTALL_HEADERS])
#message(Libraries: $$[QT_INSTALL_LIBS])
#message(Binary files (executables): $$[QT_INSTALL_BINS])
#message(Plugins: $$[QT_INSTALL_PLUGINS])
#message(Data files: $$[QT_INSTALL_DATA])
#message(Translation files: $$[QT_INSTALL_TRANSLATIONS])
#message(Settings: $$[QT_INSTALL_CONFIGURATION])
#message(Examples: $$[QT_INSTALL_EXAMPLES])
