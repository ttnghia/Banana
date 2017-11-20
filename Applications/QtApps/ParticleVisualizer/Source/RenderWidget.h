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

#include <OpenGLHelpers/ShaderProgram.h>
#include <OpenGLHelpers/OpenGLBuffer.h>
#include <OpenGLHelpers/OpenGLTexture.h>
#include <OpenGLHelpers/RenderObjects.h>

#include <ParticleTools/ParticleSerialization.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RenderWidget : public OpenGLWidget
{
    Q_OBJECT
public:
    RenderWidget(QWidget* parent);
    void setCamera(const glm::vec3& cameraPosition, const glm::vec3& cameraFocus);
    void setBox(const glm::vec3& boxMin, const glm::vec3& boxMax);

protected:
    virtual void initOpenGL() override;
    virtual void resizeOpenGLWindow(int, int) override {}
    virtual void renderOpenGL() override;

public slots:
    void updateData(const SharedPtr<SimulationData>& simData);
    void updateLights();

    void updateSimMeshes();
    void updateBoundaryMeshes();
    void updateNumSimMeshes(int numMeshes);
    void updateNumBoundaryMeshes(int numMeshes);

    void setSkyBoxTexture(int texIndex);
    void setFloorTexture(int texIndex);
    void setFloorSize(int size);
    void setParticleColorMode(int colorMode);

    void reloadTextures();

    void setParticleMaterial(const Material::MaterialData& material);
    void setMeshMaterial(const Material::MaterialData& material, int meshID);

    void enableAnisotropyKernel(bool bAniKernel);
    void enableShadow(bool bShadowEnabled);
    void visualizeShadowRegion(bool bVisualizeShadow);
    void setShadowIntensity(int intensity);

    void enableClipPlane(bool bEnable);
    void setClipPlane(const glm::vec4& clipPlane);

    void enableExportFrame(bool bEnable);

signals:
    void lightsObjChanged(const SharedPtr<PointLights>& lights);

private:
    ////////////////////////////////////////////////////////////////////////////////
    void initRDataLight();
    void renderLight();

    ////////////////////////////////////////////////////////////////////////////////
    void initRDataSkyBox();
    void renderSkyBox();

    ////////////////////////////////////////////////////////////////////////////////
    void initRDataFloor();
    void renderFloor();

    ////////////////////////////////////////////////////////////////////////////////
    void initRDataBox();
    void renderBox();

    ////////////////////////////////////////////////////////////////////////////////
    struct RDataParticle
    {
        SharedPtr<ShaderProgram> shader               = nullptr;
        UniquePtr<OpenGLBuffer>  buffPosition         = nullptr;
        UniquePtr<OpenGLBuffer>  buffAnisotropyMatrix = nullptr;
        UniquePtr<OpenGLBuffer>  buffColorData        = nullptr;
        UniquePtr<Material>      material             = nullptr;

        GLuint VAO;
        GLint  v_Position;
        GLint  v_AnisotropyMatrix0;
        GLint  v_AnisotropyMatrix1;
        GLint  v_AnisotropyMatrix2;
        GLint  v_Color;
        GLuint ub_CamData;
        GLuint ub_Light;
        GLuint ub_Material;
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
        GLint hasVColor           = 1;
        GLint pColorMode          = ParticleColorMode::Random;
        GLint useAnisotropyKernel = 1;
        bool  initialized         = false;
    } m_RDataParticle;

    SharedPtr<ParticleSerialization> m_ParticleReaderObj = std::make_shared<ParticleSerialization>();

    void initRDataParticle();
    void initFluidVAOs();
    void uploadParticleColorData();
    void renderParticles();

    ////////////////////////////////////////////////////////////////////////////////
    glm::vec4 m_ClipPlane            = DEFAULT_CLIP_PLANE;
    int       m_NumSimMeshes         = 0;
    int       m_NumBoundaryMeshes    = 0;
    float     m_ShadowIntensity      = 1.0;
    bool      m_bShadowEnabled       = false;
    bool      m_bVisualizeShadow     = false;
    bool      m_bExrportFrameToImage = false;

    ////////////////////////////////////////////////////////////////////////////////
    Vector<SharedPtr<ShaderProgram> > m_ExternalShaders;
    Vector<SharedPtr<MeshObject> >    m_SimMeshObjs;
    Vector<SharedPtr<MeshObject> >    m_BoundaryMeshObjs;
    SharedPtr<PointLights>            m_Lights;

    Vector<UniquePtr<MeshRender> > m_SimMeshRenders;
    Vector<UniquePtr<MeshRender> > m_BoundaryMeshRenders;
    UniquePtr<SkyBoxRender>        m_SkyBoxRender       = nullptr;
    UniquePtr<PlaneRender>         m_PlaneRender        = nullptr;
    UniquePtr<PointLightRender>    m_LightRender        = nullptr;
    UniquePtr<WireFrameBoxRender>  m_WireFrameBoxRender = nullptr;
};