INCLUDEPATH += $$PWD/Include

CONFIG += c++14

win32 {
    #QMAKE_CXXFLAGS += /std:c++latest
    CONFIG(debug, debug|release) {
#        LIBS += $$PWD/../Build/Debug/BananaCore.lib
    }else {
#        LIBS += $$PWD/../Build/Release/BananaCore.lib
    }

    INCLUDEPATH += $$PWD/../Externals/tbb_win/include
    LIBS += -ltbb -L$$PWD/../Externals/tbb_win/lib/intel64/vc14
}

macx {
    CONFIG(debug, debug|release) {
#        LIBS += $$PWD/../Build/Debug/libBananaCore.a
    }else {
#        LIBS += $$PWD/../Build/Release/libBananaCore.a
    }

    INCLUDEPATH += $$PWD/../Externals/tbb_osx/include
    LIBS += -ltbb -L$$PWD/../Externals/tbb_osx/lib
}

