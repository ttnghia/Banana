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
#include "DataReader.h"

#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/OpenGLWidget.h>

#include <OpenGLHelpers/OpenGLBuffer.h>
#include <OpenGLHelpers/OpenGLTexture.h>
#include <OpenGLHelpers/RenderObjects.h>
#include <QtAppHelpers/QtAppShaderProgram.h>

#include <ParticleTools/ParticleSerialization.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RenderWidget : public OpenGLWidget
{
    Q_OBJECT
public:
    RenderWidget(QWidget* parent, const SharedPtr<VisualizationData>& vizData) : OpenGLWidget(parent), m_VizData(vizData) { updateCamera(); }
private:
    virtual void initOpenGL() override;
    virtual void resizeOpenGLWindow(int width, int height) override { if(m_CheckerboardRender != nullptr) { m_CheckerboardRender->setScreenSize(width, height); } }
    virtual void renderOpenGL() override;
    SharedPtr<VisualizationData> m_VizData = nullptr;

public slots:
    void updateCamera() { m_Camera->setDefaultCamera(m_VizData->cameraPosition, m_VizData->cameraFocus, Vec3f(0, 1, 0)); }
    void updateData();

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
    void renderLight() { Q_ASSERT(m_LightRender != nullptr);    m_LightRender->render(); }
    SharedPtr<PointLights>      m_Lights      = nullptr;
    UniquePtr<PointLightRender> m_LightRender = nullptr;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // skybox and checkerboard background
public slots:
    void setRenderCheckerboard(bool bCheckerboard) { m_bRenderCheckerboardBackground = bCheckerboard; }
    void setSkyBoxTexture(int texIndex) { Q_ASSERT(m_SkyBoxRender != nullptr); m_SkyBoxRender->setRenderTextureIndex(texIndex); }
    void setCheckerboarrdColor1(const Vec3f& color1) { Q_ASSERT(m_CheckerboardRender != nullptr); m_CheckerboardRender->setColor1(color1); }
    void setCheckerboarrdColor2(const Vec3f& color2) { Q_ASSERT(m_CheckerboardRender != nullptr); m_CheckerboardRender->setColor2(color2); }
    void setCheckerboarrdScales(const Vec2f& texScales) { Q_ASSERT(m_CheckerboardRender != nullptr); m_CheckerboardRender->setTexScales(texScales); }
private:
    void initRDataSkyBox();
    void initRDataCheckerboardBackground() { m_CheckerboardRender = std::make_unique<CheckerboardBackgroundRender>(DEFAULT_CHECKERBOARD_COLOR1, DEFAULT_CHECKERBOARD_COLOR2); }
    void renderSkyBox() { Q_ASSERT(m_SkyBoxRender != nullptr); m_SkyBoxRender->render(); }
    void renderCheckerboardBackground() { Q_ASSERT(m_CheckerboardRender != nullptr); m_CheckerboardRender->render(); }

    UniquePtr<SkyBoxRender>                 m_SkyBoxRender                  = nullptr;
    UniquePtr<CheckerboardBackgroundRender> m_CheckerboardRender            = nullptr;
    bool                                    m_bRenderCheckerboardBackground = false;
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
    void updateBox() { makeCurrent(); m_DomainBoxRender->setBox(m_VizData->boxMin, m_VizData->boxMax); doneCurrent(); }
private:
    void initRDataBox();
    void renderBox() { Q_ASSERT(m_DomainBoxRender != nullptr); m_DomainBoxRender->render(); }
    UniquePtr<WireFrameBoxRender> m_DomainBoxRender = nullptr;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // particles
public slots:
    void setParticleColorMode(int colorMode);
    void setParticleMaterial(const Material::MaterialData& material);
    void enableAniKernels(bool bAniKernel) { m_RDataParticle.useAnisotropyKernel = bAniKernel ? 1 : 0; }
private:
    struct RDataParticle
    {
        Vec_UInt16 positionDataCompressed;
        Vec_UInt16 aniKernelDataCompressed;
        Vec_UInt16 colorDataCompressed;

        Vec3f dMinPosition;
        Vec3f dMaxPosition;
        Vec3f dMinColorData;
        Vec3f dMaxColorData;
        float dMinAniKernel;
        float dMaxAniKernel;

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
        GLint  v_Color;
        GLuint ub_CamData;
        GLuint ub_Light;
        GLuint ub_Material;
        GLuint u_MinPosition;
        GLuint u_MaxPosition;
        GLuint u_PointRadius;
        GLuint u_ClipPlane;
        GLuint u_IsPointView;
        GLuint u_HasVColor;
        GLuint u_UseAnisotropyKernel;
        GLuint u_ScreenWidth;
        GLuint u_ScreenHeight;

        GLuint  numParticles = 0;
        GLfloat pointRadius;

        GLint isPointView         = 0;
        GLint hasVColor           = 0;
        GLint useAnisotropyKernel = 1;
        GLint hasAnisotropyKernel = 0;
        GLint pColorMode          = ParticleColorMode::Ramp;
        bool  initialized         = false;
    } m_RDataParticle;

    void initRDataParticle();
    void initParticleVAO();
    void renderParticles();
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // meshes
public slots:
    void setMeshMaterial(const Material::MaterialData& material, int meshID = 0);
    void updateSimMeshes();
    void updateBoundaryMeshes();
    void updateNumSimMeshes(int numMeshes);
    void updateNumBoundaryMeshes(int numMeshes);
private:
    int                            m_NumSimMeshes      = 0;
    int                            m_NumBoundaryMeshes = 0;
    Vector<SharedPtr<MeshObject> > m_SimMeshObjs;
    Vector<SharedPtr<MeshObject> > m_BoundaryMeshObjs;
    Vector<UniquePtr<MeshRender> > m_SimMeshRenders;
    Vector<UniquePtr<MeshRender> > m_BoundaryMeshRenders;
    ////////////////////////////////////////////////////////////////////////////////
};