//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/26/2017
//            `)  ('                                                    Author: Nghia Truong
//         ,  /::::\  ,
//         |'.\::::/.'|
//        _|  ';::;'  |_
//       (::)   ||   (::)                       _.
//        "|    ||    |"                      _(:)
//         '.   ||   .'                       /::\
//           '._||_.'                         \::/
//            /::::\                         /:::\
//            \::::/                        _\:::/
//             /::::\_.._  _.._  _.._  _.._/::::\
//             \::::/::::\/::::\/::::\/::::\::::/
//               `""`\::::/\::::/\::::/\::::/`""`
//                    `""`  `""`  `""`  `""`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
    FloorRender*      m_FloorRender;
    PointLightRender* m_PointLightRender;

};
