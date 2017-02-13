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

#define TEST_CASE TestCase::TriMeshShadow

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLWidgetTestRender::OpenGLWidgetTestRender(QWidget *parent)
    : OpenGLWidget(parent),
    m_TestCase(TEST_CASE),
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
    glCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))));
    glCall(glEnableVertexAttribArray(0));
    glCall(glEnableVertexAttribArray(1));
    glCall(glBindVertexArray(0));
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
    glCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))));
    glCall(glEnableVertexAttribArray(0));
    glCall(glEnableVertexAttribArray(1));
    glCall(glBindVertexArray(0));


    m_Texture = new OpenGLTexture(GL_TEXTURE_2D);
    QImage texImg = QImage(texFile).convertToFormat(QImage::Format_RGBA8888);
    m_Texture->uploadData(GL_TEXTURE_2D, GL_RGBA, texImg.width(), texImg.height(), GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());

    m_Texture->setBestParametersWithMipMap();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderSkybox(QString texTopFolder)
{
    assert(isValid());
    m_SkyBoxRender = new SkyBoxRender(m_Camera, texTopFolder);
    m_SkyBoxRender->setRenderTextureIndex(1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderFloor(QString texFile)
{
    assert(isValid());
    ////////////////////////////////////////////////////////////////////////////////
   // textures
    m_Texture = new OpenGLTexture(GL_TEXTURE_2D);

    QImage texImg = QImage(texFile).convertToFormat(QImage::Format_RGBA8888);
    m_Texture->uploadData(GL_TEXTURE_2D, GL_RGBA, texImg.width(), texImg.height(), GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());
    m_Texture->setBestParametersWithMipMap();

    ////////////////////////////////////////////////////////////////////////////////
    // camera
    m_Camera->setDefaultCamera(glm::vec3(2, 1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    ////////////////////////////////////////////////////////////////////////////////
    // light
    m_Lights = new PointLights;
    m_Lights->setLightPosition(glm::vec4(0, 2, 0, 1.0));
    m_Lights->uploadDataToGPU();

    ////////////////////////////////////////////////////////////////////////////////
    // floor render
    m_FloorRender = new PlaneRender(m_Camera, m_Lights);
    m_FloorRender->addTexture(m_Texture);
    m_FloorRender->setRenderTextureIndex(1);

    m_PointLightRender = new PointLightRender(m_Camera, m_Lights);
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

    m_Lights = new PointLights;
    m_Lights->setNumLights(2);
    m_Lights->setLightPosition(glm::vec4(0, 1000, 0, 1.0), 0);
    m_Lights->setLightPosition(glm::vec4(1000, 0, 0, 1.0), 1);
    m_Lights->setLightDiffuse(glm::vec4(0.7), 0);
    m_Lights->setLightDiffuse(glm::vec4(0.7), 1);
    m_Lights->uploadDataToGPU();

    m_Material = new Material;
    m_Material->setMaterial(Material::MT_Emerald);
    m_Material->uploadDataToGPU();

    m_PointLightRender = new PointLightRender(m_Camera, m_Lights);
    m_MeshRender = new MeshRender(m_MeshObj, m_Camera, m_Lights, m_Material, m_UBufferCamData);
    m_MeshRender->transform(glm::vec3(0, 0, 0), glm::vec3(0.03));

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderMeshWithShadow(QString meshFile, QString floorTexFile)
{
    assert(isValid());
    ////////////////////////////////////////////////////////////////////////////////
    // textures
    m_Texture = new OpenGLTexture(GL_TEXTURE_2D);

    QImage texImg = QImage(floorTexFile).convertToFormat(QImage::Format_RGBA8888);
    m_Texture->uploadData(GL_TEXTURE_2D, GL_RGBA, texImg.width(), texImg.height(), GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());
    m_Texture->setBestParametersWithMipMap();

    ////////////////////////////////////////////////////////////////////////////////
    // camera
    m_Camera->setDefaultCamera(glm::vec3(2, 1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    ////////////////////////////////////////////////////////////////////////////////
    // light
    m_Lights = new PointLights;
    m_Lights->setNumLights(2);
    m_Lights->setLightPosition(glm::vec4(0, 3, 0, 1.0), 0);
    m_Lights->setLightPosition(glm::vec4(0, 3, 3, 1.0), 1);
    m_Lights->setLightDiffuse(glm::vec4(0.5), 0);
    m_Lights->setLightDiffuse(glm::vec4(0.5), 1);
    m_Lights->setSceneCenter(glm::vec3(0, 0, 0));
    m_Lights->setLightViewPerspective(30);
    m_Lights->uploadDataToGPU();

    ////////////////////////////////////////////////////////////////////////////////
    // floor render
    m_FloorRender = new PlaneRender(m_Camera, m_Lights);
    m_FloorRender->addTexture(m_Texture);
    m_FloorRender->setRenderTextureIndex(1);

    ////////////////////////////////////////////////////////////////////////////////
    m_PointLightRender = new PointLightRender(m_Camera, m_Lights);

    ////////////////////////////////////////////////////////////////////////////////
    m_MeshLoader = new MeshLoader(meshFile.toStdString());
    m_MeshObj = new MeshObject;
    m_MeshObj->setVertices(m_MeshLoader->getVertices());
    m_MeshObj->setVertexNormal(m_MeshLoader->getVertexNormal());


    m_Material = new Material;
    m_Material->setMaterial(Material::MT_Emerald);
    m_Material->uploadDataToGPU();

    m_MeshRender = new MeshRender(m_MeshObj, m_Camera, m_Lights, m_Material, m_UBufferCamData);
    m_MeshRender->transform(glm::vec3(0, 0.5, 0), glm::vec3(0.03));
    m_MeshRender->initShadowMapRenderData(m_ClearColor);
    m_FloorRender->setExternalShadowMaps(m_MeshRender->getAllShadowMaps());

    m_ScreenQuadTexRender = new ScreenQuadTextureRender;
    m_ScreenQuadTexRender->setTexture(m_MeshRender->getShadowMap(0));
    m_ScreenQuadTexRender->setValueScale(-0.10);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initializeGL()
{
    OpenGLWidget::initializeGL();

    ////////////////////////////////////////////////////////////////////////////////
    switch(m_TestCase)
    {
        case TestCase::Triangle:
            initTestRenderTriangle();
            break;
        case TestCase::Texture:
            initTestRenderTexture(QString("D:/Programming/QtApps/RealTimeFluidRendering/Textures/Floor/blue_marble.png"));
            break;
        case TestCase::Floor:
            initTestRenderFloor(QString("D:/Programming/QtApps/RealTimeFluidRendering/Textures/Floor/blue_marble.png"));
            break;
        case TestCase::SkyBox:
            initTestRenderSkybox(QString("D:/Programming/QtApps/RealTimeFluidRendering/Textures/Sky/"));
            break;
        case TestCase::TriMesh:
            initTestRenderMesh(QString("D:/GoogleDrive/DigitalAssets/Models/AirCraft/A-10_Thunderbolt_II/A-10_Thunderbolt_II.obj"));
        case TestCase::TriMeshShadow:
            initTestRenderMeshWithShadow(QString("D:/GoogleDrive/DigitalAssets/Models/AirCraft/A-10_Thunderbolt_II/A-10_Thunderbolt_II.obj"),
                                         QString("D:/Programming/QtApps/RealTimeFluidRendering/Textures/Floor/blue_marble.png"));
            break;
    }
}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::paintGL()
{
    startFrameTimer();
    OpenGLWidget::paintGL();

    ////////////////////////////////////////////////////////////////////////////////
    switch(m_TestCase)
    {
        case TestCase::Triangle:
            renderTriangle();
            break;

        case TestCase::Texture:
            renderTexture();
            break;

        case TestCase::SkyBox:
            renderSkyBox();
            break;

        case TestCase::Floor:
            renderFloor();
            break;

        case TestCase::TriMesh:
            renderMesh();
        case TestCase::TriMeshShadow:
            renderMeshWithShadow();
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
    assert(m_SkyBoxRender != nullptr);
    m_SkyBoxRender->render();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderFloor()
{
    assert(m_Texture != nullptr && m_FloorRender != nullptr);

    m_PointLightRender->render();
    m_FloorRender->render();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderMesh()
{
    assert(m_MeshObj != nullptr && m_MeshRender != nullptr);

    static std::vector<Material::MaterialData> materials = Material::getBuildInMaterials();
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderMeshWithShadow()
{
    assert(m_MeshObj != nullptr && m_MeshRender != nullptr && m_FloorRender != nullptr);

    static std::vector<Material::MaterialData> materials = Material::getBuildInMaterials();
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

    m_MeshRender->renderToDepthBuffer(width(), height(), context()->defaultFramebufferObject());

#if 0
    m_ScreenQuadTexRender->render();
#else
    m_PointLightRender->render();
    m_FloorRender->render();
    m_MeshRender->render();
#endif
}
