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

INCLUDEPATH += $$PWD/Include
INCLUDEPATH += $$PWD/../Externals/tinyply/source
INCLUDEPATH += $$PWD/../Externals/tinyobjloader

win32 {
    CONFIG(debug, debug|release) {
        message("OpenGLHelpers -- Debug")
        LIBS += $$PWD/../Build/Debug/OpenGLHelpers.lib
        PRE_TARGETDEPS += $$PWD/../Build/Debug/OpenGLHelpers.lib
    } else {
        message("OpenGLHelpers -- Release")
        static {
            LIBS += $$PWD/../Build/ReleaseStaticBuild/OpenGLHelpers.lib
            PRE_TARGETDEPS += $$PWD/../Build/ReleaseStaticBuild/OpenGLHelpers.lib
        } else {
            LIBS += $$PWD/../Build/Release/OpenGLHelpers.lib
            PRE_TARGETDEPS += $$PWD/../Build/Release/OpenGLHelpers.lib
        }
    }
}


macx {
    CONFIG(debug, debug|release) {
        message("OpenGLHelpers -- Debug")
        LIBS += $$PWD/../Build/Debug/libOpenGLHelpers.a
        PRE_TARGETDEPS += $$PWD/../Build/Debug/libOpenGLHelpers.a
    }else {
        message("OpenGLHelpers -- Release")
        LIBS += $$PWD/../Build/Release/libOpenGLHelpers.a
        PRE_TARGETDEPS += $$PWD/../Build/Release/libOpenGLHelpers.a
    }
}
