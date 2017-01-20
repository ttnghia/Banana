//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Main.cpp
//
// Created on: 9/3/2016
//     Author: Nghia Truong
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <QApplication>
#include <QSurfaceFormat>
#include <QStyle>
#include <QDesktopWidget>

#include <Mango/Core/Macro.h>
#include "TestMeshModelWindow.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int main(int argc, char* argv[])
{
    __MANGO_RUN_MAIN_WINDOW(TestMeshModelWindow, argc, argv);
}
