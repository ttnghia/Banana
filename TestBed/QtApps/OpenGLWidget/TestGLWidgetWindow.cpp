//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MainWindow.cpp
//
// Created on: 9/1/2016
//     Author: Nghia Truong
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include "TestGLWidgetWindow.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TestGLWidgetWindow::TestGLWidgetWindow(QWidget* parent)
    : OpenGLMainWindow(parent)
{
    instantiateOpenGLWidget();

//    setFocusPolicy(Qt::StrongFocus);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TestGLWidgetWindow::instantiateOpenGLWidget()
{

#if 1
    setupOpenglWidget(new TestGLSLWidget(this));
#else
    setupOpenglWidget(new TestFixedGLWidget(this));
#endif
}
