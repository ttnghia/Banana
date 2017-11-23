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

#pragma once

#include "Common.h"

#include <OpenGLHelpers/OpenGLBuffer.h>
#include <OpenGLHelpers/OpenGLTexture.h>
#include <OpenGLHelpers/MeshObjects/MeshObject.h>
#include <OpenGLHelpers/Material.h>
#include <OpenGLHelpers/Lights.h>
#include <OpenGLHelpers/RenderObjects.h>

#include <QtAppHelpers/QtAppShaderProgram.h>
#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/OpenGLWidget.h>
#include <QtAppHelpers/EnhancedMessageBox.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RenderWidget : public OpenGLWidget
{
    Q_OBJECT
public:
    RenderWidget(QWidget* parent, const SharedPtr<VisualizationData>& vizData);
private:
    virtual void initOpenGL();
    virtual void resizeOpenGLWindow(int, int);
    virtual void renderOpenGL();
    void         initCaptureDir();
    SharedPtr<VisualizationData> m_VizData;
public slots:
    void updateCamera() { m_Camera->setDefaultCamera(m_VizData->cameraPosition, m_VizData->cameraFocus, Vec3f(0, 1, 0)); }
    void updateProjection();
    void updateVizData();

    ////////////////////////////////////////////////////////////////////////////////
    // clip plane
public slots:
    void enableClipPlane(bool bEnable);
    void setClipPlane(const Vec4f& clipPlane) { m_ClipPlane = clipPlane; }
private:
    Vec4f m_ClipPlane = DEFAULT_CLIP_PLANE;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // light
signals:
    void lightsObjChanged(const SharedPtr<PointLights>& lights);
public slots:
    void updateLights();
private:
    void initRDataLight();
    void renderLight() { Q_ASSERT(m_LightRender != nullptr); m_LightRender->render(); }
    SharedPtr<PointLights>      m_Lights      = nullptr;
    UniquePtr<PointLightRender> m_LightRender = nullptr;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // skybox and checkerboard background
public slots:
    void setBackgroundMode(int backgroundMode) { m_BackgroundMode = backgroundMode; }
    void setSkyBoxTexture(int texIndex) { Q_ASSERT(m_SkyBoxRender != nullptr); m_SkyBoxRender->setRenderTextureIndex(texIndex); }
    void setCheckerboarrdColor1(const Vec3f& color1) { Q_ASSERT(m_CheckerboardRender != nullptr); m_CheckerboardRender->setColor1(color1); }
    void setCheckerboarrdColor2(const Vec3f& color2) { Q_ASSERT(m_CheckerboardRender != nullptr); m_CheckerboardRender->setColor2(color2); }
    void setCheckerboarrdScales(const Vec2i& scales) { Q_ASSERT(m_CheckerboardRender != nullptr); m_CheckerboardRender->setScales(scales); }
    void setGridBackgroundColor(const Vec3f& backgroundColor) { Q_ASSERT(m_GridRender != nullptr); m_GridRender->setBackgroundColor(backgroundColor); }
    void setGridLineColor(const Vec3f& lineColor) { Q_ASSERT(m_GridRender != nullptr); m_GridRender->setLineColor(lineColor); }
    void setGridScales(const Vec2i& scales) { Q_ASSERT(m_GridRender != nullptr); m_GridRender->setScales(scales); }
private:
    void initRDataSkyBox();
    void initRDataCheckerboardBackground() { m_CheckerboardRender = std::make_unique<CheckerboardBackgroundRender>(DEFAULT_CHECKERBOARD_COLOR1, DEFAULT_CHECKERBOARD_COLOR2); }
    void initRDataGridBackground() { m_GridRender = std::make_unique<GridBackgroundRender>(DEFAULT_CHECKERBOARD_COLOR1, DEFAULT_CHECKERBOARD_COLOR2); }
    void renderSkyBox() { Q_ASSERT(m_SkyBoxRender != nullptr); m_SkyBoxRender->render(); }
    void renderCheckerboardBackground() { Q_ASSERT(m_CheckerboardRender != nullptr); m_CheckerboardRender->render(); }
    void renderGridBackground() { Q_ASSERT(m_GridRender != nullptr); m_GridRender->render(); }

    UniquePtr<SkyBoxRender>                 m_SkyBoxRender       = nullptr;
    UniquePtr<CheckerboardBackgroundRender> m_CheckerboardRender = nullptr;
    UniquePtr<GridBackgroundRender>         m_GridRender         = nullptr;
    Int                                     m_BackgroundMode     = static_cast<Int>(BackgroundMode::SkyBox);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // floor
public slots:
    void setFloorTexture(int texIndex) { Q_ASSERT(m_FloorRender != nullptr); m_FloorRender->setRenderTextureIndex(texIndex); }
    void setFloorExposure(int percentage) { m_FloorRender->setExposure(static_cast<float>(percentage) / 100.0f); }
    void setFloorSize(int size) { m_FloorRender->transform(Vec3f(0, -1.01, 0), Vec3f(static_cast<float>(size))); }
    void setFloorTexScales(int scale) { m_FloorRender->scaleTexCoord(scale, scale); }
private:
    void initRDataFloor();
    void renderFloor() { Q_ASSERT(m_FloorRender != nullptr); m_FloorRender->render(); }
    UniquePtr<PlaneRender> m_FloorRender = nullptr;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // domain box
public slots:
    void setRenderBox(bool bRender) { m_bRenderBox = bRender; }
    void setBoxColor(const Vec3f& color) { Q_ASSERT(m_DomainBoxRender != nullptr); m_DomainBoxRender->setColor(color); }
    void updateBox() { makeCurrent(); m_DomainBoxRender->setBox(m_VizData->boxMin, m_VizData->boxMax); doneCurrent(); }
private:
    void initRDataBox();
    void renderBox() { Q_ASSERT(m_DomainBoxRender != nullptr); if(m_bRenderBox) { m_DomainBoxRender->render(); } }
    UniquePtr<WireFrameBoxRender> m_DomainBoxRender = nullptr;
    bool                          m_bRenderBox      = true;
    ////////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////////
    // particles
public slots:
    void setParticleColorMode(int colorMode);
    void setColorDataMin(const Vec3f& colorMin) { m_RDataParticle.colorMinVal = colorMin; }
    void setColorDataMax(const Vec3f& colorMax) { m_RDataParticle.colorMaxVal = colorMax; }
    void setParticleMaterial(const Material::MaterialData& material);
    void enableAniKernels(bool bAniKernel) { m_RDataParticle.useAnisotropyKernel = bAniKernel ? 1 : 0; }
private:
    struct RDataParticle
    {
        SharedPtr<QtAppShaderProgram> shader         = nullptr;
        UniquePtr<OpenGLBuffer>       buffPosition   = nullptr;
        UniquePtr<OpenGLBuffer>       buffAniKernels = nullptr;
        UniquePtr<OpenGLBuffer>       buffColorData  = nullptr;
        UniquePtr<Material>           material       = nullptr;

        GLuint VAO;
        GLint  v_Position;
        GLint  v_AnisotropyMatrix0;
        GLint  v_AnisotropyMatrix1;
        GLint  v_AnisotropyMatrix2;
        GLint  v_iColor;
        GLint  v_fColor;
        GLuint ub_CamData;
        GLuint ub_Light;
        GLuint ub_Material;
        GLuint u_nParticles;
        GLuint u_PointRadius;
        GLuint u_ClipPlane;
        GLuint u_IsPointView;
        GLuint u_ColorMode;
        GLuint u_VColorMin;
        GLuint u_VColorMax;
        GLuint u_ColorMinVal;
        GLuint u_ColorMaxVal;
        GLuint u_UseAniKernel;
        GLuint u_ScreenWidth;
        GLuint u_ScreenHeight;

        GLuint  nParticles = 0;
        GLfloat pointRadius;

        GLint isPointView         = 0;
        GLint useAnisotropyKernel = 1;
        GLint hasAniKernel        = 0;
        GLint pColorMode          = ParticleColorMode::Ramp;

        float vColorMin   = 0;
        float vColorMax   = 1.0f;
        Vec3f colorMinVal = DEFAULT_COLOR_DATA_MIN;
        Vec3f colorMaxVal = DEFAULT_COLOR_DATA_MAX;

        bool initialized = false;
    } m_RDataParticle;

    void initRDataParticle();
    void initParticleVAO();
    void renderParticles();
    ////////////////////////////////////////////////////////////////////////////////
};