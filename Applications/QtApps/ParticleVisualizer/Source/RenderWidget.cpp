//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
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

#include "RenderWidget.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
RenderWidget::RenderWidget(QWidget* parent) : OpenGLWidget(parent)
{
//    m_ClearColor = DEFAULT_CLEAR_COLOR;
    setCamera(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_FOCUS);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setCamera(const Vec3f& cameraPosition, const Vec3f& cameraFocus)
{
    m_Camera->setDefaultCamera(cameraPosition, cameraFocus, Vec3f(0, 1, 0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setBox(const Vec3f& boxMin, const Vec3f& boxMax)
{
    makeCurrent();
    m_WireFrameBoxRender->setBox(boxMin, boxMax);
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initOpenGL()
{
    initRDataSkyBox();
    initRDataLight();
    initRDataFloor();
    initRDataBox();
    initRDataParticle();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderOpenGL()
{
    renderSkyBox();
    renderLight();
    renderFloor();
    renderBox();
    renderParticles();
//            renderMeshes();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateSimMeshes()
{
    makeCurrent();

    for(auto& mesh : m_SimMeshObjs) {
        mesh->uploadDataToGPU();
    }

    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateBoundaryMeshes()
{
    makeCurrent();

    for(auto& mesh : m_BoundaryMeshObjs) {
        mesh->uploadDataToGPU();
    }

    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateNumSimMeshes(int numMeshes)
{
    m_NumSimMeshes = numMeshes;
    makeCurrent();

    for(auto& meshRender : m_SimMeshRenders) {
        meshRender->setupVAO();
        meshRender->initDepthBufferData(m_ClearColor);
    }

    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateNumBoundaryMeshes(int numMeshes)
{
    m_NumBoundaryMeshes = numMeshes;
    makeCurrent();
    for(auto& meshRender : m_BoundaryMeshRenders) {
        meshRender->setupVAO();
        meshRender->initDepthBufferData(m_ClearColor);
    }
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateLights()
{
    makeCurrent();
    m_Lights->uploadDataToGPU();
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataLight()
{
    m_Lights = std::make_shared<PointLights>();
    m_Lights->setNumLights(1);

    m_Lights->setLightPosition(glm::vec4(-10, 10, 10, 1.0), 0);
    //    m_Lights->setLightPosition(glm::vec4(0, 100, 100, 1.0), 1);
    //    m_Lights->setLightDiffuse(glm::vec4(1.0), 0);
    //    m_Lights->setLightDiffuse(glm::vec4(0.7), 1);

    m_Lights->setSceneCenter(Vec3f(0, 0, 0));
    m_Lights->setLightViewPerspective(30);
    m_Lights->uploadDataToGPU();

    m_LightRender = std::make_unique<PointLightRender>(m_Camera, m_Lights, m_UBufferCamData);
    emit lightsObjChanged(m_Lights);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderLight()
{
    Q_ASSERT(m_LightRender != nullptr);
    m_LightRender->render();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setSkyBoxTexture(int texIndex)
{
    Q_ASSERT(m_SkyBoxRender != nullptr);
    m_SkyBoxRender->setRenderTextureIndex(texIndex);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataSkyBox()
{
    Q_ASSERT(m_UBufferCamData != nullptr);

    m_SkyBoxRender = std::make_unique<SkyBoxRender>(m_Camera, QDir::currentPath() + "/Textures/Sky/", m_UBufferCamData);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderSkyBox()
{
    Q_ASSERT(m_SkyBoxRender != nullptr);
    m_SkyBoxRender->render();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setFloorTexture(int texIndex)
{
    Q_ASSERT(m_PlaneRender != nullptr);
    m_PlaneRender->setRenderTextureIndex(texIndex);
}

void RenderWidget::setFloorSize(int size)
{
    m_PlaneRender->transform(Vec3f(0), Vec3f(static_cast<float>(size)));
    m_PlaneRender->scaleTexCoord(static_cast<float>(size), static_cast<float>(size));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataFloor()
{
    Q_ASSERT(m_UBufferCamData != nullptr && m_Lights != nullptr);

    m_PlaneRender = std::make_unique<FRPlaneRender>(m_Camera, m_Lights, QDir::currentPath() + "/Textures/Floor/", m_UBufferCamData);
    m_PlaneRender->transform(Vec3f(0, -0.01, 0), Vec3f(10));
    m_PlaneRender->scaleTexCoord(2, 2);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderFloor()
{
    Q_ASSERT(m_PlaneRender != nullptr);
    m_PlaneRender->render(m_bShadowEnabled, m_bVisualizeShadow, m_ShadowIntensity);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataBox()
{
    Q_ASSERT(m_UBufferCamData != nullptr);
    m_WireFrameBoxRender = std::make_unique<WireFrameBoxRender>(m_Camera, m_UBufferCamData);
    m_WireFrameBoxRender->setBox(Vec3f(-1, 0, -1), Vec3f(1, 2, 1));
    m_WireFrameBoxRender->setColor(Vec3f(0, 1, 0.5));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderBox()
{
    Q_ASSERT(m_WireFrameBoxRender != nullptr);
    m_WireFrameBoxRender->render();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setParticleColorMode(int colorMode)
{
    Q_ASSERT(colorMode < ParticleColorMode::NumColorMode);
    Q_ASSERT(m_RDataParticle.initialized);

    m_RDataParticle.hasVColor  = colorMode != ParticleColorMode::Uniform ? 1 : 0;
    m_RDataParticle.pColorMode = colorMode;

    ////////////////////////////////////////////////////////////////////////////////
    makeCurrent();
    uploadParticleColorData();
    initFluidVAOs();
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateParticleData()
{
    Q_ASSERT(m_RDataParticle.initialized);
    makeCurrent();

    ////////////////////////////////////////////////////////////////////////////////
    // position buffer
    m_RDataParticle.buffPosition->uploadDataAsync(m_ParticleData->getArray("Position")->data(), 0, m_ParticleData->getArray("Position")->size());

    if(m_RDataParticle.useAnisotropyKernel) {
        m_RDataParticle.buffAnisotropyMatrix->uploadDataAsync(m_ParticleData->getArray("AniKernel")->data(), 0, m_ParticleData->getArray("AniKernel")->size());
    }

    ////////////////////////////////////////////////////////////////////////////////
    // color from data buffer
    if(m_RDataParticle.pColorMode == ParticleColorMode::FromData) {
        Q_ASSERT(m_ParticleData->hasArray("ColorData"));
        m_RDataParticle.buffColorData->uploadDataAsync(m_ParticleData->getArray("ColorData")->data(), 0, m_ParticleData->getArray("ColorData")->size());
    }

    ////////////////////////////////////////////////////////////////////////////////
    // also upload the particle color data
    uploadParticleColorData();

    ////////////////////////////////////////////////////////////////////////////////
    doneCurrent();
    m_RDataParticle.pointRadius  = m_ParticleData->getParticleRadius<GLfloat>();
    m_RDataParticle.numParticles = m_ParticleData->getNParticles();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataParticle()
{
    m_RDataParticle.shader = std::make_shared<ShaderProgram>("Shaders/particle.vs.glsl", "Shaders/particle.fs.glsl", "RenderPointSphere");
    m_ExternalShaders.push_back(m_RDataParticle.shader);

    m_RDataParticle.v_Position          = m_RDataParticle.shader->getAtributeLocation("v_Position");
    m_RDataParticle.v_AnisotropyMatrix0 = m_RDataParticle.shader->getAtributeLocation("v_AnisotropyMatrix0");
    m_RDataParticle.v_AnisotropyMatrix1 = m_RDataParticle.shader->getAtributeLocation("v_AnisotropyMatrix1");
    m_RDataParticle.v_AnisotropyMatrix2 = m_RDataParticle.shader->getAtributeLocation("v_AnisotropyMatrix2");
    m_RDataParticle.v_Color             = m_RDataParticle.shader->getAtributeLocation("v_Color");
    m_RDataParticle.u_ClipPlane         = m_RDataParticle.shader->getUniformLocation("u_ClipPlane");

    m_RDataParticle.ub_CamData  = m_RDataParticle.shader->getUniformBlockIndex("CameraData");
    m_RDataParticle.ub_Light    = m_RDataParticle.shader->getUniformBlockIndex("Lights");
    m_RDataParticle.ub_Material = m_RDataParticle.shader->getUniformBlockIndex("Material");

    m_RDataParticle.u_PointRadius = m_RDataParticle.shader->getUniformLocation("u_PointRadius");
    //    m_RDataParticle.u_PointScale = m_RDataParticle.shader->getUniformLocation("u_PointScale");
    m_RDataParticle.u_IsPointView         = m_RDataParticle.shader->getUniformLocation("u_IsPointView");
    m_RDataParticle.u_HasVColor           = m_RDataParticle.shader->getUniformLocation("u_HasVColor");
    m_RDataParticle.u_UseAnisotropyKernel = m_RDataParticle.shader->getUniformLocation("u_UseAnisotropyKernel");
    m_RDataParticle.u_ScreenWidth         = m_RDataParticle.shader->getUniformLocation("u_ScreenWidth");
    m_RDataParticle.u_ScreenHeight        = m_RDataParticle.shader->getUniformLocation("u_ScreenHeight");

    m_RDataParticle.buffPosition = std::make_unique<OpenGLBuffer>();
    m_RDataParticle.buffPosition->createBuffer(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);

    m_RDataParticle.buffAnisotropyMatrix = std::make_unique<OpenGLBuffer>();
    m_RDataParticle.buffAnisotropyMatrix->createBuffer(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);

    m_RDataParticle.buffColorData = std::make_unique<OpenGLBuffer>();
    m_RDataParticle.buffColorData->createBuffer(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);

    m_RDataParticle.material = std::make_unique<Material>();
    m_RDataParticle.material->setMaterial(CUSTOM_PARTICLE_MATERIAL);
    m_RDataParticle.material->uploadDataToGPU();

    glCall(glGenVertexArrays(1, &m_RDataParticle.VAO));

    m_RDataParticle.initialized = true;
    initFluidVAOs();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initFluidVAOs()
{
    Q_ASSERT(m_RDataParticle.initialized);
    glCall(glBindVertexArray(m_RDataParticle.VAO));
    glCall(glEnableVertexAttribArray(m_RDataParticle.v_Position));

    m_RDataParticle.buffPosition->bind();
    glCall(glVertexAttribPointer(m_RDataParticle.v_Position, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(0)));

    m_RDataParticle.buffAnisotropyMatrix->bind();
    glCall(glEnableVertexAttribArray(m_RDataParticle.v_AnisotropyMatrix0));
    glCall(glEnableVertexAttribArray(m_RDataParticle.v_AnisotropyMatrix1));
    glCall(glEnableVertexAttribArray(m_RDataParticle.v_AnisotropyMatrix2));
    glCall(glVertexAttribPointer(m_RDataParticle.v_AnisotropyMatrix0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0)));
    glCall(glVertexAttribPointer(m_RDataParticle.v_AnisotropyMatrix1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(sizeof(GLfloat) * 3)));
    glCall(glVertexAttribPointer(m_RDataParticle.v_AnisotropyMatrix2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(sizeof(GLfloat) * 6)));

    if(m_RDataParticle.pColorMode != ParticleColorMode::Uniform) {
        glCall(glEnableVertexAttribArray(m_RDataParticle.v_Color));
        m_RDataParticle.buffColorData->bind();
        glCall(glVertexAttribPointer(m_RDataParticle.v_Color, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(0)));
    }

    glCall(glBindVertexArray(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::uploadParticleColorData()
{
    m_RDataParticle.buffColorData->uploadDataAsync(m_ParticleData->getArray("ColorRandom")->data(), 0, m_ParticleData->getArray("ColorRandom")->size());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderParticles()
{
    Q_ASSERT(m_RDataParticle.initialized);

    m_RDataParticle.shader->bind();

    m_UBufferCamData->bindBufferBase();
    m_RDataParticle.shader->bindUniformBlock(m_RDataParticle.ub_CamData, m_UBufferCamData->getBindingPoint());

    m_Lights->bindUniformBuffer();
    m_RDataParticle.shader->bindUniformBlock(m_RDataParticle.ub_Light, m_Lights->getBufferBindingPoint());

    m_RDataParticle.material->bindUniformBuffer();
    m_RDataParticle.shader->bindUniformBlock(m_RDataParticle.ub_Material, m_RDataParticle.material->getBufferBindingPoint());

    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_PointRadius, m_RDataParticle.pointRadius);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_IsPointView, m_RDataParticle.isPointView);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_HasVColor, m_RDataParticle.hasVColor);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ClipPlane, m_ClipPlane);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ScreenWidth, width());
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ScreenHeight, height());

    if(m_RDataParticle.useAnisotropyKernel && m_ParticleData->getUInt("AniKernelReady") == 1) {
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_UseAnisotropyKernel, 1);
    } else {
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_UseAnisotropyKernel, 0);
    }

    glCall(glBindVertexArray(m_RDataParticle.VAO));
    glCall(glEnable(GL_VERTEX_PROGRAM_POINT_SIZE));
    glCall(glDrawArrays(GL_POINTS, 0, m_RDataParticle.numParticles));
    glCall(glBindVertexArray(0));
    m_RDataParticle.shader->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::reloadTextures()
{
    makeCurrent();
    m_SkyBoxRender->clearTextures();
    m_SkyBoxRender->loadTextures(QDir::currentPath() + "/Textures/Sky/");

    m_PlaneRender->clearTextures();
    m_PlaneRender->loadTextures(QDir::currentPath() + "/Textures/Floor/");
    doneCurrent();

    ////////////////////////////////////////////////////////////////////////////////
    QMessageBox::information(this, "Info", "Textures reloaded!");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setParticleMaterial(const Material::MaterialData& material)
{
    makeCurrent();
    m_RDataParticle.material->setMaterial(material);
    m_RDataParticle.material->uploadDataToGPU();
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setMeshMaterial(const Material::MaterialData& material, int meshID)
{
    makeCurrent();
    m_SimMeshRenders[meshID]->getMaterial()->setMaterial(material);
    m_SimMeshRenders[meshID]->getMaterial()->uploadDataToGPU();
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::enableAnisotropyKernel(bool bAniKernel)
{
    m_RDataParticle.useAnisotropyKernel = bAniKernel ? 1 : 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::enableShadow(bool bShadowEnabled)
{
    m_bShadowEnabled = bShadowEnabled;
}

void RenderWidget::visualizeShadowRegion(bool bVisualizeShadow)
{
    m_bVisualizeShadow = bVisualizeShadow;
}

void RenderWidget::setShadowIntensity(int intensity)
{
    m_ShadowIntensity = static_cast<float>(intensity) / 100.0f;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::enableClipPlane(bool bEnable)
{
    if(!isValid()) {
        return;
    }

    makeCurrent();

    if(bEnable) {
        glCall(glEnable(GL_CLIP_PLANE0));
    } else {
        glCall(glDisable(GL_CLIP_PLANE0));
    }

    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setClipPlane(const glm::vec4& clipPlane)
{
    m_ClipPlane = clipPlane;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::enableExportFrame(bool bEnable)
{
    m_bExrportFrameToImage = bEnable;
}
