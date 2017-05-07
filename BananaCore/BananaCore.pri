INCLUDEPATH += $$PWD/Include

CONFIG += c++14

win32 {
    #QMAKE_CXXFLAGS += /std:c++latest
    CONFIG(debug, debug|release) {
#        LIBS += $$PWD/../Build/Debug/BananaCore.lib
    }else {
#        LIBS += $$PWD/../Build/Release/BananaCore.lib
    }
}

macx {
    CONFIG(debug, debug|release) {
#        LIBS += $$PWD/../Build/Debug/libBananaCore.a
    }else {
#        LIBS += $$PWD/../Build/Release/libBananaCore.a
    }
}
