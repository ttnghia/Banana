//------------------------------------------------------------------------------------------
// main.cpp
//
// Created on: 8/10/2016
//     Author: Nghia Truong
//
//------------------------------------------------------------------------------------------
#include <QApplication>
#include <QSurfaceFormat>
#include <QStyle>

#include <Mango/Core/Macro.h>
#include "TestGLWidgetWindow.h"

//------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    __MANGO_RUN_MAIN_WINDOW(TestGLWidgetWindow, argc, argv);
}
