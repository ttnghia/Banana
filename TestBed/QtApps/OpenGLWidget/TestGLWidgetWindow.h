//------------------------------------------------------------------------------------------
// TestOpenGLWidget.h
//
// Created on: 9/1/2016
//     Author: Nghia Truong
//
//------------------------------------------------------------------------------------------
#ifndef TEST_GLWIDGET_WINDOW_H
#define TEST_GLWIDGET_WINDOW_H

#include <Mango/Core/OpenGLMainWindow.h>
#include <Mango/Core/OpenGLWidget.h>
#include "TestGLSLWidget.h"
#include "TestFixedGLWidget.h"

//------------------------------------------------------------------------------------------

class TestGLWidgetWindow : public OpenGLMainWindow
{
    Q_OBJECT

public:
    TestGLWidgetWindow(QWidget* parent = 0);

private:
    virtual void instantiateOpenGLWidget();

    OpenGLWidget* glWidget;
};

#endif // TEST_GLWIDGET_WINDOW_H
