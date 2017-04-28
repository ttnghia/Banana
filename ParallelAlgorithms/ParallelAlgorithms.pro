QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG+=c++14

TARGET = ParallelAlgorithms

TEMPLATE = subdirs

#CONFIG += staticlib

INCLUDEPATH += ./Include
win32{
    INCLUDEPATH += $$PWD/../Externals/tbb_win/include
}
macx {
    INCLUDEPATH += $$PWD/../Externals/tbb_osx/include
}

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
    Include/ParallelAlgorithms/ParallelBLAS.h \
    Include/ParallelAlgorithms/ParallelClass.h \
    Include/ParallelAlgorithms/ParallelFuncs.h \
    Include/ParallelAlgorithms/ParallelSTL.h
