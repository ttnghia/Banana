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

#include <OpenGLHelpers/OpenGLMacros.h>
#include <OpenGLHelpers/OpenGLBuffer.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define MAX_NUM_LIGHTS 8

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Lights : public OpenGLCallable
{
public:
    Lights() : m_NumActiveLights(1) { static_assert(sizeof(Vec4f) == sizeof(GLfloat) * 4, "Size of Vec4f != 4 * sizeof(GLfloat)."); }
    Lights(int numLights) : m_NumActiveLights(numLights) { static_assert(sizeof(Vec4f) == sizeof(GLfloat) * 4, "Size of Vec4f != 4 * sizeof(GLfloat)."); }

    void setNumLights(int numLights) { m_NumActiveLights = numLights; }
    int  getNumLights() const { return m_NumActiveLights; }
    void createUniformBuffer();

    virtual void setLightAmbient(const Vec4f& ambient, int lightID = 0) = 0;
    virtual void setLightDiffuse(const Vec4f& diffuse, int lightID = 0) = 0;
    virtual void setLightSpecular(const Vec4f& specular, int lightID = 0) = 0;

    virtual const Vec4f& getLightAmbient(int lightID  = 0) const = 0;
    virtual const Vec4f& getLightDiffuse(int lightID  = 0) const = 0;
    virtual const Vec4f& getLightSpecular(int lightID = 0) const = 0;

    void   bindUniformBuffer() { assert(m_UniformBuffer.isCreated()); m_UniformBuffer.bindBufferBase(); }
    GLuint getBufferBindingPoint();
    size_t getUniformBufferSize() const { return MAX_NUM_LIGHTS * getLightSize() + sizeof(GLint); }
    size_t getLightDataSize() const { return MAX_NUM_LIGHTS * getLightSize(); }

    // data for shadow map
    void   setSceneCenter(const Vec3f& sceneCenter) { m_SceneCenter = sceneCenter; }
    void   bindUniformBufferLightMatrix() { assert(m_UniformBufferLightMatrix.isCreated()); m_UniformBufferLightMatrix.bindBufferBase(); }
    GLuint getBufferLightMatrixBindingPoint();
    void   uploadLightMatrixToGPU();

    ////////////////////////////////////////////////////////////////////////////////
    virtual void   updateLightMatrixBuffer() = 0;
    virtual void   uploadDataToGPU()         = 0;
    virtual size_t getLightSize() const      = 0;

protected:
    struct LightMatrix
    {
        Mat4x4f lightViewMatrix       = Mat4x4f(1);
        Mat4x4f lightProjectionMatrix = Mat4x4f(1);
    };

    GLint        m_NumActiveLights;
    OpenGLBuffer m_UniformBuffer;
    OpenGLBuffer m_UniformBufferLightMatrix;
    LightMatrix  m_LightMatrices[MAX_NUM_LIGHTS];
    Vec3f        m_SceneCenter;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DirectionalLights : public Lights
{
public:
    struct DirectionalLightData
    {
        Vec4f ambient   = Vec4f(1.0);
        Vec4f diffuse   = Vec4f(1.0);
        Vec4f specular  = Vec4f(1.0);
        Vec4f direction = Vec4f(1.0);
    };

    void        setLight(const DirectionalLightData& lightData, int lightID = 0) { assert(lightID < m_NumActiveLights); m_Lights[lightID] = lightData; }
    const auto& getLight(int lightID = 0) const { assert(lightID < m_NumActiveLights); return m_Lights[lightID]; }

    void        setLightDirection(const Vec4f& direction, int lightID = 0) { assert(lightID < m_NumActiveLights); m_Lights[lightID].direction = direction; }
    const auto& getLightDirection(int lightID = 0) const { assert(lightID < m_NumActiveLights); return m_Lights[lightID].direction; }

    virtual void setLightAmbient(const Vec4f& ambient, int lightID = 0) override { assert(lightID < m_NumActiveLights); m_Lights[lightID].ambient = ambient; }
    virtual void setLightDiffuse(const Vec4f& diffuse, int lightID = 0) override { assert(lightID < m_NumActiveLights); m_Lights[lightID].diffuse = diffuse; }
    virtual void setLightSpecular(const Vec4f& specular, int lightID = 0) override { assert(lightID < m_NumActiveLights); m_Lights[lightID].specular = specular; }

    virtual const Vec4f& getLightAmbient(int lightID  = 0) const override { assert(lightID < m_NumActiveLights); return m_Lights[lightID].ambient; }
    virtual const Vec4f& getLightDiffuse(int lightID  = 0) const override { assert(lightID < m_NumActiveLights); return m_Lights[lightID].diffuse; }
    virtual const Vec4f& getLightSpecular(int lightID = 0) const override { assert(lightID < m_NumActiveLights); return m_Lights[lightID].specular; }

    virtual void   uploadDataToGPU() override;
    virtual size_t getLightSize() const override { return 4 * sizeof(Vec4f); }

    // shadow map helpers
    virtual void updateLightMatrixBuffer() override;
    void         setShadowMapBox(GLfloat minX, GLfloat maxX, GLfloat minY, GLfloat maxY, GLfloat minZ, GLfloat maxZ);

private:
    DirectionalLightData m_Lights[MAX_NUM_LIGHTS];

    GLfloat m_ShadowMinX;
    GLfloat m_ShadowMaxX;
    GLfloat m_ShadowMinY;
    GLfloat m_ShadowMaxY;
    GLfloat m_ShadowMinZ;
    GLfloat m_ShadowMaxZ;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PointLights : public Lights
{
public:
    struct PointLightData
    {
        Vec4f ambient  = Vec4f(1.0);
        Vec4f diffuse  = Vec4f(1.0);
        Vec4f specular = Vec4f(1.0);
        Vec4f position = Vec4f(1.0);
    };

    virtual void setLight(const PointLightData& lightData, int lightID = 0) { assert(lightID < m_NumActiveLights); m_Lights[lightID] = lightData; }
    const auto&  getLight(int lightID) const { assert(lightID < m_NumActiveLights); return m_Lights[lightID]; }

    void        setLightPosition(const Vec4f& position, int lightID = 0) { assert(lightID < m_NumActiveLights); m_Lights[lightID].position = position; }
    const auto& getLightPosition(int lightID = 0) const { assert(lightID < m_NumActiveLights); return m_Lights[lightID].position; }

    virtual void setLightAmbient(const Vec4f& ambient, int lightID = 0) override { assert(lightID < m_NumActiveLights); m_Lights[lightID].ambient = ambient; }
    virtual void setLightDiffuse(const Vec4f& diffuse, int lightID = 0) override { assert(lightID < m_NumActiveLights); m_Lights[lightID].diffuse = diffuse; }
    virtual void setLightSpecular(const Vec4f& specular, int lightID = 0) override { assert(lightID < m_NumActiveLights); m_Lights[lightID].specular = specular; }

    virtual const Vec4f& getLightAmbient(int lightID  = 0) const override { assert(lightID < m_NumActiveLights); return m_Lights[lightID].ambient; }
    virtual const Vec4f& getLightDiffuse(int lightID  = 0) const override { assert(lightID < m_NumActiveLights); return m_Lights[lightID].diffuse; }
    virtual const Vec4f& getLightSpecular(int lightID = 0) const override { assert(lightID < m_NumActiveLights); return m_Lights[lightID].specular; }

    virtual void   uploadDataToGPU() override;
    virtual size_t getLightSize() const override { return 4 * sizeof(Vec4f); }

    // shadow map helpers
    virtual void updateLightMatrixBuffer() override;
    void         setLightViewPerspective(GLfloat fov, GLfloat asspect = 1.0, GLfloat nearZ = 0.1f, GLfloat farZ = 1000.0f);

private:
    PointLightData m_Lights[MAX_NUM_LIGHTS];
    GLfloat        m_ShadowFOV;
    GLfloat        m_ShadowAspect;
    GLfloat        m_ShadowNearZ;
    GLfloat        m_ShadowFarZ;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class SpotLights : public Lights
{
public:
    struct SpotLightData
    {
        Vec4f   ambient;
        Vec4f   diffuse;
        Vec4f   specular;
        Vec4f   position;
        Vec4f   direction;
        GLfloat innerCutOffAngle;
        GLfloat outerCutOffAngle;
    };

    void        setLight(const SpotLightData& lightData, int lightID = 0) { assert(lightID < m_NumActiveLights); m_Lights[lightID] = lightData; }
    const auto& getLight(int lightID = 0) { assert(lightID < m_NumActiveLights); return m_Lights[lightID]; }

    void setLightPosition(const Vec4f& position, int lightID = 0) { assert(lightID < m_NumActiveLights); m_Lights[lightID].position = position; }
    void setLightDirection(const Vec4f& direction, int lightID = 0) { assert(lightID < m_NumActiveLights); m_Lights[lightID].direction = direction; }
    void setLightCuffOffAngles(float innerAngle, float outerAngle, int lightID = 0);

    const auto& getLightPosition(int lightID    = 0) const { assert(lightID < m_NumActiveLights); return m_Lights[lightID].position; }
    const auto& getLightDirection(int lightID   = 0) const { assert(lightID < m_NumActiveLights); return m_Lights[lightID].direction; }
    auto        getInnerCutOffAngle(int lightID = 0) const { assert(lightID < m_NumActiveLights); return m_Lights[lightID].innerCutOffAngle; }
    auto        getOuterCutOffAngle(int lightID = 0) const { assert(lightID < m_NumActiveLights); return m_Lights[lightID].outerCutOffAngle; }

    virtual void setLightAmbient(const Vec4f& ambient, int lightID = 0) override { assert(lightID < m_NumActiveLights); m_Lights[lightID].ambient = ambient; }
    virtual void setLightDiffuse(const Vec4f& diffuse, int lightID = 0) override { assert(lightID < m_NumActiveLights); m_Lights[lightID].diffuse = diffuse; }
    virtual void setLightSpecular(const Vec4f& specular, int lightID = 0) override { assert(lightID < m_NumActiveLights); m_Lights[lightID].specular = specular; }

    virtual const Vec4f& getLightAmbient(int lightID  = 0) const override { assert(lightID < m_NumActiveLights); return m_Lights[lightID].ambient; }
    virtual const Vec4f& getLightDiffuse(int lightID  = 0) const override { assert(lightID < m_NumActiveLights); return m_Lights[lightID].diffuse; }
    virtual const Vec4f& getLightSpecular(int lightID = 0) const override { assert(lightID < m_NumActiveLights); return m_Lights[lightID].specular; }

    virtual void   uploadDataToGPU() override;
    virtual size_t getLightSize() const override { return 5 * sizeof(Vec4f) + 2 * sizeof(GLfloat); }

    // shadow map helpers
    virtual void updateLightMatrixBuffer() override;
    void         setLightViewPerspective(GLfloat fov, GLfloat asspect = 1.0f, GLfloat nearZ = 0.1f, GLfloat farZ = 1000.0f);

private:
    SpotLightData m_Lights[MAX_NUM_LIGHTS];
    GLfloat       m_ShadowFOV;
    GLfloat       m_ShadowAspect;
    GLfloat       m_ShadowNearZ;
    GLfloat       m_ShadowFarZ;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana