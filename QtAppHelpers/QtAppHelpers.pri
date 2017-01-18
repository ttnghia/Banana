# this is the common include for anything compiled inside the Waoo project

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
    CONFIG(release, debug|release): QMAKE_CXXFLAGS += /O2
    CONFIG(debug, debug|release):QMAKE_CXXFLAGS += /DEBUG /O2

}


INCLUDEPATH += $$PWD/../Noodle/Engine/Include
INCLUDEPATH += $$PWD/Engine/Include
INCLUDEPATH += $$PWD/Engine/Libs/glm
INCLUDEPATH += $$PWD/Engine/Libs/
INCLUDEPATH += $$PWD/Engine/Libs/AntTweakBar/include

win32 {
    INCLUDEPATH += $$PWD/../Libs/tbb44_win/include
}
macx {
    INCLUDEPATH += $$PWD/../Noodle/Engine/Libs/tbb44_osx/include
}


LIBS += -L$$PWD/Engine/Build/ -lMango
DEPENDPATH += $$PWD/Engine/Build


win32 {
    LIBS += -lAntTweakBar64 -L$$PWD/Engine/Libs/AntTweakBar/lib
    LIBS += -ltbb -L$$PWD/../Libs/tbb44_win/lib/intel64/vc14
}
macx {
    LIBS += -lAntTweakBar -L$$PWD/Engine/Libs/AntTweakBar/lib
    LIBS += -ltbb -L$$PWD/../Libs/tbb44_osx/lib
}


win32 {
    PRE_TARGETDEPS += $$PWD/Engine/Build/Mango.lib
}
macx {
    PRE_TARGETDEPS += $$PWD/Engine/Build/libMango.a
}
