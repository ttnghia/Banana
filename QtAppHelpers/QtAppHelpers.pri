macx {
  QMAKE_MAC_SDK = macosx10.12
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12
}

#CONFIG += warn_off
CONFIG += c++11
macx {
    INCLUDEPATH +=
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
    CONFIG(release, debug|release): QMAKE_CXXFLAGS += -O3 -s -DNDEBUG
    CONFIG(debug, debug|release):QMAKE_CXXFLAGS += -O0 -g
}

win32 {
    CONFIG(debug, debug|release) {
        message("QtAppHelpers -- Debug")
        QMAKE_CXXFLAGS += /DEBUG /Zi /D "_DEBUG"
        LIBS += $$PWD/../Build/DebugQt/QtAppHelpers.lib
    }else {
        message("QtAppHelpers -- Release")
        QMAKE_CXXFLAGS += /O2 /Ob2 /GL /Qpar
        LIBS += $$PWD/../Build/ReleaseQt/QtAppHelpers.lib
    }
}


include($$[QT_INSTALL_EXAMPLES]/widgets/painting/shared/shared.pri)

INCLUDEPATH += $$PWD/Include
INCLUDEPATH += $$PWD/../Externals/glm
INCLUDEPATH += $$PWD/../Externals/AntTweakBar/include

win32 {
    LIBS += -lAntTweakBar64 -L$$PWD/../Externals/AntTweakBar/lib
}
macx {
    LIBS += -lAntTweakBar -L$$PWD/../Externals/AntTweakBar/lib
}

#message(Qt version: $$[QT_VERSION])
#message(Qt is installed in $$[QT_INSTALL_PREFIX])
#message(Qt resources can be found in the following locations:)
#message(Documentation: $$[QT_INSTALL_DOCS])
#message(Header files: $$[QT_INSTALL_HEADERS])
#message(Libraries: $$[QT_INSTALL_LIBS])
#message(Binary files (executables): $$[QT_INSTALL_BINS])
#message(Plugins: $$[QT_INSTALL_PLUGINS])
#message(Data files: $$[QT_INSTALL_DATA])
#message(Translation files: $$[QT_INSTALL_TRANSLATIONS])
#message(Settings: $$[QT_INSTALL_CONFIGURATION])
#message(Examples: $$[QT_INSTALL_EXAMPLES])