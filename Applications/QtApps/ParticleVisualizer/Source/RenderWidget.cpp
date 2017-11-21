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

    renderFloor();
    renderLight();
    renderBox();
    renderParticles();
    // renderMeshes();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateData()
{
    Q_ASSERT(m_RDataParticle.initialized);
    makeCurrent();
    ////////////////////////////////////////////////////////////////////////////////
    __BNN_ASSERT(m_VizData->particleReader.getParticleAttributeCompressed("position", m_RDataParticle.positionDataCompressed, m_RDataParticle.dMinPosition, m_RDataParticle.dMaxPosition));
    m_RDataParticle.buffPosition->uploadDataAsync(m_RDataParticle.positionDataCompressed.data(), 0, m_RDataParticle.positionDataCompressed.size() * sizeof(UInt16));

    if(m_RDataParticle.useAnisotropyKernel) {
        if(m_VizData->particleReader.hasParticleAttribute("anisotropic_kernel")) {
            __BNN_ASSERT(m_VizData->particleReader.getParticleAttributeCompressed("anisotropic_kernel", m_RDataParticle.aniKernelDataCompressed, m_RDataParticle.dMinAniKernel, m_RDataParticle.dMaxAniKernel));
            m_RDataParticle.buffAniKernels->uploadDataAsync(m_RDataParticle.aniKernelDataCompressed.data(), 0, m_RDataParticle.aniKernelDataCompressed.size() * sizeof(UInt16));
            m_RDataParticle.hasAnisotropyKernel = 1;
        } else {
            m_RDataParticle.hasAnisotropyKernel = 0;
        }
    }

    if(m_RDataParticle.pColorMode == ParticleColorMode::FromData) {
        if(!m_VizData->colorDataName.empty()) {
            if(m_RDataParticle.colorDataSize == 1) {
                __BNN_ASSERT(m_VizData->particleReader.getParticleAttributeCompressed(m_VizData->colorDataName, m_RDataParticle.colorDataCompressed, m_RDataParticle.dMinColorData1, m_RDataParticle.dMaxColorData1));
            } else {
                __BNN_ASSERT(m_VizData->particleReader.getParticleAttributeCompressed(m_VizData->colorDataName, m_RDataParticle.colorDataCompressed, m_RDataParticle.dMinColorData3, m_RDataParticle.dMaxColorData3));
            }

            m_RDataParticle.buffColorData->uploadDataAsync(m_RDataParticle.colorDataCompressed.data(), 0, m_RDataParticle.colorDataCompressed.size() * sizeof(UInt16));
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    doneCurrent();
    m_VizData->particleReader.getFixedAttribute("particle_radius", m_RDataParticle.pointRadius);
    m_RDataParticle.nParticles = m_VizData->particleReader.getNParticles();
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
        makeCurrent();
        m_Lights->uploadDataToGPU();
        doneCurrent();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataLight()
{
    m_Lights = std::make_shared<PointLights>();
    m_Lights->setNumLights(1);

    m_Lights->setLightPosition(DEFAULT_LIGHT_POSITION, 0);
    //    m_Lights->setLightDiffuse(glm::vec4(1.0), 0);

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
    m_SkyBoxRender = std::make_unique<SkyBoxRender>(m_Camera, QDir::currentPath() + "/Textures/Sky/", m_UBufferCamData);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataFloor()
{
    Q_ASSERT(m_UBufferCamData != nullptr && m_Lights != nullptr);

    m_FloorRender = std::make_unique<PlaneRender>(m_Camera, m_Lights, QDir::currentPath() + "/Textures/Floor/", m_UBufferCamData);
    m_FloorRender->transform(Vec3f(0, -1.01, 0), Vec3f(10));
    m_FloorRender->scaleTexCoord(2, 2);
    m_FloorRender->setAllowNonTextureRender(false);
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
    if(m_RDataParticle.pColorMode == ParticleColorMode::FromData && !m_VizData->colorDataName.empty()) {
        makeCurrent();
        initParticleVAO();
        doneCurrent();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setColorData(const String& colorDataName, Int dataSize)
{
    m_VizData->colorDataName      = colorDataName;
    m_RDataParticle.colorDataSize = dataSize;
    ////////////////////////////////////////////////////////////////////////////////
    if(m_RDataParticle.pColorMode == ParticleColorMode::FromData && !m_VizData->colorDataName.empty()) {
        makeCurrent();
        initParticleVAO();
        doneCurrent();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setMeshMaterial(const Material::MaterialData& material, int meshID /*= 0*/)
{
    makeCurrent();
    m_SimMeshRenders[meshID]->getMaterial()->setMaterial(material);
    m_SimMeshRenders[meshID]->getMaterial()->uploadDataToGPU();
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataParticle()
{
    m_RDataParticle.shader = std::make_shared<QtAppShaderProgram>("RenderPointSphere");
    m_RDataParticle.shader->addVertexShaderFromResource(":/Shaders/particle.vs.glsl");
    m_RDataParticle.shader->addFragmentShaderFromResource(":/Shaders/particle.fs.glsl");
    m_RDataParticle.shader->link();

    m_RDataParticle.v_Position          = m_RDataParticle.shader->getAtributeLocation("v_Position");
    m_RDataParticle.v_AnisotropyMatrix0 = m_RDataParticle.shader->getAtributeLocation("v_AnisotropyMatrix0");
    m_RDataParticle.v_AnisotropyMatrix1 = m_RDataParticle.shader->getAtributeLocation("v_AnisotropyMatrix1");
    m_RDataParticle.v_AnisotropyMatrix2 = m_RDataParticle.shader->getAtributeLocation("v_AnisotropyMatrix2");
    m_RDataParticle.v_Color1            = m_RDataParticle.shader->getAtributeLocation("v_Color1");
    m_RDataParticle.v_Color3            = m_RDataParticle.shader->getAtributeLocation("v_Color3");
    m_RDataParticle.u_ClipPlane         = m_RDataParticle.shader->getUniformLocation("u_ClipPlane");

    m_RDataParticle.ub_CamData  = m_RDataParticle.shader->getUniformBlockIndex("CameraData");
    m_RDataParticle.ub_Light    = m_RDataParticle.shader->getUniformBlockIndex("Lights");
    m_RDataParticle.ub_Material = m_RDataParticle.shader->getUniformBlockIndex("Material");

    m_RDataParticle.u_nParticles  = m_RDataParticle.shader->getUniformLocation("u_nParticles");
    m_RDataParticle.u_PointRadius = m_RDataParticle.shader->getUniformLocation("u_PointRadius");
    //    m_RDataParticle.u_PointScale = m_RDataParticle.shader->getUniformLocation("u_PointScale");
    m_RDataParticle.u_IsPointView         = m_RDataParticle.shader->getUniformLocation("u_IsPointView");
    m_RDataParticle.u_ColorMode           = m_RDataParticle.shader->getUniformLocation("u_ColorMode");
    m_RDataParticle.u_ColorDataSize       = m_RDataParticle.shader->getUniformLocation("u_ColorDataSize");
    m_RDataParticle.u_ColorDataMin        = m_RDataParticle.shader->getUniformLocation("u_ColorDataMin");
    m_RDataParticle.u_ColorDataMax        = m_RDataParticle.shader->getUniformLocation("u_ColorDataMax");
    m_RDataParticle.u_UseAnisotropyKernel = m_RDataParticle.shader->getUniformLocation("u_UseAnisotropyKernel");
    m_RDataParticle.u_ScreenWidth         = m_RDataParticle.shader->getUniformLocation("u_ScreenWidth");
    m_RDataParticle.u_ScreenHeight        = m_RDataParticle.shader->getUniformLocation("u_ScreenHeight");

    m_RDataParticle.u_MinPosition = m_RDataParticle.shader->getUniformLocation("u_MinPosition");
    m_RDataParticle.u_MaxPosition = m_RDataParticle.shader->getUniformLocation("u_MaxPosition");

    m_RDataParticle.buffPosition = std::make_unique<OpenGLBuffer>();
    m_RDataParticle.buffPosition->createBuffer(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);

    m_RDataParticle.buffAniKernels = std::make_unique<OpenGLBuffer>();
    m_RDataParticle.buffAniKernels->createBuffer(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);

    m_RDataParticle.buffColorData = std::make_unique<OpenGLBuffer>();
    m_RDataParticle.buffColorData->createBuffer(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);

    m_RDataParticle.material = std::make_unique<Material>();
    m_RDataParticle.material->setMaterial(CUSTOM_PARTICLE_MATERIAL);
    m_RDataParticle.material->uploadDataToGPU();

    ////////////////////////////////////////////////////////////////////////////////
    initParticleVAO();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initParticleVAO()
{
    glCall(glGenVertexArrays(1, &m_RDataParticle.VAO));
    glCall(glBindVertexArray(m_RDataParticle.VAO));
    glCall(glEnableVertexAttribArray(m_RDataParticle.v_Position));

    m_RDataParticle.buffPosition->bind();
    glCall(glVertexAttribIPointer(m_RDataParticle.v_Position, 3, GL_UNSIGNED_SHORT, 0, reinterpret_cast<GLvoid*>(0)));

//    m_RDataParticle.buffAniKernels->bind();
//    glCall(glEnableVertexAttribArray(m_RDataParticle.v_AnisotropyMatrix0));
//    glCall(glEnableVertexAttribArray(m_RDataParticle.v_AnisotropyMatrix1));
//    glCall(glEnableVertexAttribArray(m_RDataParticle.v_AnisotropyMatrix2));
//    glCall(glVertexAttribPointer(m_RDataParticle.v_AnisotropyMatrix0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0)));
//    glCall(glVertexAttribPointer(m_RDataParticle.v_AnisotropyMatrix1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(sizeof(GLfloat) * 3)));
//    glCall(glVertexAttribPointer(m_RDataParticle.v_AnisotropyMatrix2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(sizeof(GLfloat) * 6)));

    if(m_RDataParticle.pColorMode == ParticleColorMode::FromData) {
        m_RDataParticle.buffColorData->bind();
        if(m_RDataParticle.colorDataSize == 1) {
            glCall(glEnableVertexAttribArray(m_RDataParticle.v_Color1));
            glCall(glVertexAttribIPointer(m_RDataParticle.v_Color1, 1, GL_UNSIGNED_SHORT, 0, reinterpret_cast<GLvoid*>(0)));
        } else {
            glCall(glEnableVertexAttribArray(m_RDataParticle.v_Color3));
            glCall(glVertexAttribIPointer(m_RDataParticle.v_Color3, 3, GL_UNSIGNED_SHORT, 0, reinterpret_cast<GLvoid*>(0)));
        }
    }

    glCall(glBindVertexArray(0));
    m_RDataParticle.initialized = true;
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
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_IsPointView, m_RDataParticle.isPointView);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ColorMode, m_RDataParticle.pColorMode);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ClipPlane, m_ClipPlane);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ScreenWidth, width());
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ScreenHeight, height());

    if(m_RDataParticle.pColorMode == ParticleColorMode::FromData) {
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ColorDataSize, m_RDataParticle.colorDataSize);
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ColorDataMin, m_RDataParticle.colorDataMin);
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ColorDataMax, m_RDataParticle.colorDataMax);
    }

    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_MinPosition, m_RDataParticle.dMinPosition);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_MaxPosition, m_RDataParticle.dMaxPosition);

    if(m_RDataParticle.useAnisotropyKernel && m_RDataParticle.hasAnisotropyKernel) {
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_UseAnisotropyKernel, 1);
    } else {
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_UseAnisotropyKernel, 0);
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateSimMeshes()
{
    if(isValid()) {
        makeCurrent();
        for(auto& mesh : m_SimMeshObjs) {
            mesh->uploadDataToGPU();
        }
        doneCurrent();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateBoundaryMeshes()
{
    if(isValid()) {
        makeCurrent();
        for(auto& mesh : m_BoundaryMeshObjs) {
            mesh->uploadDataToGPU();
        }
        doneCurrent();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateNumSimMeshes(int numMeshes)
{
    if(isValid()) {
        m_NumSimMeshes = numMeshes;
        makeCurrent();
        for(auto& meshRender : m_SimMeshRenders) {
            meshRender->setupVAO();
            meshRender->initDepthBufferData(m_ClearColor);
        }
        doneCurrent();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateNumBoundaryMeshes(int numMeshes)
{
    if(isValid()) {
        m_NumBoundaryMeshes = numMeshes;
        makeCurrent();
        for(auto& meshRender : m_BoundaryMeshRenders) {
            meshRender->setupVAO();
            meshRender->initDepthBufferData(m_ClearColor);
        }
        doneCurrent();
    }
}
