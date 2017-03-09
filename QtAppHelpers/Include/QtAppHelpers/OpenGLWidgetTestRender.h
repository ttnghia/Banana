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
#include  <OpenGLHelpers/Lights.h>
#include  <OpenGLHelpers/Material.h>
#include  <OpenGLHelpers/RenderObjects.h>

#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/OpenGLWidget.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
enum class TestCase
{
    Triangle,
    Texture,
    SkyBox,
    Floor,
    TriMesh,
    TriMeshShadow
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OpenGLWidgetTestRender : public OpenGLWidget
{
    Q_OBJECT

public:
    OpenGLWidgetTestRender(QWidget* parent);

    void initTestRenderTriangle();
    void initTestRenderTexture(QString texFile);
    void initTestRenderSkybox(QString texFolder);
    void initTestRenderFloor(QString texFile);
    void initTestRenderMesh(QString meshFile);
    void initTestRenderMeshWithShadow(QString meshFile, QString floorTexFile);

    virtual void initOpenGL();
    virtual void resizeOpenGLWindow(int, int)
    {}
    virtual void renderOpenGL();

private:
    void renderTriangle();
    void renderTexture();
    void renderSkyBox();
    void renderFloor();
    void renderMesh();
    void renderMeshWithShadow();

    TestCase                                 m_TestCase;
    std::unique_ptr<OpenGLBuffer>            m_UBufferModelMatrix;
    std::unique_ptr<OpenGLBuffer>            m_UBufferCamData;
    std::unique_ptr<OpenGLBuffer>            m_VertexBuffer;
    std::unique_ptr<OpenGLBuffer>            m_IndexBuffer;
    std::unique_ptr<OpenGLTexture>           m_Texture;
    std::unique_ptr<MeshObject>              m_MeshObj;
    std::unique_ptr<MeshLoader>              m_MeshLoader;
    std::unique_ptr<PointLights>             m_Lights;
    std::unique_ptr<Material>                m_Material;
    std::unique_ptr<SkyBoxRender>            m_SkyBoxRender;
    std::unique_ptr<PlaneRender>             m_FloorRender;
    std::unique_ptr<PointLightRender>        m_PointLightRender;
    std::unique_ptr<MeshRender>              m_MeshRender;
    std::unique_ptr<DepthBufferRender>       m_DepthBufferRender;
    std::unique_ptr<ScreenQuadTextureRender> m_ScreenQuadTexRender;
    std::shared_ptr<ShaderProgram>           m_Shader;
    GLuint                                   m_VAO;
};
