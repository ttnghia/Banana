QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

TARGET = BananaCore

#TEMPLATE = lib
TEMPLATE = subdirs

#CONFIG += staticlib

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
    Include/BananaCore/AppConfigReader.h \
    Include/BananaCore/DataIO.h \
    Include/BananaCore/FileHelpers.h \
    Include/BananaCore/Macros.h \
    Include/BananaCore/NumberHelpers.h \
    Include/BananaCore/OptionalParameter.h \
    Include/BananaCore/ParticleSystemData.h \
    Include/BananaCore/STLHelpers.h \
    Include/BananaCore/Timer.h \
    Include/BananaCore/TypeNames.h \
    Include/BananaCore/Array/Array2.h \
    Include/BananaCore/Array/Array3.h \
    Include/BananaCore/Array/ArrayHelpers.h \
    Include/BananaCore/Data/DataIO.h \
    Include/BananaCore/Data/DataPrinter.h \
    Include/BananaCore/Data/ParticleSystemData.h \
    Include/BananaCore/Geometry/KDTree.h \
    Include/BananaCore/Geometry/SignDistanceField.h \
    Include/BananaCore/CallStack.h \
    Include/BananaCore/MathHelpers.h \
    Include/BananaCore/MemoryUsage.h \
    Include/BananaCore/SVD.h

DISTFILES += \
    BananaCore.pri \
    BananaCore.licenseheader \
    BananaCore.vcxproj \
    BananaCore.vcxproj.filters
