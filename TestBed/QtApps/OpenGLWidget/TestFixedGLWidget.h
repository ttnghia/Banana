//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// TestFixedGLWidget.h
//
// Created on: 8/10/2016
//     Author: Nghia Truong
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef TEST_FIXEDGL_WIDGET_H
#define TEST_FIXEDGL_WIDGET_H

#include <QtGui>
#include <QtWidgets>
#include <Mango/Core/Camera.h>
#include <QOpenGLFunctions_1_0>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class TestFixedGLWidget : public QOpenGLWidget,
    public QOpenGLFunctions_1_0
{
    Q_OBJECT
public:
    explicit TestFixedGLWidget(QWidget* parent = 0);

    // => QOpenGLWidget interface
protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
    Camera camera;

};

#endif // TEST_FIXEDGL_WIDGET_H
