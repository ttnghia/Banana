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
    Lights() : m_NumActiveLights(1)
    {
        static_assert(sizeof(glm::vec4) == sizeof(GLfloat) * 4, "Size of glm::vec4 != 4 * sizeof(GLfloat).");
    }

    Lights(int numLights) : m_NumActiveLights(numLights)
    {
        static_assert(sizeof(glm::vec4) == sizeof(GLfloat) * 4, "Size of glm::vec4 != 4 * sizeof(GLfloat).");
    }

    void setNumLights(int numLights);
    int  getNumLights() const;
    void createUniformBuffer();

    virtual void setLightAmbient(const glm::vec4& ambient, int lightID = 0) = 0;
    virtual void setLightDiffuse(const glm::vec4& diffuse, int lightID = 0) = 0;
    virtual void setLightSpecular(const glm::vec4& specular, int lightID = 0) = 0;

    virtual glm::vec4 getLightAmbient(int lightID  = 0) const = 0;
    virtual glm::vec4 getLightDiffuse(int lightID  = 0) const = 0;
    virtual glm::vec4 getLightSpecular(int lightID = 0) const = 0;

    virtual void uploadLightAmbient(int lightID  = 0) = 0;
    virtual void uploadLightDiffuse(int lightID  = 0) = 0;
    virtual void uploadLightSpecular(int lightID = 0) = 0;

    void   bindUniformBuffer();
    GLuint getBufferBindingPoint();
    size_t getUniformBufferSize() const;
    size_t getLightDataSize() const;

    // data for shadow map
    void   setSceneCenter(const glm::vec3& sceneCenter);
    void   bindUniformBufferLightMatrix();
    GLuint getBufferLightMatrixBindingPoint();
    void   uploadLightMatrixToGPU();

    ////////////////////////////////////////////////////////////////////////////////
    virtual void   updateLightMatrixBuffer() = 0;
    virtual void   uploadDataToGPU()         = 0;
    virtual size_t getLightSize() const      = 0;

protected:
    struct LightMatrix
    {
        LightMatrix()
        {
            setViewMatrix(glm::mat4(1));
            setProjectionMatrix(glm::mat4(1));
        }

        void setViewMatrix(const glm::mat4& viewMatrix)
        {
            const GLfloat* ptr = glm::value_ptr(viewMatrix);
            memcpy(lightViewMatrix, ptr, sizeof(GLfloat) * 16);
        }

        void setProjectionMatrix(const glm::mat4& prjMatrix)
        {
            const GLfloat* ptr = glm::value_ptr(prjMatrix);
            memcpy(lightPrjMatrix, ptr, sizeof(GLfloat) * 16);
        }

        GLfloat lightViewMatrix[16];
        GLfloat lightPrjMatrix[16];
    };

    GLint        m_NumActiveLights;
    OpenGLBuffer m_UniformBuffer;
    OpenGLBuffer m_UniformBufferLightMatrix;
    LightMatrix  m_LightMatrices[MAX_NUM_LIGHTS];
    glm::vec3    m_SceneCenter;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DirectionalLights : public Lights
{
public:
    DirectionalLights() {}

    struct DirectionalLightData
    {
        DirectionalLightData()
        {
            setAmbient(glm::vec4(1));
            setDiffuse(glm::vec4(1));
            setSpecular(glm::vec4(1));
            setDirection(glm::vec4(-1));
        }

        void setAmbient(const glm::vec4& ambient_)
        {
            for(int i = 0; i < 4; ++i) {
                ambient[i] = ambient_[i];
            }
        }

        void setDiffuse(const glm::vec4& diffuse_)
        {
            for(int i = 0; i < 4; ++i) {
                diffuse[i] = diffuse_[i];
            }
        }

        void setSpecular(const glm::vec4& specular_)
        {
            for(int i = 0; i < 4; ++i) {
                specular[i] = specular_[i];
            }
        }

        void setDirection(const glm::vec4& direction_)
        {
            for(int i = 0; i < 4; ++i) {
                direction[i] = direction_[i];
            }
        }

        GLfloat ambient[4];
        GLfloat diffuse[4];
        GLfloat specular[4];
        GLfloat direction[4];
    };

    void                 setLight(const DirectionalLightData& lightData, int lightID = 0);
    DirectionalLightData getLight(int lightID = 0) const;

    void      setLightDirection(const glm::vec4& direction, int lightID = 0);
    glm::vec4 getLightDirection(int lightID    = 0) const;
    void      uploadLightDirection(int lightID = 0);

    virtual void setLightAmbient(const glm::vec4& ambient, int lightID = 0) override;
    virtual void setLightDiffuse(const glm::vec4& diffuse, int lightID = 0) override;
    virtual void setLightSpecular(const glm::vec4& specular, int lightID = 0) override;

    virtual glm::vec4 getLightAmbient(int lightID  = 0) const override;
    virtual glm::vec4 getLightDiffuse(int lightID  = 0) const override;
    virtual glm::vec4 getLightSpecular(int lightID = 0) const override;

    virtual void uploadLightAmbient(int lightID  = 0) override;
    virtual void uploadLightDiffuse(int lightID  = 0) override;
    virtual void uploadLightSpecular(int lightID = 0) override;

    virtual void   uploadDataToGPU() override;
    virtual size_t getLightSize() const override
    {
        return 4 * sizeof(glm::vec4);
    }

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
    PointLights() {}

    struct PointLightData
    {
        PointLightData()
        {
            setAmbient(glm::vec4(1));
            setDiffuse(glm::vec4(1));
            setSpecular(glm::vec4(1));
            setPosition(glm::vec4(-1));
        }

        void setAmbient(const glm::vec4& ambient_)
        {
            for(int i = 0; i < 4; ++i) {
                ambient[i] = ambient_[i];
            }
        }

        void setDiffuse(const glm::vec4& diffuse_)
        {
            for(int i = 0; i < 4; ++i) {
                diffuse[i] = diffuse_[i];
            }
        }

        void setSpecular(const glm::vec4& specular_)
        {
            for(int i = 0; i < 4; ++i) {
                specular[i] = specular_[i];
            }
        }

        void setPosition(const glm::vec4& position_)
        {
            for(int i = 0; i < 4; ++i) {
                position[i] = position_[i];
            }
        }

        GLfloat ambient[4];
        GLfloat diffuse[4];
        GLfloat specular[4];
        GLfloat position[4];
    };

    virtual void   setLight(const PointLightData& lightData, int lightID = 0);
    PointLightData getLight(int lightID) const;

    void      setLightPosition(const glm::vec4& direction, int lightID = 0);
    glm::vec4 getLightPosition(int lightID    = 0) const;
    void      uploadLightPosition(int lightID = 0);

    virtual void setLightAmbient(const glm::vec4& ambient, int lightID = 0) override;
    virtual void setLightDiffuse(const glm::vec4& diffuse, int lightID = 0) override;
    virtual void setLightSpecular(const glm::vec4& specular, int lightID = 0) override;

    virtual glm::vec4 getLightAmbient(int lightID  = 0) const override;
    virtual glm::vec4 getLightDiffuse(int lightID  = 0) const override;
    virtual glm::vec4 getLightSpecular(int lightID = 0) const override;

    virtual void uploadLightAmbient(int lightID  = 0) override;
    virtual void uploadLightDiffuse(int lightID  = 0) override;
    virtual void uploadLightSpecular(int lightID = 0) override;

    virtual void   uploadDataToGPU() override;
    virtual size_t getLightSize() const override
    {
        return 4 * sizeof(glm::vec4);
    }

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
    SpotLights() {}

    struct SpotLightData
    {
        SpotLightData()
        {
            setAmbient(glm::vec4(1));
            setDiffuse(glm::vec4(1));
            setSpecular(glm::vec4(1));
            setPosition(glm::vec4(10));
            setDirection(glm::vec4(-1));

            innerCutOffAngle = M_PI;
            outerCutOffAngle = M_PI;
        }

        void setAmbient(const glm::vec4& ambient_)
        {
            for(int i = 0; i < 4; ++i) {
                ambient[i] = ambient_[i];
            }
        }

        void setDiffuse(const glm::vec4& diffuse_)
        {
            for(int i = 0; i < 4; ++i) {
                diffuse[i] = diffuse_[i];
            }
        }

        void setSpecular(const glm::vec4& specular_)
        {
            for(int i = 0; i < 4; ++i) {
                specular[i] = specular_[i];
            }
        }

        void setPosition(const glm::vec4& position_)
        {
            for(int i = 0; i < 4; ++i) {
                position[i] = position_[i];
            }
        }

        void setDirection(const glm::vec4& direction_)
        {
            for(int i = 0; i < 4; ++i) {
                direction[i] = direction_[i];
            }
        }

        GLfloat ambient[4];
        GLfloat diffuse[4];
        GLfloat specular[4];
        GLfloat position[4];
        GLfloat direction[4];
        GLfloat innerCutOffAngle;
        GLfloat outerCutOffAngle;
    };

    void          setLight(const SpotLightData& lightData, int lightID = 0);
    SpotLightData getLight(int lightID = 0);

    void      setLightDirection(const glm::vec4& direction, int lightID = 0);
    void      setLightPosition(const glm::vec4& position, int lightID = 0);
    void      setLightCuffOffAngles(float innerAngle, float outerAngle, int lightID = 0);
    glm::vec4 getLightPosition(int lightID        = 0) const;
    glm::vec4 getLightDirection(int lightID       = 0) const;
    float     getInnerCutOffAngle(int lightID     = 0) const;
    float     getOuterCutOffAngle(int lightID     = 0) const;
    void      uploadLightPosition(int lightID     = 0);
    void      uploadLightDirection(int lightID    = 0);
    void      uploadLightCutOffAngles(int lightID = 0);

    virtual void setLightAmbient(const glm::vec4& ambient, int lightID = 0) override;
    virtual void setLightDiffuse(const glm::vec4& diffuse, int lightID = 0) override;
    virtual void setLightSpecular(const glm::vec4& specular, int lightID = 0) override;

    virtual glm::vec4 getLightAmbient(int lightID  = 0) const override;
    virtual glm::vec4 getLightDiffuse(int lightID  = 0) const override;
    virtual glm::vec4 getLightSpecular(int lightID = 0) const override;

    virtual void uploadLightAmbient(int lightID  = 0) override;
    virtual void uploadLightDiffuse(int lightID  = 0) override;
    virtual void uploadLightSpecular(int lightID = 0) override;

    virtual void   uploadDataToGPU() override;
    virtual size_t getLightSize() const override
    {
        return 5 * sizeof(glm::vec4) + 2 * sizeof(GLfloat);
    }

    // shadow map helpers
    virtual void updateLightMatrixBuffer() override;
    void         setLightViewPerspective(GLfloat fov, GLfloat asspect = 1.0, GLfloat nearZ = 0.1f, GLfloat farZ = 1000.0f);

private:
    SpotLightData m_Lights[MAX_NUM_LIGHTS];
    GLfloat       m_ShadowFOV;
    GLfloat       m_ShadowAspect;
    GLfloat       m_ShadowNearZ;
    GLfloat       m_ShadowFarZ;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana