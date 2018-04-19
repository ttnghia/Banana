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
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <Banana/Geometry/MeshLoader.h>
#include <Banana/Utils/NumberHelpers.h>
#include "RenderWidget.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
RenderWidget::RenderWidget(QWidget* parent, const SharedPtr<HairModel>& hairModel) : OpenGLWidget(parent), m_HairModel(hairModel)
{
    m_Camera->setCamera(DEFAULT_CAMERA_POSITION, DEFAULT_CAMERA_FOCUS, Vec3f(0, 1, 0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initOpenGL()
{
    initRDataMesh();
    initRDataHair();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::resizeOpenGLWindow(int, int height)
{
    m_RDataHair.pointScale = static_cast<GLfloat>(height) / tanf(55.0 * 0.5 * M_PI / 180.0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderOpenGL()
{
    renderMesh();
    renderHair();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateModelDimension()
{
    if(m_HairModel->getDimension() == 3) {
        m_Camera->setProjection(Camera::PerspectiveProjection);
    } else {
        m_Camera->setProjection(Camera::OrthographicProjection);
        m_Camera->setOrthoBox(m_HairModel->getBMin().x * 1.01f, m_HairModel->getBMax().x * 1.01f,
                              m_HairModel->getBMin().y * 1.01f, m_HairModel->getBMax().y * 1.01f);
    }

    makeCurrent();
    initHairVAO();
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateVizData()
{
    Q_ASSERT(m_RDataHair.initialized);
    makeCurrent();
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataHair.buffPosition->uploadDataAsync(m_HairModel->getVertices().data(),
                                              0, m_HairModel->getNTotalVertices() * m_HairModel->getDimension() * sizeof(float));
    ////////////////////////////////////////////////////////////////////////////////
    // color data
    //    if(m_RDataHair.pColorMode == HairColorMode::ObjectIndex ||
    //       m_RDataHair.pColorMode == HairColorMode::VelocityMagnitude) {
    //        if(m_RDataHair.pColorMode == HairColorMode::ObjectIndex) {
    //            m_RDataHair.buffColorData->uploadDataAsync(m_HairModel->objIndex, 0, m_HairModel->nParticles * sizeof(Int16));
    //            m_RDataHair.vColorMin = 0;
    //            m_RDataHair.vColorMax = m_HairModel->nObjects > 1u ? static_cast<float>(m_HairModel->nObjects - 1) : 1.0f;
    //        } else {
    //            static Vec_Float velMag2;
    //            velMag2.resize(m_HairModel->nParticles);
    //            if(m_HairModel->systemDimension == 2) {
    //                auto velPtr = reinterpret_cast<Vec2f*>(m_HairModel->velocities);
    //                __BNN_REQUIRE(velPtr != nullptr);
    //                Scheduler::parallel_for(velMag2.size(), [&](size_t i) { velMag2[i] = glm::length2(velPtr[i]); });
    //            } else {
    //                auto velPtr = reinterpret_cast<Vec3f*>(m_HairModel->velocities);
    //                __BNN_REQUIRE(velPtr != nullptr);
    //                Scheduler::parallel_for(velMag2.size(), [&](size_t i) { velMag2[i] = glm::length2(velPtr[i]); });
    //            }
    //            m_RDataHair.vColorMin = ParallelSTL::min(velMag2);
    //            m_RDataHair.vColorMax = ParallelSTL::max(velMag2);
    //            m_RDataHair.buffColorData->uploadDataAsync(velMag2.data(), 0, velMag2.size() * sizeof(float));
    //        }
    //    }
    ////////////////////////////////////////////////////////////////////////////////
    doneCurrent();
    m_RDataHair.nStrands  = m_HairModel->getNStrands();
    m_RDataHair.nVertices = m_HairModel->getNTotalVertices();
    //    m_RDataHair.pointRadius = m_HairModel->particleRadius;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::loadHairModel(const String& hairFile)
{
    if(m_HairModel->loadHairModel(hairFile)) {
        updateVizData();
        transformMeshWithHair();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setColorMode(int colorMode)
{
    Q_ASSERT(colorMode < HairColorMode::NumColorMode);
    Q_ASSERT(m_RDataHair.initialized);
    m_RDataHair.colorMode = colorMode;
    ////////////////////////////////////////////////////////////////////////////////
    if(colorMode == HairColorMode::ObjectIndex ||
       colorMode == HairColorMode::VelocityMagnitude) {
        updateVizData();
        makeCurrent();
        initHairVAO();
        doneCurrent();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setHairMaterial(const Material::MaterialData& material)
{
    makeCurrent();
    m_RDataHair.material->setMaterial(material);
    m_RDataHair.material->uploadDataToGPU();
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataHair()
{
    m_RDataHair.shader = std::make_shared<QtAppShaderProgram>("RenderHair");
    m_RDataHair.shader->addVertexShaderFromResource(":/Shaders/hair.vs.glsl");
    m_RDataHair.shader->addFragmentShaderFromResource(":/Shaders/hair.fs.glsl");
    m_RDataHair.shader->link();
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataHair.v_Position = m_RDataHair.shader->getAtributeLocation("v_Position");
    //    m_RDataHair.v_iColor   = m_RDataHair.shader->getAtributeLocation("v_iColor");
    //    m_RDataHair.v_fColor   = m_RDataHair.shader->getAtributeLocation("v_fColor");
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataHair.ub_CamData = m_RDataHair.shader->getUniformBlockIndex("CameraData");
    //    m_RDataHair.ub_Light    = m_RDataHair.shader->getUniformBlockIndex("Lights");
    m_RDataHair.ub_Material = m_RDataHair.shader->getUniformBlockIndex("Material");
    ////////////////////////////////////////////////////////////////////////////////
    //    m_RDataHair.u_nStrands   = m_RDataHair.shader->getUniformLocation("u_nStrands");
    m_RDataHair.u_SegmentIdx = m_RDataHair.shader->getUniformLocation("u_SegmentIdx");
    //    m_RDataHair.u_nParticles   = m_RDataHair.shader->getUniformLocation("u_nParticles");
    //    m_RDataHair.u_PointRadius  = m_RDataHair.shader->getUniformLocation("u_PointRadius");
    //    m_RDataHair.u_PointScale   = m_RDataHair.shader->getUniformLocation("u_PointScale");
    //    m_RDataHair.u_Dimension    = m_RDataHair.shader->getUniformLocation("u_Dimension");
    //    m_RDataHair.u_ScreenHeight = m_RDataHair.shader->getUniformLocation("u_ScreenHeight");
    //    m_RDataHair.u_DomainHeight = m_RDataHair.shader->getUniformLocation("u_DomainHeight");
    m_RDataHair.u_ColorMode = m_RDataHair.shader->getUniformLocation("u_ColorMode");
    //    m_RDataHair.u_vColorMin    = m_RDataHair.shader->getUniformLocation("u_vColorMin");
    //    m_RDataHair.u_vColorMax    = m_RDataHair.shader->getUniformLocation("u_vColorMax");
    //    m_RDataHair.u_ColorMinVal  = m_RDataHair.shader->getUniformLocation("u_ColorMinVal");
    //    m_RDataHair.u_ColorMaxVal  = m_RDataHair.shader->getUniformLocation("u_ColorMaxVal");
    //    m_RDataHair.u_ClipPlane    = m_RDataHair.shader->getUniformLocation("u_ClipPlane");
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataHair.buffPosition = std::make_unique<OpenGLBuffer>();
    m_RDataHair.buffPosition->createBuffer(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataHair.buffColorData = std::make_unique<OpenGLBuffer>();
    m_RDataHair.buffColorData->createBuffer(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataHair.material = std::make_unique<Material>();
    m_RDataHair.material->setMaterial(CUSTOM_HAIR_MATERIAL);
    m_RDataHair.material->uploadDataToGPU();
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataHair.initialized = true;
    initHairVAO();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initHairVAO()
{
    Q_ASSERT(m_RDataHair.initialized);
    glCall(glGenVertexArrays(1, &m_RDataHair.VAO));
    glCall(glBindVertexArray(m_RDataHair.VAO));
    glCall(glEnableVertexAttribArray(m_RDataHair.v_Position));
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataHair.buffPosition->bind();
    glCall(glVertexAttribPointer(m_RDataHair.v_Position, m_HairModel->getDimension(), GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(0)));
    ////////////////////////////////////////////////////////////////////////////////
    //    if(m_RDataHair.pColorMode == HairColorMode::ObjectIndex ||
    //       m_RDataHair.pColorMode == HairColorMode::VelocityMagnitude) {
    //        m_RDataHair.buffColorData->bind();
    //        if(m_RDataHair.pColorMode == HairColorMode::ObjectIndex) {
    //            glCall(glEnableVertexAttribArray(m_RDataHair.v_iColor));
    //            glCall(glVertexAttribIPointer(m_RDataHair.v_iColor, 1, GL_SHORT, 0, reinterpret_cast<GLvoid*>(0)));
    //        } else {
    //            glCall(glEnableVertexAttribArray(m_RDataHair.v_fColor));
    //            glCall(glVertexAttribPointer(m_RDataHair.v_fColor, 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(0)));
    //        }
    //    }
    ////////////////////////////////////////////////////////////////////////////////
    glCall(glBindVertexArray(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderHair()
{
    Q_ASSERT(m_RDataHair.initialized);

    m_RDataHair.shader->bind();
    ////////////////////////////////////////////////////////////////////////////////
    m_UBufferCamData->bindBufferBase();
    m_Lights->bindUniformBuffer();
    m_RDataHair.material->bindUniformBuffer();
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataHair.shader->bindUniformBlock(m_RDataHair.ub_CamData, m_UBufferCamData->getBindingPoint());

    m_RDataHair.shader->setUniformValue(m_RDataHair.u_nStrands, m_RDataHair.nStrands);

    //    m_RDataHair.shader->bindUniformBlock(m_RDataHair.ub_Light,    m_Lights->getBufferBindingPoint());
    m_RDataHair.shader->bindUniformBlock(m_RDataHair.ub_Material, m_RDataHair.material->getBufferBindingPoint());
    ////////////////////////////////////////////////////////////////////////////////
    //    m_RDataHair.shader->setUniformValue(m_RDataHair.u_nVertices,    m_RDataHair.nVertices);
    //    m_RDataHair.shader->setUniformValue(m_RDataHair.u_PointRadius,  m_RDataHair.pointRadius);
    //    m_RDataHair.shader->setUniformValue(m_RDataHair.u_PointScale,   m_RDataHair.pointScale);
    //    m_RDataHair.shader->setUniformValue(m_RDataHair.u_Dimension,    m_HairModel->systemDimension);
    //    m_RDataHair.shader->setUniformValue(m_RDataHair.u_ScreenHeight, height());
    //    m_RDataHair.shader->setUniformValue(m_RDataHair.u_DomainHeight, (m_Camera->getOrthoBoxMax().y - m_Camera->getOrthoBoxMin().y) * 0.9f);
    m_RDataHair.shader->setUniformValue(m_RDataHair.u_ColorMode, m_RDataHair.colorMode);
    //    m_RDataHair.shader->setUniformValue(m_RDataHair.u_ClipPlane,    m_ClipPlane);
    ////////////////////////////////////////////////////////////////////////////////
    //    if(m_RDataHair.pColorMode == HairColorMode::ObjectIndex ||
    //       m_RDataHair.pColorMode == HairColorMode::VelocityMagnitude) {
    //        m_RDataHair.shader->setUniformValue(m_RDataHair.u_vColorMin,   m_RDataHair.vColorMin);
    //        m_RDataHair.shader->setUniformValue(m_RDataHair.u_vColorMax,   m_RDataHair.vColorMax);
    //        m_RDataHair.shader->setUniformValue(m_RDataHair.u_ColorMinVal, m_RDataHair.colorMinVal);
    //        m_RDataHair.shader->setUniformValue(m_RDataHair.u_ColorMaxVal, m_RDataHair.colorMaxVal);
    //    }
    ////////////////////////////////////////////////////////////////////////////////
    glCall(glBindVertexArray(m_RDataHair.VAO));
    //    glCall(glEnable(GL_VERTEX_PROGRAM_POINT_SIZE));
    UInt vertexIdx = 0;
    for(UInt strand = 0; strand < m_RDataHair.nStrands; ++strand) {
        UInt nVertices = m_HairModel->getNStrandVertices()[strand];
        m_RDataHair.shader->setUniformValue(m_RDataHair.u_SegmentIdx, strand);
        ////////////////////////////////////////////////////////////////////////////////
        glDrawArrays(GL_LINE_STRIP, vertexIdx, nVertices);
        vertexIdx += nVertices;
    }

    glCall(glBindVertexArray(0));
    m_RDataHair.shader->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::loadMesh(const QString& meshFile)
{
    MeshLoader meshLoader(meshFile.toStdString());
    if(m_HairModel->getModelType() == HairModel::ModelType::CYType) {
        meshLoader.swapYZ();
        meshLoader.swapXZ();
    }
    bool isEmpty = m_MeshObject->isEmpty();
    m_MeshObject->clearData();
    m_MeshObject->setVertices(meshLoader.getFaceVertices());
    m_MeshObject->setVertexNormal(meshLoader.getFaceVertexNormals());

    // backup vertices before transformation
    m_MeshObject->backupVertices();
    m_MeshObject->transform(m_HairModel->getTranslation(), m_HairModel->getScale());

    makeCurrent();
    m_MeshObject->uploadDataToGPU();
    if(isEmpty) {
        m_MeshRender->setupVAO();
    }
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::transformMeshWithHair()
{
    // perform transformation on original vertices
    m_MeshObject->restoreVertices();
    m_MeshObject->transform(m_HairModel->getTranslation(), m_HairModel->getScale());
    makeCurrent();
    m_MeshObject->uploadDataToGPU();
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataMesh()
{
    Q_ASSERT(m_UBufferCamData != nullptr && m_Lights != nullptr);

    m_MeshObject = std::make_shared<MeshObject>();
    m_MeshRender = std::make_unique<MeshRender>(m_MeshObject, m_Camera, m_Lights, nullptr, m_UBufferCamData);

    m_MeshRender->getMaterial()->setMaterial(CUSTOM_MESH_MATERIAL);
    m_MeshRender->getMaterial()->uploadDataToGPU();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderMesh()
{
    Q_ASSERT(m_MeshRender != nullptr);
    m_MeshRender->render();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setMeshMaterial(const Material::MaterialData& material)
{
    Q_ASSERT(m_MeshRender != nullptr);
    makeCurrent();
    m_MeshRender->getMaterial()->setMaterial(material);
    m_MeshRender->getMaterial()->uploadDataToGPU();
    doneCurrent();
}
