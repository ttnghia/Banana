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
#include "TeapotRayTracer.h"

#include <Banana/Data/ParticleSystemData.h>

#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/OpenGLWidget.h>
#include <QtAppHelpers/EnhancedMessageBox.h>

#include <OpenGLHelpers/Material.h>
#include <OpenGLHelpers/Lights.h>
//#include <OpenGLHelpers/ShaderProgram.h>
//#include <OpenGLHelpers/OpenGLBuffer.h>
#include <OpenGLHelpers/OpenGLTexture.h>
#include <OpenGLHelpers/RenderObjects.h>

#include "CUDA/Common.cuh"
#include "CUDA/Camera.h"
//#include <sutil/OptiXMesh.h>

#include <map>
#include <memory>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RenderWidget : public OpenGLWidget
{
    Q_OBJECT

public:
    RenderWidget(QWidget* parent = 0);

protected:
    virtual void initOpenGL();
    virtual void resizeOpenGLWindow(int, int);
    virtual void renderOpenGL();

public slots:
    void resetCamera();
    void updateLights();
    void setSkyBoxTexture(int texIndex);
    void setFloorTexture(int texIndex);
    void setFloorSize(int size);
    void setFloorTexScale(int scale);
    void setMeshMaterial(const Material::MaterialData& material, int meshID);
    void reloadTextures();
    void saveRenderImage();

signals:
    void lightsObjChanged(const std::shared_ptr<PointLights>& lights);

private:
    void    initRayTracer();
    void    renderRayTracingBuffer();
    void    initCaptureDir();
    int     getLastCaptureIdx();
    QString getCaptureFilePath(int frame);

    ////////////////////////////////////////////////////////////////////////////////
    GLboolean   m_bUseSRGB = GL_FALSE;
    QStringList m_SkyTexFile;
    QStringList m_FloorTexFile;

    std::shared_ptr<PointLights>             m_Lights              = nullptr;
    std::shared_ptr<OpenGLTexture>           m_RenderTexture       = nullptr;
    std::unique_ptr<ScreenQuadTextureRender> m_ScreenQuadTexRender = nullptr;
    std::unique_ptr<TeapotRayTracer>         m_RayTracer           = nullptr;
};