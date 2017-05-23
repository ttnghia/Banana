QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

TARGET = ParallelHelpers

#TEMPLATE = lib
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
    Include/ParallelHelpers/ParallelBLAS.h \
    Include/ParallelHelpers/ParallelFuncs.h \
    Include/ParallelHelpers/ParallelSTL.h \
    Include/ParallelHelpers/ParallelObjects.h \
    Include/ParallelHelpers/LinearAlgebra/BlockPCGSolver.h \
    Include/ParallelHelpers/LinearAlgebra/BlockSparseMatrix.h \
    Include/ParallelHelpers/LinearAlgebra/PCGSolver.h \
    Include/ParallelHelpers/LinearAlgebra/SparseMatrix.h \
    Include/ParallelHelpers/LinearAlgebra/BlockPCGSolver_Impl.hpp \
    Include/ParallelHelpers/LinearAlgebra/BlockSparseMatrix_Impl.hpp \
    Include/ParallelHelpers/LinearAlgebra/PCGSolver_Impl.hpp \
    Include/ParallelHelpers/LinearAlgebra/SparseMatrix_Impl.hpp

DISTFILES += \
    ParallelHelpers.pri \
    ParallelHelpers.licenseheader \
    ParallelHelpers.vcxproj \
    ParallelHelpers.vcxproj.filters \
    README.md
