//------------------------------------------------------------------------------------------
// TestMeshModelWindow.h
//
// Created on: 9/3/2016
//     Author: Nghia Truong
//
//------------------------------------------------------------------------------------------
#ifndef TEST_MESHMODEL_WINDOW_H
#define TEST_MESHMODEL_WINDOW_H

#include <Mango/Core/OpenGLMainWindow.h>

//------------------------------------------------------------------------------------------
class TestMeshModelWindow : public OpenGLMainWindow
{
    Q_OBJECT

public:
    TestMeshModelWindow(QWidget* parent = 0)
        : OpenGLMainWindow(parent)
    {
        instantiateOpenGLWidget();
    }

protected:
    virtual void instantiateOpenGLWidget();

};

#endif // TEST_MESHMODEL_WINDOW_H
