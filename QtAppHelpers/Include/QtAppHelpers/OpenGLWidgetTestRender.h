//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <Banana/Geometry/MeshLoader.h>

#include <OpenGLHelpers/OpenGLBuffer.h>
#include <OpenGLHelpers/OpenGLTexture.h>
#include <OpenGLHelpers/ShaderProgram.h>
#include <OpenGLHelpers/Lights.h>
#include <OpenGLHelpers/Material.h>
#include <OpenGLHelpers/RenderObjects.h>
#include <OpenGLHelpers/MeshObjects/CubeObject.h>
#include <OpenGLHelpers/MeshObjects/MeshObject.h>

#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/OpenGLWidget.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
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
using namespace Banana;

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
    virtual void resizeOpenGLWindow(int, int) {}
    virtual void renderOpenGL();

private:
    void renderTriangle();
    void renderTexture();
    void renderSkyBox();
    void renderFloor();
    void renderMesh();
    void renderMeshWithShadow();

    TestCase                                 m_TestCase;
    UniquePtr<OpenGLBuffer>            m_UBufferModelMatrix;
    UniquePtr<OpenGLBuffer>            m_VertexBuffer;
    UniquePtr<OpenGLBuffer>            m_IndexBuffer;
    UniquePtr<MeshLoader>              m_MeshLoader;
    UniquePtr<SkyBoxRender>            m_SkyBoxRender;
    UniquePtr<PlaneRender>             m_FloorRender;
    UniquePtr<PointLightRender>        m_PointLightRender;
    UniquePtr<MeshRender>              m_MeshRender;
    UniquePtr<DepthBufferRender>       m_DepthBufferRender;
    UniquePtr<ScreenQuadTextureRender> m_ScreenQuadTexRender;

    SharedPtr<OpenGLTexture> m_Texture;
    SharedPtr<MeshObject>    m_MeshObj;
    SharedPtr<PointLights>   m_Lights;
    SharedPtr<Material>      m_Material;
    SharedPtr<ShaderProgram> m_Shader;
    GLuint                         m_VAO;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana