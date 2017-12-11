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

    void setCamera(const Vec3f& cameraPosition, const Vec3f& cameraFocus);
    void setBox(const Vec3f& boxMin, const Vec3f& boxMax);

    const std::shared_ptr<ParticleSystemData>& getParticleDataObj() const;

protected:
    virtual void initOpenGL();
    virtual void resizeOpenGLWindow(int, int);
    virtual void renderOpenGL();

public slots:
    void updateParticleData();

    void setNegativeParticleMaterial(const Material::MaterialData& material);
    void setPositiveParticleMaterial(const Material::MaterialData& material);
    void setNegativeParticleSize(int sizeScale);
    void setPositiveParticleSize(int sizeScale);
    void hideNegativeParticles(bool bHide);
    void hidePositiveParticles(bool bHide);

private:
    struct RDataParticle
    {
        std::shared_ptr<QtAppShaderProgram> shader                   = nullptr;
        std::unique_ptr<OpenGLBuffer>       buffPosition             = nullptr;
        std::unique_ptr<OpenGLBuffer>       buffColorScale           = nullptr;
        std::unique_ptr<Material>           negativeParticleMaterial = nullptr;
        std::unique_ptr<Material>           positiveParticleMaterial = nullptr;

        GLuint VAO;
        GLint  v_Position;
        GLint  v_ColorScale;
        GLuint ub_CamData;
        GLuint ub_Light;
        GLuint ub_Material;
        GLuint u_PointRadius;
        GLuint u_PointScale;
        GLuint u_IsPointView;
        GLuint u_ClipPlane;

        GLuint  numNegativeParticles;
        GLuint  numPositiveParticles;
        GLfloat negativePointRadius;
        GLfloat positivePointRadius;
        GLfloat pointScale;

        bool hideNegativeParticles = false;
        bool hidePositveParticles  = false;

        GLint isPointView = 0;
        bool  initialized = false;
    } m_RDataParticle;

    std::shared_ptr<ParticleSystemData> m_ParticleData = std::make_shared<ParticleSystemData>();

    void initRDataParticle();
    void initParticleVAOs();
    void renderParticles();
    void initParticleDataObj();

    GLfloat m_NegativeParticleSizeScale = 0.7f;
    GLfloat m_PositveParticleSizeScale  = 0.1f;
};