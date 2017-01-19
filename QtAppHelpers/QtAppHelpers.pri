macx {
  QMAKE_MAC_SDK = macosx10.12
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12
}

#CONFIG += warn_off
CONFIG += c++11
macx {
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
    CONFIG(release, debug|release): QMAKE_CXXFLAGS += -O3 -s -DNDEBUG
    CONFIG(debug, debug|release):QMAKE_CXXFLAGS += -O0 -g
}

win32 {
    CONFIG(debug, debug|release) {
        message("Debug")
        QMAKE_CXXFLAGS += /DEBUG /Zi
        LIBS += $$PWD/../Build/Debug/QtAppHelpers.lib
    }else {
        message("Release")
        QMAKE_CXXFLAGS += /O2 /Ob2 /GL /Qpar
        LIBS += $$PWD/../Build/Release/QtAppHelpers.lib
    }
}


INCLUDEPATH += $$PWD/Include
INCLUDEPATH += $$PWD/../Externals/glm
INCLUDEPATH += $$PWD/../Externals/AntTweakBar/include

win32 {
#    INCLUDEPATH += $$PWD/../Externals/tbb_win/include
#    LIBS += -ltbb -L$$PWD/../Externals/tbb_win/lib/intel64/vc14
    LIBS += -lAntTweakBar64 -L$$PWD/../Externals/AntTweakBar/lib
}
macx {
#    INCLUDEPATH += $$PWD/../Externals/tbb_osx/include
#    LIBS += -ltbb -L$$PWD/../Externals/tbb_osx/lib
    LIBS += -lAntTweakBar -L$$PWD/../Externals/AntTweakBar/lib
}
