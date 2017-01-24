INCLUDEPATH += $$PWD/Include
INCLUDEPATH += $$PWD/../Externals/glew-2.0.0/include

win32 {
    CONFIG(debug, debug|release) {
        LIBS += $$PWD/../Build/Debug/OpenGLHelpers.lib
    }else {
        LIBS += $$PWD/../Build/Release/OpenGLHelpers.lib
    }
}


LIBS += -lglew32 -L$$PWD/../Externals/glew-2.0.0/lib/Release/x64
