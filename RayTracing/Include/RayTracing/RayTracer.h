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

#define NOMINMAX // required for optix

#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_aabb_namespace.h>
#include <optixu/optixu_math_stream_namespace.h>

#include <Banana/TypeNames.h>
#include <OpenGLHelpers/Camera.h>
#include <OpenGLHelpers/OpenGLTexture.h>
#include <OpenGLHelpers/OpenGLMacros.h>

#include <memory>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RayTracer : public OpenGLCallable
{
public:
    RayTracer(const std::shared_ptr<Camera>& camera) : m_Camera(camera) {}
    virtual ~RayTracer() { destroyOptiXContext(); }
    void setCamera(const std::shared_ptr<Camera>& camera) { m_Camera = camera; }

    virtual void createOptiXContext(int, int);
    virtual void resizeViewport(int, int);
    virtual void createPrograms() = 0;
    virtual void createScene()    = 0;
    virtual void render()         = 0;

    optix::Buffer getOptiXOutputBuffer();
    GLuint        getOutputBufferOID() const;
    void          getOutputAsTexture(const std::shared_ptr<OpenGLTexture>& texture);
    void          getOutputAsByteArray(std::vector<unsigned char>& data);
    void          updateCamera();
    void          validateContext();

protected:
    void resizeBuffer(optix::Buffer, int, int);
    void destroyOptiXContext();
    void computeEyeUVW(optix::float3&, optix::float3&, optix::float3&, optix::float3&);

    ////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<Camera> m_Camera       = nullptr;
    optix::Context          m_OptiXContext = 0;
    optix::Buffer           m_OutBuffer    = 0;

    unsigned int m_FrameNumber   = 0;
    float        m_AspectRatio   = 1.0f;
    int          m_Width         = 0;
    int          m_Height        = 0;
    bool         m_bRenderGround = true;
    bool         m_bRenderSky    = true;
};