//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//
// Created on: 2/20/2015
//     Author: Nghia Truong
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#ifndef FluidRenderer_H
#define FluidRenderer_H

#include <QtGui>
#include <QtWidgets>
#include <QOpenGLFunctions_4_1_Core>
#include "AvgTimer.h"

#define RENDER_TEXTURE_SIZE 1024
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class FluidRenderer : public QOpenGLWidget, QOpenGLFunctions_4_1_Core
{
    Q_OBJECT
    friend class MainWindow;
public:
    explicit FluidRenderer(QWidget* parent = 0);
    void dump();


    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    int getMethod();
    void setMethod(int _method);
    virtual void keyPressEvent(QKeyEvent* event);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();


private:
    void checkOpenGLVersion();
    void initScene();
    void initScreenQuadMemory();
    void initOfflineRenderingObjects();
    bool initOldMethodProgram();
    void initOldMethodVAO();
    bool initNewMethodProgram();
    void initNewMethodVAO();

    void OldMethod();
    void NewMethod();

    QOpenGLShaderProgram* programOldMethod;
    QOpenGLShaderProgram* programNewMethodFirst;
    QOpenGLShaderProgram* programNewMethodSecond;
    QOpenGLVertexArrayObject vaoOldMethod;
    QOpenGLVertexArrayObject vaoNewMethodFirst;
    QOpenGLVertexArrayObject vaoNewMethodSecond;

    GLint attrVertexOldMethod;
    GLint attrVertexNewMethodFirst;
    GLint attrVertexNewMethodSecond;
    GLint attrTexCoordOldMethod;
    GLint attrTexCoordNewMethodFirst;
    GLint attrTexCoordNewMethodSecond;

    QOpenGLBuffer vboScreenQuad;
    QOpenGLBuffer iboScreenQuad;

    GLuint FBO1TexOutput;
    GLuint FBOTManyTexOutput;
    GLuint texDepthOutput;
    GLuint texDepthInput;
    GLuint tex4Element0;
    GLuint tex4Element1;
    GLuint tex4Element2;

    int numIterations;

    AvgTimer FPSTimer;

    AvgTimer avgOldMethodTimer;
    AvgTimer avgNewMethodTimer;
    AvgTimer avgNewMethodTimerFirst;
    AvgTimer avgNewMethodTimerSecond;

    int method;

};

#endif // FluidRenderer_H
