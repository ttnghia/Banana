//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____                              
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _ 
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/ 
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include  <OpenGLHelpers/OpenGLBuffer.h>
#include  <OpenGLHelpers/OpenGLTexture.h>
#include  <OpenGLHelpers/ShaderProgram.h>
#include  <OpenGLHelpers/CubeObject.h>
#include  <OpenGLHelpers/MeshObject.h>
#include  <OpenGLHelpers/MeshLoader.h>
#include  <OpenGLHelpers/Light.h>
#include  <OpenGLHelpers/Material.h>
#include  <OpenGLHelpers/RenderObjects.h>

#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/OpenGLWidget.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OpenGLWidgetTestRender : public OpenGLWidget
{
    Q_OBJECT

public:
    enum class RenderType
    {
        NotSetup,
        Triangle,
        Texture,
        SkyBox,
        Floor,
        TriMesh
    };

    OpenGLWidgetTestRender(QWidget *parent);
    ~OpenGLWidgetTestRender();

    void initTestRenderTriangle();
    void initTestRenderTexture(QString texFile);
    void initTestRenderSkybox(QString texFolder);
    void initTestRenderFloor(QString texFile);
    void initTestRenderMesh(QString meshFile);

    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

private:
    void renderTriangle();
    void renderTexture();
    void renderSkyBox();
    void renderFloor();
    void renderMesh();

    RenderType        m_RenderType;
    OpenGLBuffer*     m_UBufferModelMatrix;
    OpenGLBuffer*     m_UBufferCamData;
    OpenGLBuffer*     m_VertexBuffer;
    OpenGLBuffer*     m_IndexBuffer;
    OpenGLTexture*    m_Texture;
    ShaderProgram*    m_Shader;
    MeshObject*       m_MeshObj;
    MeshLoader*       m_MeshLoader;
    PointLight*       m_Light;
    Material*         m_Material;
    GLuint            m_VAO;
    SkyBoxRender*     m_SkyBoxRender;
    PlaneRender*      m_FloorRender;
    PointLightRender* m_PointLightRender;
    MeshRender*       m_MeshRender;

};
