INCLUDEPATH += $$PWD/Include

CONFIG += c++14

win32 {
    #QMAKE_CXXFLAGS += /std:c++latest
    CONFIG(debug, debug|release) {
        LIBS += $$PWD/../Build/Debug/BananaAppCore.lib
    }else {
        LIBS += $$PWD/../Build/Release/BananaAppCore.lib
    }
}

macx {
    CONFIG(debug, debug|release) {
        LIBS += $$PWD/../Build/Debug/libBananaAppCore.a
    }else {
        LIBS += $$PWD/../Build/Release/libBananaAppCore.a
    }
}
