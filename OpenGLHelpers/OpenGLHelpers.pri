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
INCLUDEPATH += $$PWD/../Externals/glew-2.0.0/include
INCLUDEPATH += $$PWD/../Externals/tinyply/source
INCLUDEPATH += $$PWD/../Externals/tinyobjloader

CONFIG += c++14

win32 {
    CONFIG(debug, debug|release) {
        message("OpenGLHelpers -- Debug")
        LIBS += $$PWD/../Build/DebugQt/OpenGLHelpers.lib
    }else {
        message("OpenGLHelpers -- Release")
        static {
            LIBS += $$PWD/../Build/ReleaseStaticBuild/OpenGLHelpers.lib
        } else {
            LIBS += $$PWD/../Build/ReleaseQt/OpenGLHelpers.lib
        }
    }
}


macx {
    CONFIG(debug, debug|release) {
        message("OpenGLHelpers -- Debug")
        LIBS += $$PWD/../Build/Debug/libOpenGLHelpers.a
    }else {
        message("OpenGLHelpers -- Release")
        LIBS += $$PWD/../Build/Release/libOpenGLHelpers.a
    }
}
