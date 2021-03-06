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

#include <QtAppHelpers/OpenGLWidgetTestRender.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
#define TEST_CASE TestCase::TriMeshShadow

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLWidgetTestRender::OpenGLWidgetTestRender(QWidget* parent) : OpenGLWidget(parent), m_TestCase(TEST_CASE)
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderTriangle()
{
    assert(isValid());

    ////////////////////////////////////////////////////////////////////////////////
    // setup triangle
    GLfloat triangle[] = {
        0.0f,   0.8f, 0.0f, /* position */
        1.0f,   1.0f, 0.0f, /* color */
        -0.8f, -0.8f, 0.0f, /* position */
        0.0f,   0.0f, 1.0f, /* color */
        0.8f,  -0.8f, 0.0f, /* position */
        1.0f,   0.0f, 0.0f  /* color */
    };

    m_VertexBuffer = std::make_unique<OpenGLBuffer>();
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
    GLfloat triangle[] = {
        0.0f,   0.8f, 0.0f, /* position */
        0.5f,   1.0f,       /* texcoord */
        -0.8f, -0.8f, 0.0f, /* position */
        0.0f,   0.0f,       /* texcoord */
        0.8f,  -0.8f, 0.0f, /* position */
        1.0f, 0.0f          /* texcoord */
    };

    m_VertexBuffer = std::make_unique<OpenGLBuffer>();
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

    m_Texture = std::make_unique<OpenGLTexture>(GL_TEXTURE_2D);
    QImage texImg = QImage(texFile).convertToFormat(QImage::Format_RGBA8888);
    m_Texture->uploadData(GL_TEXTURE_2D, GL_RGBA, texImg.width(), texImg.height(), GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());

    m_Texture->setBestParametersWithMipMap();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderSkybox(QString texTopFolder)
{
    assert(isValid());
    m_SkyBoxRender = std::make_unique<SkyBoxRender>(m_Camera, texTopFolder);
    m_SkyBoxRender->setRenderTextureIndex(1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderFloor(QString texFile)
{
    assert(isValid());
    ////////////////////////////////////////////////////////////////////////////////
    // textures
    m_Texture = std::make_unique<OpenGLTexture>(GL_TEXTURE_2D);

    QImage texImg = QImage(texFile).convertToFormat(QImage::Format_RGBA8888);
    m_Texture->uploadData(GL_TEXTURE_2D, GL_RGBA, texImg.width(), texImg.height(), GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());
    m_Texture->setBestParametersWithMipMap();

    ////////////////////////////////////////////////////////////////////////////////
    // camera
    m_Camera->setCamera(Vec3f(2, 1, 0), Vec3f(0, 0, 0), Vec3f(0, 1, 0));

    ////////////////////////////////////////////////////////////////////////////////
    // light
    m_Lights = std::make_shared<PointLights>();
    m_Lights->setLightPosition(Vec4f(0, 2, 0, 1.0));
    m_Lights->uploadDataToGPU();

    ////////////////////////////////////////////////////////////////////////////////
    // floor render
    m_FloorRender = std::make_unique<PlaneRender>(m_Camera, m_Lights);
    m_FloorRender->addTexture(m_Texture);
    m_FloorRender->setRenderTextureIndex(1);

    m_PointLightRender = std::make_unique<PointLightRender>(m_Camera, m_Lights);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderMesh(QString meshFile)
{
    assert(isValid());

    ////////////////////////////////////////////////////////////////////////////////
    m_MeshLoader = std::make_unique<MeshLoader>(meshFile.toStdString());
    m_MeshObj    = std::make_shared<MeshObject>();
    m_MeshObj->setVertices(m_MeshLoader->getFaceVertices());
    m_MeshObj->setVertexNormal(m_MeshLoader->getFaceVertexNormals());

    m_Lights = std::make_shared<PointLights>();
    m_Lights->setNumLights(2);
    m_Lights->setLightPosition(Vec4f(0, 1000, 0, 1.0), 0);
    m_Lights->setLightPosition(Vec4f(1000, 0, 0, 1.0), 1);
    m_Lights->setLightDiffuse(Vec4f(0.7f), 0);
    m_Lights->setLightDiffuse(Vec4f(0.7f), 1);
    m_Lights->uploadDataToGPU();

    m_Material = std::make_shared<Material>();
    m_Material->setMaterial(Material::MT_Emerald);
    m_Material->uploadDataToGPU();

    m_PointLightRender = std::make_unique<PointLightRender>(m_Camera, m_Lights);
    m_MeshRender       = std::make_unique<MeshRender>(m_MeshObj, m_Camera, m_Lights, m_Material, m_UBufferCamData);
    m_MeshRender->transform(Vec3f(0), Vec3f(0.3f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderMeshWithShadow(QString meshFile, QString floorTexFile)
{
    assert(isValid());
    ////////////////////////////////////////////////////////////////////////////////
    // textures
    m_Texture = std::make_unique<OpenGLTexture>(GL_TEXTURE_2D);

    QImage texImg = QImage(floorTexFile).convertToFormat(QImage::Format_RGBA8888);
    m_Texture->uploadData(GL_TEXTURE_2D, GL_RGBA, texImg.width(), texImg.height(), GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());
    m_Texture->setBestParametersWithMipMap();

    ////////////////////////////////////////////////////////////////////////////////
    // camera
    m_Camera->setCamera(Vec3f(2, 1, 0), Vec3f(0, 0, 0), Vec3f(0, 1, 0));

    ////////////////////////////////////////////////////////////////////////////////
    // light
    m_Lights = std::make_unique<PointLights>();

#define NUM_LIGHTS 1

#if NUM_LIGHTS == 1
    m_Lights->setNumLights(1);
    m_Lights->setLightPosition(Vec4f(0, 10, 3, 1.0), 0);
    m_Lights->setLightDiffuse(Vec4f(1.0), 0);
#endif
#if NUM_LIGHTS == 2
    m_Lights->setNumLights(2);
    m_Lights->setLightPosition(Vec4f(0, 3, 0, 1.0), 0);
    m_Lights->setLightPosition(Vec4f(0, 3, 3, 1.0), 1);
    m_Lights->setLightDiffuse(Vec4f(0.5), 0);
    m_Lights->setLightDiffuse(Vec4f(0.5), 1);
#endif
#if NUM_LIGHTS == 3
    m_Lights->setNumLights(3);
    m_Lights->setLightPosition(Vec4f(0, 3, 0, 1.0),  0);
    m_Lights->setLightPosition(Vec4f(0, 3, 3, 1.0),  1);
    m_Lights->setLightPosition(Vec4f(3, 3, -3, 1.0), 2);
    m_Lights->setLightAmbient(Vec4f(0.05), 0);
    m_Lights->setLightAmbient(Vec4f(0.05), 1);
    m_Lights->setLightAmbient(Vec4f(0.05), 2);
    m_Lights->setLightDiffuse(Vec4f(0.5), 0);
    m_Lights->setLightDiffuse(Vec4f(0.5), 1);
    m_Lights->setLightDiffuse(Vec4f(0.5), 2);
    m_Lights->setLightSpecular(Vec4f(0.5), 0);
    m_Lights->setLightSpecular(Vec4f(0.5), 1);
    m_Lights->setLightSpecular(Vec4f(0.5), 2);
#endif
    m_Lights->setSceneCenter(Vec3f(0, 0, 0));
    m_Lights->setLightViewPerspective(30);
    m_Lights->uploadDataToGPU();

    ////////////////////////////////////////////////////////////////////////////////
    // floor render
    m_FloorRender = std::make_unique<PlaneRender>(m_Camera, m_Lights);
    m_FloorRender->addTexture(m_Texture);
    m_FloorRender->setRenderTextureIndex(1);

    ////////////////////////////////////////////////////////////////////////////////
    m_PointLightRender = std::make_unique<PointLightRender>(m_Camera, m_Lights);

    ////////////////////////////////////////////////////////////////////////////////
    m_MeshLoader = std::make_unique<MeshLoader>(meshFile.toUtf8().constData());
    m_MeshObj    = std::make_shared<MeshObject>();

    m_MeshObj->setVertices(m_MeshLoader->getFaceVertices());
    m_MeshObj->setVertexNormal(m_MeshLoader->getFaceVertexNormals());

    m_Material = std::make_shared<Material>();
    m_Material->setMaterial(Material::MT_Emerald);
    m_Material->uploadDataToGPU();

    m_MeshRender = std::make_unique<MeshRender>(m_MeshObj, m_Camera, m_Lights, m_Material, m_UBufferCamData);
    m_MeshRender->transform(Vec3f(0, 0.5f, 0), Vec3f(0.03f));
    m_MeshRender->initDepthBufferData(m_ClearColor);
    //m_MeshRender->resizeShadowMap(512, 512);
    m_FloorRender->setExternalShadowMaps(m_MeshRender->getAllLightShadowMaps());
    m_FloorRender->transform(Vec3f(0, 0, 0), Vec3f(10));
    m_FloorRender->scaleTexCoord(10, 10);

    m_ScreenQuadTexRender = std::make_unique<ScreenQuadTextureRender>();
    m_ScreenQuadTexRender->setTexture(m_MeshRender->getLightShadowMap(0));
    m_ScreenQuadTexRender->setValueScale(-0.10f);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initOpenGL()
{
    switch(m_TestCase) {
        case TestCase::Triangle:
            initTestRenderTriangle();
            break;
        case TestCase::Texture:
#ifdef _WIN32
            initTestRenderTexture(QString("D:/Programming/QtApps/FluidSimulationAndRendering/Textures/Floor/blue_marble.png"));
#else
            initTestRenderTexture(QString("D:/Programming/QtApps/FluidSimulationAndRendering/Textures/Floor/blue_marble.png"));
#endif
            break;
        case TestCase::Floor:
            initTestRenderFloor(QString("D:/Programming/QtApps/FluidSimulationAndRendering/Textures/Floor/blue_marble.png"));
            break;
        case TestCase::SkyBox:
            initTestRenderSkybox(QString("D:/Programming/QtApps/FluidSimulationAndRendering/Textures/Sky/"));
            break;
        case TestCase::TriMesh:
#ifdef _WIN32
            //initTestRenderMesh(QString("E:/GoogleDrive/DigitalAssets/Models/AirCraft/A-10_Thunderbolt_II/A-10_Thunderbolt_II.obj"));
            initTestRenderMesh(QString("E:/GoogleDrive/Classes/CS6620/prj8/teapot-low.obj"));
#else
            initTestRenderMesh(QString("/Volumes/Working/GoogleDrive/DigitalAssets/Models/AirCraft/A-10_Thunderbolt_II/A-10_Thunderbolt_II.obj"));
#endif
            break;

        case TestCase::TriMeshShadow:
#ifdef _WIN32
            //initTestRenderMeshWithShadow(QString("E:/GoogleDrive/DigitalAssets/Models/AirCraft/A-10_Thunderbolt_II/A-10_Thunderbolt_II.obj"),
            //initTestRenderMeshWithShadow(QString("E:/GoogleDrive/Classes/CS6620/prj8/teapot.obj"),
            //initTestRenderMeshWithShadow(QString("D:/Programming/MerLit/bunny_watertight.obj"),
            initTestRenderMeshWithShadow(QString("D:/Programming/Banana/Assets/PLY/sofa.ply"),
                                         QString("D:/Programming/Banana/Applications/QtApps/OpenGLTest/blue_marble.png"));
#else
            initTestRenderMeshWithShadow(QString("/Volumes/Working/GoogleDrive/DigitalAssets/Models/AirCraft/A-10_Thunderbolt_II/A-10_Thunderbolt_II.obj"),
                                         QString("/Volumes/Working/Programming/QtApps/FluidSimulationAndRendering/Textures/Floor/blue_marble.png"));
#endif
            break;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderOpenGL()
{
    switch(m_TestCase) {
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
            break;

        case TestCase::TriMeshShadow:
            renderMeshWithShadow();
            break;

        default:
            ;
    }
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

    static std::vector<Material::MaterialData> materials            = Material::getBuildInMaterials();
    static int                                 count                = 0;
    static int                                 currentMaterialIndex = 0;

    ++count;
    if(count > 300) {
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

    static std::vector<Material::MaterialData> materials            = Material::getBuildInMaterials();
    static int                                 count                = 0;
    static int                                 currentMaterialIndex = 0;

    ++count;
    if(count > 300) {
        count = 0;

        m_Material->setMaterial(materials[currentMaterialIndex]);
        m_Material->uploadDataToGPU();
        currentMaterialIndex = (currentMaterialIndex + 1) % materials.size();
        qDebug() << "Material: " << QString::fromStdString(m_Material->getName());
    }

    m_MeshRender->renderToLightDepthBuffer(width(), height(), context()->defaultFramebufferObject());

#if 0
    m_ScreenQuadTexRender->render();
#else
    m_PointLightRender->render();
    m_FloorRender->render();
    m_MeshRender->render();
#endif
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana