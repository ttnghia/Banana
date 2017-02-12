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

#include <QtAppHelpers/OpenGLWidgetTestRender.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLWidgetTestRender::OpenGLWidgetTestRender(QWidget *parent)
    : OpenGLWidget(parent),
    m_RenderType(RenderType::NotSetup),
    m_UBufferModelMatrix(nullptr),
    m_UBufferCamData(nullptr),
    m_VertexBuffer(nullptr),
    m_IndexBuffer(nullptr),
    m_Texture(nullptr),
    m_Shader(nullptr),
    m_MeshObj(nullptr),
    m_MeshLoader(nullptr)
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLWidgetTestRender::~OpenGLWidgetTestRender()
{
    delete m_UBufferModelMatrix;
    delete m_UBufferCamData;
    delete m_VertexBuffer;
    delete m_IndexBuffer;
    delete m_Texture;
    delete m_Shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderTriangle()
{
    assert(isValid());

    ////////////////////////////////////////////////////////////////////////////////
    // setup triangle
    GLfloat triangle[] ={
        /* position */ 0.0,  0.8, 0.0, /* color */ 1.0, 1.0, 0.0,
        /* position */ -0.8, -0.8, 0.0, /* color */ 0.0, 0.0, 1.0,
        /* position */ 0.8, -0.8, 0.0, /* color */ 1.0, 0.0, 0.0
    };

    m_VertexBuffer = new OpenGLBuffer;
    m_VertexBuffer->createBuffer(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);


    // shaders and VAO
    m_Shader = ShaderProgram::getSimpleVertexColorShader();


    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glBindVertexArray(m_VAO));
    m_VertexBuffer->bind();
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0));
    glCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
        (GLvoid*)(3 * sizeof(GLfloat))));
    glCall(glEnableVertexAttribArray(0));
    glCall(glEnableVertexAttribArray(1));
    glCall(glBindVertexArray(0));

    ////////////////////////////////////////////////////////////////////////////////
    m_RenderType = RenderType::Triangle;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderTexture(QString texFile)
{
    assert(isValid());

    ////////////////////////////////////////////////////////////////////////////////
    // setup square and texture
    GLfloat triangle[] ={
        /* position */ 0.0,  0.8, 0.0, /* texcoord */ 0.5, 1.0,
        /* position */ -0.8, -0.8, 0.0, /* texcoord */ 0.0, 0.0,
        /* position */ 0.8, -0.8, 0.0, /* texcoord */ 1.0, 0.0
    };

    m_VertexBuffer = new OpenGLBuffer;
    m_VertexBuffer->createBuffer(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);


    // shaders and VAO
    m_Shader = ShaderProgram::getSimpleTextureShader();


    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glBindVertexArray(m_VAO));
    m_VertexBuffer->bind();
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0));
    glCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
        (GLvoid*)(3 * sizeof(GLfloat))));
    glCall(glEnableVertexAttribArray(0));
    glCall(glEnableVertexAttribArray(1));
    glCall(glBindVertexArray(0));


    m_Texture = new OpenGLTexture(GL_TEXTURE_2D);
    QImage texImg = QImage(texFile).convertToFormat(QImage::Format_RGBA8888);
    m_Texture->uploadData(GL_TEXTURE_2D,
                          GL_RGBA, texImg.width(), texImg.height(),
                          GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());

    m_Texture->setBestParametersWithMipMap();

    ////////////////////////////////////////////////////////////////////////////////
    m_RenderType = RenderType::Texture;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderSkybox(QString texTopFolder)
{
    m_SkyBoxRender = new SkyBoxRender(m_Camera, texTopFolder);
    m_SkyBoxRender->setRenderTextureIndex(1);

    ////////////////////////////////////////////////////////////////////////////////
    m_RenderType = RenderType::SkyBox;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderFloor(QString texFile)
{
    ////////////////////////////////////////////////////////////////////////////////
   // textures
    m_Texture = new OpenGLTexture(GL_TEXTURE_2D);

    QImage texImg = QImage(texFile).convertToFormat(QImage::Format_RGBA8888);

    m_Texture->uploadData(GL_TEXTURE_2D,
                          GL_RGBA, texImg.width(), texImg.height(),
                          GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());

    m_Texture->setBestParametersWithMipMap();

    ////////////////////////////////////////////////////////////////////////////////
    // camera
    m_Camera->setDefaultCamera(glm::vec3(2, 1, 0), glm::vec3(0, 0, 0),
                               glm::vec3(0, 1, 0));

    ////////////////////////////////////////////////////////////////////////////////
    // light
    m_Light = new PointLights;
    m_Light->setLightPosition(glm::vec4(0, 2, 0, 1.0));
    m_Light->uploadDataToGPU();

    ////////////////////////////////////////////////////////////////////////////////
    // render data
    m_FloorRender = new PlaneRender(m_Camera, m_Light);
    m_FloorRender->addTexture(m_Texture);
    m_FloorRender->setRenderTextureIndex(1);

    m_PointLightRender = new PointLightRender(m_Camera, m_Light);


    ////////////////////////////////////////////////////////////////////////////////
    m_RenderType = RenderType::Floor;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderMesh(QString meshFile)
{
    assert(isValid());

    ////////////////////////////////////////////////////////////////////////////////
    m_MeshLoader = new MeshLoader(meshFile.toStdString());
    m_MeshObj = new MeshObject;
    m_MeshObj->setVertices(m_MeshLoader->getVertices());
    m_MeshObj->setVertexNormal(m_MeshLoader->getVertexNormal());

    m_Light = new PointLights;
    m_Light->setNumLights(2);
    m_Light->setLightPosition(glm::vec4(0, 1000, 0, 1.0), 0);
    m_Light->setLightPosition(glm::vec4(1000, 0, 0, 1.0), 1);
    m_Light->setLightDiffuse(glm::vec4(0.7), 0);
    m_Light->setLightDiffuse(glm::vec4(0.7), 1);
    m_Light->uploadDataToGPU();

    m_Material = new Material;
    m_Material->setMaterial(Material::MT_Emerald);
    m_Material->uploadDataToGPU();

    m_PointLightRender = new PointLightRender(m_Camera, m_Light);
    m_MeshRender = new MeshRender(m_MeshObj, m_Camera, m_Light, m_Material, m_UBufferCamData);
    m_MeshRender->transform(glm::vec3(0, 0, 0), glm::vec3(0.001));

    ////////////////////////////////////////////////////////////////////////////////
    m_RenderType = RenderType::TriMesh;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initializeGL()
{
    OpenGLWidget::initializeGL();

#define TEST 5
#if TEST==0
    initTestRenderTriangle();
#endif
#if TEST==1
    initTestRenderTexture(
        QString("D:/Programming/QtApps/RealTimeFluidRendering/Textures/Floor/blue_marble.png"));
#endif    
#if TEST==2
    initTestRenderFloor(
        QString("D:/Programming/QtApps/RealTimeFluidRendering/Textures/Floor/blue_marble.png"));
#endif
#if TEST==3
    initTestRenderSkybox(
        QString("D:/Programming/QtApps/RealTimeFluidRendering/Textures/Sky/"));
#endif
#if TEST==4
    initTestRenderMesh(QString("D:/GoogleDrive/DigitalAssets/Models/Animal/Bear 1/model_mesh.obj"));
#endif
#if TEST==5
    initTestRenderMesh(QString("D:/GoogleDrive/DigitalAssets/Models/Car/Volkswagen Touareg 2/model/Touareg.obj"));
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::resizeGL(int w, int h)
{
    OpenGLWidget::resizeGL(w, h);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::paintGL()
{
    startFrameTimer();
    OpenGLWidget::paintGL();

    ////////////////////////////////////////////////////////////////////////////////
    switch(m_RenderType)
    {
        case RenderType::Triangle:
            renderTriangle();
            break;

        case RenderType::Texture:
            renderTexture();
            break;

        case RenderType::SkyBox:
            renderSkyBox();
            break;

        case RenderType::Floor:
            renderFloor();
            break;

        case RenderType::TriMesh:
            renderMesh();
            break;

        default:
            ;
    }
    ////////////////////////////////////////////////////////////////////////////////

    endFrameTimer();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderTriangle()
{
    m_Shader->bind();
    glCall(glBindVertexArray(m_VAO));
    glCall(glDrawArrays(GL_TRIANGLES, 0, 3));
    m_Shader->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderTexture()
{
    assert(m_Texture != nullptr);

    m_Shader->bind();
    m_Texture->bind();

    m_Shader->setUniformValue<GLint>("u_TexSampler", 0);
    glCall(glBindVertexArray(m_VAO));
    glCall(glDrawArrays(GL_TRIANGLES, 0, 3));
    m_Texture->release();
    m_Shader->release();

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderSkyBox()
{
    m_SkyBoxRender->render();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderFloor()
{
    assert(m_Texture != nullptr);

    m_PointLightRender->render();
    m_FloorRender->render();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderMesh()
{
    assert(m_MeshObj != nullptr);

    static std::vector<Material::MaterialData> materials =
        Material::getBuildInMaterials();
    static int count = 0;
    static int currentMaterialIndex = 0;

    ++count;
    if(count > 300)
    {
        count = 0;

        m_Material->setMaterial(materials[currentMaterialIndex]);
        m_Material->uploadDataToGPU();
        currentMaterialIndex = (currentMaterialIndex + 1) % materials.size();
        qDebug() << "Material: " << QString::fromStdString(m_Material->getName());
    }

    m_PointLightRender->render();
    m_MeshRender->render();
}