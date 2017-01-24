INCLUDEPATH += $$PWD/Include
INCLUDEPATH += $$PWD/../Externals/glew-2.0.0/include

win32 {
    CONFIG(debug, debug|release) {
        message("OpenGLHelpers -- Debug")
        LIBS += $$PWD/../Build/DebugQt/OpenGLHelpers.lib
    }else {
        message("OpenGLHelpers -- Release")
        LIBS += $$PWD/../Build/ReleaseQt/OpenGLHelpers.lib
    }
}


LIBS += -lglew32 -L$$PWD/../Externals/glew-2.0.0/lib/Release/x64
