//------------------------------------------------------------------------------------------
// TestGLSLWidget.h
//
// Created on: 8/10/2016
//     Author: Nghia Truong
//
//------------------------------------------------------------------------------------------
#ifndef TEST_GLSL_WIDGET_H
#define TEST_GLSL_WIDGET_H

#include <Mango/Core/OpenGLWidget.h>
//------------------------------------------------------------------------------------------
class TestGLSLWidget : public OpenGLWidget
{
    Q_OBJECT
public:
    explicit TestGLSLWidget(QWidget* parent = 0);

    // => QOpenGLWidget interface
protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;

    // => private members
private:
    void initTestScene();
    void renderTestScene();

    bool initializedTestScene;
    QOpenGLShaderProgram* testProgram;
    GLuint vbo_triangle;
    GLuint vao;

};

#endif // TEST_GLSL_WIDGET_H
