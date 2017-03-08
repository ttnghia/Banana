INCLUDEPATH += $$PWD/Include

win32 {
    QMAKE_CXXFLAGS += /std:c++latest
    CONFIG(debug, debug|release) {
        LIBS += $$PWD/../Build/Debug/BananaAppCore.lib
    }else {
        LIBS += $$PWD/../Build/Release/BananaAppCore.lib
    }
}
