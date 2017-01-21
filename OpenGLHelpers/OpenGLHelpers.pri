INCLUDEPATH += $$PWD/Include
INCLUDEPATH += $$PWD/../Externals/glew-2.0.0/include


#CONFIG(debug, debug|release) {
#    message("Debug")
#    LIBS += $$PWD/../Build/Debug/OpenGLHelpers.lib
#}else {
#    message("Release")
#    LIBS += $$PWD/../Build/Release/OpenGLHelpers.lib
#}

LIBS += -lglew32 -L$$PWD/../Externals/glew-2.0.0/lib/Release/x64