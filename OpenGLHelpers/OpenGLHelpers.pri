INCLUDEPATH += $$PWD/Include
INCLUDEPATH += $$PWD/../Externals/glew-2.0.0/include

win32 {
    CONFIG(debug, debug|release) {
        LIBS += $$PWD/../Build/DebugQt/OpenGLHelpers.lib
    }else {
        LIBS += $$PWD/../Build/ReleaseQt/OpenGLHelpers.lib
    }
}


LIBS += -lglew32 -L$$PWD/../Externals/glew-2.0.0/lib/Release/x64
