QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

TARGET = BananaAppCore

TEMPLATE = lib

CONFIG += staticlib

INCLUDEPATH += ./Include
INCLUDEPATH += $$PWD/../Externals/glm
INCLUDEPATH += $$PWD/../Externals/catch/include
INCLUDEPATH += $$PWD/../Externals/progress-cpp
INCLUDEPATH += $$PWD/../Externals/spdlog/include


CONFIG(debug, debug|release) {
    message("Debug")
    win32 {
         QMAKE_CXXFLAGS += /DEBUG /INCREMENTAL:NO /Zi /D "_DEBUG"
    }
    macx {
        QMAKE_CXXFLAGS += -g
    }
    DESTDIR = $$PWD/../Build/Debug
    LIBS += ../Build/Debug
}else {
    message("Release")
    win32 {
         QMAKE_CXXFLAGS += /O3 /Ob2 /GL /Qpar
    }
    macx {
        QMAKE_CXXFLAGS += -O3
    }
    DESTDIR = $$PWD/../Build/Release
    LIBS += ../Build/Release
}


macx {
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

HEADERS += \
    Include/BananaAppCore/AppConfigReader.h \
    Include/BananaAppCore/DataIO.h \
    Include/BananaAppCore/FileHelpers.h \
    Include/BananaAppCore/Macros.h \
    Include/BananaAppCore/NumberHelpers.h \
    Include/BananaAppCore/OptionalParameter.h \
    Include/BananaAppCore/ParticleSystemData.h \
    Include/BananaAppCore/STLHelpers.h \
    Include/BananaAppCore/Timer.h \
    Include/BananaAppCore/TypeNames.h

SOURCES += \
    Source/DataIO.cpp
