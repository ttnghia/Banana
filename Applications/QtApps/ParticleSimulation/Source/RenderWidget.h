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

#pragma once

#include "Common.h"

#include <Banana/Data/ParticleSystemData.h>

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

#include <map>
#include <memory>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RenderWidget : public OpenGLWidget
{
    Q_OBJECT

public:
    RenderWidget(QWidget* parent = 0);
    void setParticlePositions(Vec_Vec3<float>* positions) { m_ParticlePositions = positions; }
    void setCamera(const glm::vec3& cameraPosition, const glm::vec3& cameraFocus) { m_Camera->setDefaultCamera(cameraPosition, cameraFocus, glm::vec3(0, 1, 0)); }
    void setBox(const glm::vec3& boxMin, const glm::vec3& boxMax) { makeCurrent(); m_WireFrameBoxRender->setBox(boxMin, boxMax); doneCurrent(); }

    const std::shared_ptr<ParticleSystemData>& getParticleDataObj() const { return m_ParticleData; }

protected:
    virtual void initOpenGL();
    virtual void resizeOpenGLWindow(int, int);
    virtual void renderOpenGL();

public slots:
    void updateParticleData();
    void updateLights();

    void setSkyBoxTexture(int texIndex);
    void setFloorTexture(int texIndex);
    void setFloorSize(int size);
    void setFloorExposure(int percentage);
    void setParticleColorMode(int colorMode);
    void setParticleMaterial(const Material::MaterialData& material);

    void reloadTextures();
    void enableClipPlane(bool bEnable = true);
    void setClipPlane(const glm::vec4& clipPlane);

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
        std::shared_ptr<QtAppShaderProgram> shader          = nullptr;
        std::unique_ptr<OpenGLBuffer>       buffPosition    = nullptr;
        std::unique_ptr<OpenGLBuffer>       buffColorRandom = nullptr;
        std::unique_ptr<OpenGLBuffer>       buffColorRamp   = nullptr;
        std::unique_ptr<Material>           material        = nullptr;

        GLuint VAO;
        GLint  v_Position;
        GLint  v_Color;
        GLuint ub_CamData;
        GLuint ub_Light;
        GLuint ub_Material;
        GLuint u_PointRadius;
        GLuint u_PointScale;
        GLuint u_IsPointView;
        GLuint u_HasVColor;
        GLuint u_ClipPlane;

        GLuint  nParticles = 0;
        GLfloat pointRadius;
        GLfloat pointScale;

        GLint isPointView = 0;
        GLint hasVColor   = 1;
        GLint pColorMode  = ParticleColorMode::Ramp;
        bool  initialized = false;
    } m_RDataParticle;

    std::shared_ptr<ParticleSystemData> m_ParticleData      = std::make_shared<ParticleSystemData>();
    Vec_Vec3<float>*                    m_ParticlePositions = nullptr;

    void initRDataParticle();
    void initFluidVAOs();
    void uploadParticleColorData();
    void renderParticles();
    void initParticleDataObj();
    void initCaptureDir();

    ////////////////////////////////////////////////////////////////////////////////
    glm::vec4 m_ClipPlane = DEFAULT_CLIP_PLANE;

    std::vector<std::shared_ptr<ShaderProgram> > m_ExternalShaders;
    std::shared_ptr<PointLights>                 m_Lights;

    std::unique_ptr<SkyBoxRender>       m_SkyBoxRender       = nullptr;
    std::unique_ptr<PlaneRender>        m_PlaneRender        = nullptr;
    std::unique_ptr<PointLightRender>   m_LightRender        = nullptr;
    std::unique_ptr<WireFrameBoxRender> m_WireFrameBoxRender = nullptr;
};