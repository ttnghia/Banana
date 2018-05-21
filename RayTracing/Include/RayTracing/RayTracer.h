//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

#define NOMINMAX // required for optix

#include <optixu/optixpp_namespace.h>
#include <optixu/optixu_aabb_namespace.h>
#include <optixu/optixu_math_stream_namespace.h>

#include <Banana/Setup.h>
#include <OpenGLHelpers/Camera.h>
#include <OpenGLHelpers/OpenGLTexture.h>
#include <OpenGLHelpers/OpenGLMacros.h>

#include <memory>
#include <map>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RayTracer : public OpenGLCallable
{
public:
    RayTracer(const std::shared_ptr<Camera>& camera) : m_Camera(camera) {}
    virtual ~RayTracer() { destroyOptiXContext(); }
    void setCamera(const std::shared_ptr<Camera>& camera) { m_Camera = camera; }
    bool isDone() { return m_bDone; }

    virtual void createOptiXContext(int, int);
    virtual void resizeViewport(int, int);
    virtual void createPrograms() = 0;
    virtual void createScene()    = 0;
    virtual void render()         = 0;

    optix::Buffer         getOptiXOutputBuffer();
    GLuint                getOutputBufferOID() const;
    void                  getOutputAsTexture(const std::shared_ptr<OpenGLTexture>& texture);
    void                  getOutputAsByteArray(std::vector<unsigned char>& data);
    void                  getOutputAsByteArray(unsigned char* data);
    void                  validateContext();
    bool                  updateCamera();
    optix::TextureSampler createTexSampler(const std::string& texFile, bool bUseCache = true);
    optix::TextureSampler getDummyTexSampler(const glm::vec3& defaultColor = glm::vec3(1.0f));

protected:
    void resizeBuffer(optix::Buffer, int, int);
    void destroyOptiXContext();
    void computeEyeUVW(optix::float3&, optix::float3&, optix::float3&, optix::float3&);

    ////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<Camera>                        m_Camera       = nullptr;
    optix::Context                                 m_OptiXContext = 0;
    optix::Buffer                                  m_OutBuffer    = 0;
    std::map<std::string, optix::Material>         m_Materials;
    std::map<std::string, optix::TextureSampler>   m_TexSamplers;
    std::map<std::string, optix::GeometryInstance> m_Geometry;

    unsigned int m_FrameNumber = 0;
    float        m_AspectRatio = 1.0f;
    int          m_Width       = 0;
    int          m_Height      = 0;
    bool         m_bDone       = false;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana