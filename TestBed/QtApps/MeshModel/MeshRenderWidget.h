//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MeshRenderWidget.h
//
// Created on: 9/5/2016
//     Author: Nghia Truong
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef MESHRENDERWIDGET_H
#define MESHRENDERWIDGET_H

#include <Mango/Core/OpenGLWidget.h>
#include <Mango/Core/MeshModel.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MeshRenderWidget : public OpenGLWidget
{
public:
    explicit MeshRenderWidget(QWidget* parent = 0);

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;

private:
    void loadMesh();
    void renderMesh();

    bool mesh_ready;
    MeshModel* meshModel;
    QOpenGLShaderProgram* shaderProgram;
    GLuint vbo_mesh;
    GLuint vao_mesh;
};

#endif // MESHRENDERWIDGET_H
