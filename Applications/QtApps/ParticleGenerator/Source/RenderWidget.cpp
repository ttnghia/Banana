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

#include "RenderWidget.h"
#include <tbb/tbb.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
RenderWidget::RenderWidget(QWidget* parent) : OpenGLWidget(parent)
{
    m_DefaultSize = QSize(1200, 1000);
    setCamera(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_FOCUS);
    initParticleDataObj();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const std::shared_ptr<ParticleSystemData>& RenderWidget::getParticleDataObj() const
{
    return m_ParticleData;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setCamera(const Vec3f& cameraPosition, const Vec3f& cameraFocus)
{
    m_Camera->setCamera(cameraPosition, cameraFocus, Vec3f(0, 1, 0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initOpenGL()
{
    return;
    initRDataSkyBox();
    initRDataLight();
    initRDataFloor();
    initRDataBox();
    initRDataParticle();

    updateParticleData();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::resizeOpenGLWindow(int, int h)
{
    m_RDataParticle.pointScale = static_cast<GLfloat>(h) / tanf(55.0 * 0.5 * M_PI / 180.0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderOpenGL()
{
    return;
    renderParticles();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateParticleData()
{
    return;
    Q_ASSERT(m_RDataParticle.initialized);
    makeCurrent();

    ////////////////////////////////////////////////////////////////////////////////
    // position buffer
    m_RDataParticle.buffPosition->uploadDataAsync(m_ParticleData->getArray("Position")->data(), 0, m_ParticleData->getArray("Position")->size());
    m_RDataParticle.buffColorScale->uploadDataAsync(m_ParticleData->getArray("ColorScale")->data(), 0, m_ParticleData->getArray("ColorScale")->size());

    ////////////////////////////////////////////////////////////////////////////////
    doneCurrent();
    m_RDataParticle.negativePointRadius  = m_ParticleData->getParticleRadius<GLfloat>() * m_NegativeParticleSizeScale;
    m_RDataParticle.positivePointRadius  = m_ParticleData->getParticleRadius<GLfloat>() * m_PositveParticleSizeScale;
    m_RDataParticle.numNegativeParticles = m_ParticleData->getUInt("NumNegative");
    m_RDataParticle.numPositiveParticles = m_ParticleData->getUInt("NumPositive");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataParticle()
{
    m_RDataParticle.shader = std::make_shared<QtAppShaderProgram>("RenderPointSphere");
    m_RDataParticle.shader->addVertexShaderFromResource(":/Shaders/particle.vs.glsl");
    m_RDataParticle.shader->addFragmentShaderFromResource(":/Shaders/particle.fs.glsl");
    m_RDataParticle.shader->link();

    m_RDataParticle.v_Position   = m_RDataParticle.shader->getAtributeLocation("v_Position");
    m_RDataParticle.v_ColorScale = m_RDataParticle.shader->getAtributeLocation("v_ColorScale");

    m_RDataParticle.ub_CamData  = m_RDataParticle.shader->getUniformBlockIndex("CameraData");
    m_RDataParticle.ub_Light    = m_RDataParticle.shader->getUniformBlockIndex("Lights");
    m_RDataParticle.ub_Material = m_RDataParticle.shader->getUniformBlockIndex("Material");

    m_RDataParticle.u_PointRadius = m_RDataParticle.shader->getUniformLocation("u_PointRadius");
    m_RDataParticle.u_PointScale  = m_RDataParticle.shader->getUniformLocation("u_PointScale");

    m_RDataParticle.u_ClipPlane = m_RDataParticle.shader->getUniformLocation("u_ClipPlane");

    m_RDataParticle.buffPosition = std::make_unique<OpenGLBuffer>();
    m_RDataParticle.buffPosition->createBuffer(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);

    m_RDataParticle.buffColorScale = std::make_unique<OpenGLBuffer>();
    m_RDataParticle.buffColorScale->createBuffer(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);

    m_RDataParticle.negativeParticleMaterial = std::make_unique<Material>();
    m_RDataParticle.negativeParticleMaterial->setMaterial(CUSTOM_PARTICLE_MATERIAL_INSIDE);
    m_RDataParticle.negativeParticleMaterial->uploadDataToGPU();

    m_RDataParticle.positiveParticleMaterial = std::make_unique<Material>();
    m_RDataParticle.positiveParticleMaterial->setMaterial(CUSTOM_PARTICLE_MATERIAL_OUTSIDE);
    m_RDataParticle.positiveParticleMaterial->uploadDataToGPU();

    glCall(glGenVertexArrays(1, &m_RDataParticle.VAO));

    m_RDataParticle.initialized = true;
    initParticleVAOs();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initParticleVAOs()
{
    Q_ASSERT(m_RDataParticle.initialized);
    glCall(glBindVertexArray(m_RDataParticle.VAO));

    glCall(glEnableVertexAttribArray(m_RDataParticle.v_Position));
    m_RDataParticle.buffPosition->bind();
    glCall(glVertexAttribPointer(m_RDataParticle.v_Position, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(0)));

    glCall(glEnableVertexAttribArray(m_RDataParticle.v_ColorScale));
    m_RDataParticle.buffColorScale->bind();
    glCall(glVertexAttribPointer(m_RDataParticle.v_ColorScale, 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(0)));

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

    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_PointScale, m_RDataParticle.pointScale);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ClipPlane,  m_ClipPlane);

    glCall(glBindVertexArray(m_RDataParticle.VAO));
    glCall(glEnable(GL_VERTEX_PROGRAM_POINT_SIZE));

    if(!m_RDataParticle.hideNegativeParticles) {
        m_RDataParticle.negativeParticleMaterial->bindUniformBuffer();
        m_RDataParticle.shader->bindUniformBlock(m_RDataParticle.ub_Material, m_RDataParticle.negativeParticleMaterial->getBufferBindingPoint());
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_PointRadius, m_RDataParticle.negativePointRadius);
        glCall(glDrawArrays(GL_POINTS, 0, m_RDataParticle.numNegativeParticles));
    }

    if(!m_RDataParticle.hidePositveParticles) {
        m_RDataParticle.positiveParticleMaterial->bindUniformBuffer();
        m_RDataParticle.shader->bindUniformBlock(m_RDataParticle.ub_Material, m_RDataParticle.positiveParticleMaterial->getBufferBindingPoint());
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_PointRadius, m_RDataParticle.positivePointRadius);
        glCall(glDrawArrays(GL_POINTS, m_RDataParticle.numNegativeParticles, m_RDataParticle.numPositiveParticles));
    }
    glCall(glBindVertexArray(0));
    m_RDataParticle.shader->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initParticleDataObj()
{
    Q_ASSERT(m_ParticleData != nullptr);
    m_ParticleData->addArray<GLfloat, 3>("Position");

#if 1
    const int     sizeXYZ = 20;
    const GLfloat step    = 2.0 / static_cast<GLfloat>(sizeXYZ - 1);
    m_ParticleData->setNumParticles(sizeXYZ * sizeXYZ * sizeXYZ);
    m_ParticleData->setParticleRadius(0.5 * step);

    GLfloat*      dataPtr    = reinterpret_cast<GLfloat*>(m_ParticleData->getArray("Position")->data());
    int           p          = 0;
    const GLfloat randomness = 0.0;

    for(int i = 0; i < sizeXYZ; ++i) {
        for(int j = 0; j < sizeXYZ; ++j) {
            for(int k = 0; k < sizeXYZ; ++k) {
                dataPtr[p++] = -1.0 + static_cast<GLfloat>(i) * step + ((float)rand() / RAND_MAX * 2 - 1) * m_ParticleData->getParticleRadius<float>() * randomness;
                dataPtr[p++] = -1.0 + static_cast<GLfloat>(j) * step + ((float)rand() / RAND_MAX * 2 - 1) * m_ParticleData->getParticleRadius<float>() * randomness;
                dataPtr[p++] = -1.0 + static_cast<GLfloat>(k) * step + ((float)rand() / RAND_MAX * 2 - 1) * m_ParticleData->getParticleRadius<float>() * randomness;
            }
        }
    }
#endif

    unsigned int numTotal    = sizeXYZ * sizeXYZ * sizeXYZ;
    unsigned int numNegative = sizeXYZ * sizeXYZ * sizeXYZ / 2;
    unsigned int numPositive = numTotal - numNegative;
    m_ParticleData->setUInt("NumNegative", numNegative);
    m_ParticleData->setUInt("NumPositive", numPositive);
    m_ParticleData->addArray<GLfloat, 1>("ColorScale", true, 1.0f);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setNegativeParticleMaterial(const Material::MaterialData& material)
{
    makeCurrent();
    m_RDataParticle.negativeParticleMaterial->setMaterial(material);
    m_RDataParticle.negativeParticleMaterial->uploadDataToGPU();
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setPositiveParticleMaterial(const Material::MaterialData& material)
{
    makeCurrent();
    m_RDataParticle.positiveParticleMaterial->setMaterial(material);
    m_RDataParticle.positiveParticleMaterial->uploadDataToGPU();
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setNegativeParticleSize(int sizeScale)
{
    m_NegativeParticleSizeScale         = static_cast<GLfloat>(sizeScale) / 100.0f;
    m_RDataParticle.negativePointRadius = m_ParticleData->getParticleRadius<GLfloat>() * m_NegativeParticleSizeScale;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setPositiveParticleSize(int sizeScale)
{
    m_PositveParticleSizeScale          = static_cast<GLfloat>(sizeScale) / 100.0f;
    m_RDataParticle.positivePointRadius = m_ParticleData->getParticleRadius<GLfloat>() * m_PositveParticleSizeScale;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::hideNegativeParticles(bool bHide)
{
    m_RDataParticle.hideNegativeParticles = bHide;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::hidePositiveParticles(bool bHide)
{
    m_RDataParticle.hidePositveParticles = bHide;
}