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
#include "FRRenderObjects.h"

#include <Banana/Data/ParticleSystemData.h>

#include <OpenGLHelpers/ShaderProgram.h>
#include <OpenGLHelpers/OpenGLBuffer.h>
#include <OpenGLHelpers/OpenGLTexture.h>

#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/OpenGLWidget.h>

#include <map>
#include <memory>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RenderWidget : public OpenGLWidget
{
    Q_OBJECT

public:

    RenderWidget(QWidget* parent = 0);

    const auto& getParticleDataObj() const { return m_ParticleData; }
    const auto& getSimMeshObjs() const { return m_SimMeshObjs; }
    const auto& getBoundaryMeshObjs() const { return m_BoundaryMeshObjs; }

    int  getNSimMeshes() const { return m_NumSimMeshes; }
    int  getNBoundaryMeshes() const { return m_NumBoundaryMeshes; }
    void setCamera(const glm::vec3& cameraPosition, const glm::vec3& cameraFocus);
    void setBox(const glm::vec3& boxMin, const glm::vec3& boxMax);

protected:
    virtual void initOpenGL() override;
    virtual void resizeOpenGLWindow(int, int) override {}
    virtual void renderOpenGL() override;

public slots:
    void updateParticleData();
    void updateSimMeshes();
    void updateBoundaryMeshes();
    void updateNumSimMeshes(int numMeshes);
    void updateNumBoundaryMeshes(int numMeshes);
    void updateLights();

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
    void lightsObjChanged(const std::shared_ptr<PointLights>& lights);

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
        std::shared_ptr<ShaderProgram> shader               = nullptr;
        std::unique_ptr<OpenGLBuffer>  buffPosition         = nullptr;
        std::unique_ptr<OpenGLBuffer>  buffAnisotropyMatrix = nullptr;
        std::unique_ptr<OpenGLBuffer>  buffColorData        = nullptr;
        std::unique_ptr<Material>      material             = nullptr;

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

    SharedPtr<ParticleSystemData> m_ParticleData = std::make_shared<ParticleSystemData>();


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
    std::vector<std::shared_ptr<ShaderProgram> > m_ExternalShaders;
    std::vector<std::shared_ptr<MeshObject> >    m_SimMeshObjs;
    std::vector<std::shared_ptr<MeshObject> >    m_BoundaryMeshObjs;
    std::shared_ptr<PointLights>                 m_Lights;

    std::vector<std::unique_ptr<FRMeshRender> > m_SimMeshRenders;
    std::vector<std::unique_ptr<FRMeshRender> > m_BoundaryMeshRenders;
    std::unique_ptr<SkyBoxRender>               m_SkyBoxRender       = nullptr;
    std::unique_ptr<FRPlaneRender>              m_PlaneRender        = nullptr;
    std::unique_ptr<PointLightRender>           m_LightRender        = nullptr;
    std::unique_ptr<WireFrameBoxRender>         m_WireFrameBoxRender = nullptr;
};