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

#include <Banana/ParallelHelpers/ParallelFuncs.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include "RenderWidget.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
RenderWidget::RenderWidget(QWidget* parent, const SharedPtr<VisualizationData>& vizData) : OpenGLWidget(parent), m_VizData(vizData)
{
    m_DefaultSize = QSize(1200, 1000);
    updateCamera();
    setCapturePath(getDefaultCapturePath());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initOpenGL()
{
    initRDataSkyBox();
    initRDataCheckerboardBackground();
    initRDataGridBackground();
    initRDataLight();
    initRDataFloor();
    initRDataBox();
    initRDataParticle();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::resizeOpenGLWindow(int width, int height)
{
    m_RDataParticle.pointScale = static_cast<GLfloat>(height) / tanf(55.0 * 0.5 * M_PI / 180.0);

    if(m_CheckerboardRender != nullptr) {
        m_CheckerboardRender->setScreenSize(width, height);
    }
    if(m_GridRender != nullptr) {
        m_GridRender->setScreenSize(width, height);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderOpenGL()
{
    switch(m_BackgroundMode) {
        case BackgroundMode::SkyBox:
            renderSkyBox();
            break;
        case BackgroundMode::Checkerboard:
            renderCheckerboardBackground();
            break;
        case BackgroundMode::Grid:
            renderGridBackground();
            break;
        case BackgroundMode::Color:
        default:
            ;
    }
    renderLight();
    renderFloor();
    renderBox();
    renderParticles();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateProjection()
{
    if(m_VizData->systemDimension == 3u) {
        m_Camera->setProjection(Camera::OrthographicProjection);
        m_Camera->setOrthoBox(m_VizData->boxMin, m_VizData->boxMax);
    } else {
        m_Camera->setProjection(Camera::PerspectiveProjection);
        updateCamera();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateVizData()
{
    Q_ASSERT(m_RDataParticle.initialized);
    makeCurrent();
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataParticle.buffPosition->uploadDataAsync(m_VizData->positions, 0, m_VizData->nParticles * m_VizData->systemDimension * sizeof(float));

    ////////////////////////////////////////////////////////////////////////////////
    // color data
    if(m_RDataParticle.pColorMode == ParticleColorMode::ObjectIndex ||
       m_RDataParticle.pColorMode == ParticleColorMode::VelocityMagnitude) {
        if(m_RDataParticle.pColorMode == ParticleColorMode::ObjectIndex) {
            m_RDataParticle.buffColorData->uploadDataAsync(m_VizData->objIndex, 0, m_VizData->nParticles);
            m_RDataParticle.vColorMin = 0;
            m_RDataParticle.vColorMax = static_cast<float>(m_VizData->nObjects - 1);
        } else {
            static Vec_Float velMag2;
            velMag2.resize(m_VizData->nParticles);
            if(m_VizData->systemDimension == 2) {
                auto velPtr = reinterpret_cast<Vec2f*>(m_VizData->velocities);
                __BNN_ASSERT(velPtr != nullptr);
                ParallelFuncs::parallel_for(velMag2.size(), [&](size_t i) { velMag2[i] = glm::length2(velPtr[i]); });
            } else {
                auto velPtr = reinterpret_cast<Vec3f*>(m_VizData->velocities);
                __BNN_ASSERT(velPtr != nullptr);
                ParallelFuncs::parallel_for(velMag2.size(), [&](size_t i) { velMag2[i] = glm::length2(velPtr[i]); });
            }
            m_RDataParticle.vColorMin = ParallelSTL::min(velMag2);
            m_RDataParticle.vColorMax = ParallelSTL::max(velMag2);
            m_RDataParticle.buffColorData->uploadDataAsync(velMag2.data(), 0, velMag2.size() * sizeof(float));
        }
    }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    doneCurrent();
    m_RDataParticle.nParticles  = m_VizData->nParticles;
    m_RDataParticle.pointRadius = m_VizData->particleRadius;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::enableClipPlane(bool bEnable)
{
    if(isValid()) {
        makeCurrent();
        if(bEnable) {
            glCall(glEnable(GL_CLIP_PLANE0));
        } else {
            glCall(glDisable(GL_CLIP_PLANE0));
        }
        doneCurrent();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateLights()
{
    if(isValid()) {
        Int nLights = static_cast<Int>(m_VizData->lights.size());
        m_Lights->setNumLights(nLights);
        for(Int i = 0; i < nLights; ++i) {
            m_Lights->setLightPosition(Vec4f(m_VizData->lights[i].position, 1.0f), i);
            m_Lights->setLightAmbient(Vec4f(m_VizData->lights[i].ambient, 1.0f), i);
            m_Lights->setLightDiffuse(Vec4f(m_VizData->lights[i].diffuse, 1.0f), i);
            m_Lights->setLightSpecular(Vec4f(m_VizData->lights[i].specular, 1.0f), i);
        }
        makeCurrent();
        m_Lights->uploadDataToGPU();
        doneCurrent();
        emit lightsObjChanged(m_Lights);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataLight()
{
    m_Lights = std::make_shared<PointLights>();
    m_Lights->setNumLights(2);
    m_Lights->setLightPosition(DEFAULT_LIGHT1_POSITION, 0);
    m_Lights->setLightPosition(DEFAULT_LIGHT2_POSITION, 1);

    m_Lights->setSceneCenter(Vec3f(0, 0, 0));
    m_Lights->setLightViewPerspective(30);
    m_Lights->uploadDataToGPU();

    m_LightRender = std::make_unique<PointLightRender>(m_Camera, m_Lights, m_UBufferCamData);
    emit lightsObjChanged(m_Lights);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataSkyBox()
{
    Q_ASSERT(m_UBufferCamData != nullptr);
    m_SkyBoxRender = std::make_unique<SkyBoxRender>(m_Camera, getTexturePath() + "/Sky/", m_UBufferCamData);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataFloor()
{
    Q_ASSERT(m_UBufferCamData != nullptr && m_Lights != nullptr);

    m_FloorRender = std::make_unique<PlaneRender>(m_Camera, m_Lights, getTexturePath() + "/Floor/", m_UBufferCamData);
    m_FloorRender->transform(Vec3f(0, -1.01, 0), Vec3f(DEFAULT_FLOOR_SIZE));
    m_FloorRender->scaleTexCoord(DEFAULT_FLOOR_SIZE, DEFAULT_FLOOR_SIZE);
    m_FloorRender->setAllowNonTextureRender(false);
    m_FloorRender->setExposure(static_cast<float>(DEFAULT_FLOOR_EXPOSURE) / 100.0f);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataBox()
{
    Q_ASSERT(m_UBufferCamData != nullptr);
    m_DomainBoxRender = std::make_unique<WireFrameBoxRender>(m_Camera, m_UBufferCamData);
    m_DomainBoxRender->setBox(Vec3f(-1), Vec3f(1));
    m_DomainBoxRender->setColor(DEFAULT_BOX_COLOR);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setParticleColorMode(int colorMode)
{
    Q_ASSERT(colorMode < ParticleColorMode::NumColorMode);
    Q_ASSERT(m_RDataParticle.initialized);
    m_RDataParticle.pColorMode = colorMode;
    ////////////////////////////////////////////////////////////////////////////////
    if(colorMode == ParticleColorMode::ObjectIndex ||
       colorMode == ParticleColorMode::VelocityMagnitude) {
        updateVizData();
        makeCurrent();
        initParticleVAO();
        doneCurrent();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataParticle()
{
    m_RDataParticle.shader = std::make_shared<QtAppShaderProgram>("RenderPointSphere");
    m_RDataParticle.shader->addVertexShaderFromResource(":/Shaders/particle.vs.glsl");
    m_RDataParticle.shader->addFragmentShaderFromResource(":/Shaders/particle.fs.glsl");
    m_RDataParticle.shader->link();

    m_RDataParticle.v_Position = m_RDataParticle.shader->getAtributeLocation("v_Position");
    m_RDataParticle.v_iColor   = m_RDataParticle.shader->getAtributeLocation("v_iColor");
    m_RDataParticle.v_fColor   = m_RDataParticle.shader->getAtributeLocation("v_fColor");

    m_RDataParticle.ub_CamData  = m_RDataParticle.shader->getUniformBlockIndex("CameraData");
    m_RDataParticle.ub_Light    = m_RDataParticle.shader->getUniformBlockIndex("Lights");
    m_RDataParticle.ub_Material = m_RDataParticle.shader->getUniformBlockIndex("Material");

    m_RDataParticle.u_nParticles  = m_RDataParticle.shader->getUniformLocation("u_nParticles");
    m_RDataParticle.u_PointRadius = m_RDataParticle.shader->getUniformLocation("u_PointRadius");
    m_RDataParticle.u_PointScale  = m_RDataParticle.shader->getUniformLocation("u_PointScale");
    m_RDataParticle.u_ColorMode   = m_RDataParticle.shader->getUniformLocation("u_ColorMode");
    m_RDataParticle.u_vColorMin   = m_RDataParticle.shader->getUniformLocation("u_vColorMin");
    m_RDataParticle.u_vColorMax   = m_RDataParticle.shader->getUniformLocation("u_vColorMax");
    m_RDataParticle.u_ColorMinVal = m_RDataParticle.shader->getUniformLocation("u_ColorMinVal");
    m_RDataParticle.u_ColorMaxVal = m_RDataParticle.shader->getUniformLocation("u_ColorMaxVal");
    m_RDataParticle.u_ClipPlane   = m_RDataParticle.shader->getUniformLocation("u_ClipPlane");

    m_RDataParticle.buffPosition = std::make_unique<OpenGLBuffer>();
    m_RDataParticle.buffPosition->createBuffer(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);

    m_RDataParticle.buffColorData = std::make_unique<OpenGLBuffer>();
    m_RDataParticle.buffColorData->createBuffer(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);

    m_RDataParticle.material = std::make_unique<Material>();
    m_RDataParticle.material->setMaterial(CUSTOM_PARTICLE_MATERIAL);
    m_RDataParticle.material->uploadDataToGPU();

    ////////////////////////////////////////////////////////////////////////////////
    m_RDataParticle.initialized = true;
    initParticleVAO();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initParticleVAO()
{
    Q_ASSERT(m_RDataParticle.initialized);
    glCall(glGenVertexArrays(1, &m_RDataParticle.VAO));
    glCall(glBindVertexArray(m_RDataParticle.VAO));
    glCall(glEnableVertexAttribArray(m_RDataParticle.v_Position));
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataParticle.buffPosition->bind();
    glCall(glVertexAttribPointer(m_RDataParticle.v_Position, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(0)));
    ////////////////////////////////////////////////////////////////////////////////
    if(m_RDataParticle.pColorMode == ParticleColorMode::ObjectIndex ||
       m_RDataParticle.pColorMode == ParticleColorMode::VelocityMagnitude) {
        m_RDataParticle.buffColorData->bind();
        if(m_RDataParticle.pColorMode == ParticleColorMode::ObjectIndex) {
            glCall(glEnableVertexAttribArray(m_RDataParticle.v_iColor));
            glCall(glVertexAttribIPointer(m_RDataParticle.v_iColor, 1, GL_BYTE, 0, reinterpret_cast<GLvoid*>(0)));
        } else {
            glCall(glEnableVertexAttribArray(m_RDataParticle.v_fColor));
            glCall(glVertexAttribPointer(m_RDataParticle.v_fColor, 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(0)));
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    glCall(glBindVertexArray(0));
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

    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_nParticles, m_RDataParticle.nParticles);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_PointRadius, m_RDataParticle.pointRadius);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_PointScale,  m_RDataParticle.pointScale);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ColorMode, m_RDataParticle.pColorMode);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ClipPlane, m_ClipPlane);

    if(m_RDataParticle.pColorMode == ParticleColorMode::ObjectIndex ||
       m_RDataParticle.pColorMode == ParticleColorMode::VelocityMagnitude) {
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_vColorMin, m_RDataParticle.vColorMin);
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_vColorMax, m_RDataParticle.vColorMax);
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ColorMinVal, m_RDataParticle.colorMinVal);
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ColorMaxVal, m_RDataParticle.colorMaxVal);
    }

    glCall(glBindVertexArray(m_RDataParticle.VAO));
    glCall(glEnable(GL_VERTEX_PROGRAM_POINT_SIZE));
    glCall(glDrawArrays(GL_POINTS, 0, m_RDataParticle.nParticles));
    glCall(glBindVertexArray(0));
    m_RDataParticle.shader->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setParticleMaterial(const Material::MaterialData& material)
{
    makeCurrent();
    m_RDataParticle.material->setMaterial(material);
    m_RDataParticle.material->uploadDataToGPU();
    doneCurrent();
}
