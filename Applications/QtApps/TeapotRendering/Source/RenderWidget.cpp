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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
RenderWidget::RenderWidget(QWidget* parent) : OpenGLWidget(parent)
{
    m_DefaultSize = QSize(1200, 1000);
    m_Camera->setTranslationLag(0.7);
    m_Camera->setRotationLag(0.7);
    m_Camera->setZoomingLag(0.7);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initOpenGL()
{
    glCall(glGetBooleanv(GL_FRAMEBUFFER_SRGB_CAPABLE_EXT, &m_bUseSRGB));

    ////////////////////////////////////////////////////////////////////////////////
    // light
    m_Lights = std::make_shared<PointLights>();
    m_Lights->setNumLights(1);
    m_Lights->setLightPosition(glm::vec4(0, 100, -100, 1.0), 0);
    m_Lights->setLightDiffuse(glm::vec4(1.0), 0);

    ////////////////////////////////////////////////////////////////////////////////
    // textures
    m_RenderTexture = std::make_shared<OpenGLTexture>(GL_TEXTURE_2D);
    m_RenderTexture->setSimplestTexture();
    m_ScreenQuadTexRender = std::make_unique<ScreenQuadTextureRender>();

    m_FloorTexFile = getTextureFilePaths("Floor");
    m_SkyTexFile   = getTextureFilePaths("Sky");

    ////////////////////////////////////////////////////////////////////////////////
    // ray tracing engine
    initRayTracer();
    initCaptureDir();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::resizeOpenGLWindow(int width, int height)
{
    glCall(glViewport(0, 0, width, height));

    if(m_RayTracer != nullptr)
        m_RayTracer->resizeViewport(width, height);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderOpenGL()
{
    renderRayTracingBuffer();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::resetCamera()
{
    m_Camera->reset();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateLights()
{
    m_RayTracer->setLights(m_Lights);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setSkyBoxTexture(int texIndex)
{
    if(texIndex > 0)
        m_RayTracer->setSkyTexture(m_SkyTexFile[texIndex - 1]);
    else
        m_RayTracer->setSkyTexture(QString(""));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setFloorTexture(int texIndex)
{
    if(texIndex > 0)
        m_RayTracer->setFloorTexture(m_FloorTexFile[texIndex - 1]);
    else
        m_RayTracer->setFloorTexture(QString(""));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setFloorSize(int size)
{
    m_RayTracer->setFloorSize(size);
}

void RenderWidget::setFloorTexScale(int scale)
{
    float fscale = static_cast<float>(scale) / 100.0f;
    m_RayTracer->setFloorTexScale(fscale);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::reloadTextures()
{
    m_FloorTexFile = getTextureFilePaths("Floor");
    m_SkyTexFile   = getTextureFilePaths("Sky");

    ////////////////////////////////////////////////////////////////////////////////
    EnhancedMessageBox msgBox(this);
    msgBox.setAutoCloseTimeout(2000);
    msgBox.setText("Textures reloaded!");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setAutoClose(true);
    msgBox.exec();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::saveRenderImage()
{
    m_RayTracer->getOutputAsByteArray(m_CaptureImage->bits());
    m_CaptureImage->save(getCaptureFilePath(getLastCaptureIdx()));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setMeshMaterial(const Material::MaterialData& material, int meshID)
{
    m_RayTracer->setGlassMaterial(glm::vec3(material.diffuse));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRayTracer()
{
    m_RayTracer = std::make_unique<TeapotRayTracer>(m_Camera);

    m_RayTracer->createOptiXContext(width(), height());
    m_RayTracer->createPrograms();
    m_RayTracer->createScene();
    m_RayTracer->validateContext();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderRayTracingBuffer()
{
    Q_ASSERT(m_RayTracer != nullptr);
    Q_ASSERT(m_RenderTexture != nullptr);

    // launch ray tracing engine
    m_RayTracer->render();
    m_RayTracer->getOutputAsTexture(m_RenderTexture);

    // render
    if(m_bUseSRGB)
        glCall(glEnable(GL_FRAMEBUFFER_SRGB_EXT));
    glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    m_ScreenQuadTexRender->setTexture(m_RenderTexture, 3);
    m_ScreenQuadTexRender->render();
    if(m_bUseSRGB)
        glDisable(GL_FRAMEBUFFER_SRGB_EXT);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initCaptureDir()
{
    QString capturePath = QDir::currentPath() + QString("/Capture/");
    setCapturePath(capturePath);

    if(!QDir(capturePath).exists())
        QDir().mkdir(capturePath);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int RenderWidget::getLastCaptureIdx()
{
    int idx = 1;
    for(; idx < 1000000; ++idx)
    {
        if(!QFile::exists(getCaptureFilePath(idx)))
            break;
    }

    return idx;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QString RenderWidget::getCaptureFilePath(int frame)
{
    return m_CapturePath + QString("/frame.%1.jpg").arg(frame, 4, 10, QChar('0'));
}
