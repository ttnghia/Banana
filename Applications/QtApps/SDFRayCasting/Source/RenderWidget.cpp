//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//        __  __        _        __  ___ ____   __  ___
//       / / / /____ _ (_)_____ /  |/  // __ \ /  |/  /
//      / /_/ // __ `// // ___// /|_/ // /_/ // /|_/ /
//     / __  // /_/ // // /   / /  / // ____// /  / /
//    /_/ /_/ \__,_//_//_/   /_/  /_//_/    /_/  /_/
//
//    This file is part of HairMPM - Material Point Method for Hair Simulation.
//    Created: 2018. All rights reserved.
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <Core/Geometry/MeshLoader.h>
#include <Core/Utils/NumberHelpers.h>
#include <QtAppHelpers/QtAppUtils.h>
#include "RenderWidget.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
RenderWidget::RenderWidget(QWidget* parent) : OpenGLWidget(parent)
{
    m_Camera->setCamera(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_FOCUS, Vec3f(0, 1, 0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initOpenGL()
{
    initRData();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderOpenGL()
{
    renderObjects();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::reloadShader()
{
    Q_ASSERT(m_RData.initialized);
    makeCurrent();
    auto reloadResult = m_RData.shader->reloadShaders();
    doneCurrent();

    if(reloadResult) {
        QMessageBox::information(this, "Info", "Successfully reload shaders!");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setMaterial(const Material::MaterialData& material)
{
    makeCurrent();
    m_RData.material->setMaterial(material);
    m_RData.material->uploadDataToGPU();
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRData()
{
    m_RData.shader = ShaderProgram::getScreenQuadShaderFromFile("Shaders/sdf.frag", "SDFShader");
    ////////////////////////////////////////////////////////////////////////////////
    m_RData.material = std::make_unique<Material>();
    m_RData.material->setMaterial(CUSTOM_MATERIAL);
    m_RData.material->uploadDataToGPU();
    ////////////////////////////////////////////////////////////////////////////////
    m_RData.ub_CamData   = m_RData.shader->getUniformBlockIndex("CameraData");
    m_RData.ub_Light     = m_RData.shader->getUniformBlockIndex("Lights");
    m_RData.ub_Material  = m_RData.shader->getUniformBlockIndex("Material");
    m_RData.u_Resolution = m_RData.shader->getUniformLocation("u_Resolution");
    m_RData.u_RayDir     = m_RData.shader->getUniformLocation("u_RayDir");
    ////////////////////////////////////////////////////////////////////////////////
    m_RData.initialized = true;
    initVAO();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initVAO()
{
    Q_ASSERT(m_RData.initialized);
    glCall(glGenVertexArrays(1, &m_RData.vao));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderObjects()
{
    Q_ASSERT(m_RData.initialized);
    m_RData.shader->bind();
    ////////////////////////////////////////////////////////////////////////////////
    m_UBufferCamData->bindBufferBase();
    m_Lights->bindUniformBuffer();
    m_RData.material->bindUniformBuffer();
    ////////////////////////////////////////////////////////////////////////////////
    m_RData.shader->bindUniformBlock(m_RData.ub_CamData,  m_UBufferCamData->getBindingPoint());
    m_RData.shader->bindUniformBlock(m_RData.ub_Light,    m_Lights->getBufferBindingPoint());
    m_RData.shader->bindUniformBlock(m_RData.ub_Material, m_RData.material->getBufferBindingPoint());
    m_RData.shader->setUniformValue(m_RData.u_Resolution, Vec2f(width(), height()));
    m_RData.shader->setUniformValue(m_RData.u_RayDir,     glm::normalize(m_Camera->getCameraFocus() - m_Camera->getCameraPosition()));
    ////////////////////////////////////////////////////////////////////////////////
    glCall(glBindVertexArray(m_RData.vao));
    glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); );
    glCall(glBindVertexArray(0));
    m_RData.shader->release();
}
