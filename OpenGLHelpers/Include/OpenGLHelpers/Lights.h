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

#include <OpenGLHelpers/OpenGLMacros.h>
#include <OpenGLHelpers/OpenGLBuffer.h>

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
    int getNumLights() const;
    void createUniformBuffer();

    virtual void setLightAmbient(const glm::vec4& ambient, int lightID   = 0) = 0;
    virtual void setLightDiffuse(const glm::vec4& diffuse, int lightID   = 0) = 0;
    virtual void setLightSpecular(const glm::vec4& specular, int lightID = 0) = 0;

    virtual glm::vec4 getLightAmbient(int lightID  = 0) const = 0;
    virtual glm::vec4 getLightDiffuse(int lightID  = 0) const = 0;
    virtual glm::vec4 getLightSpecular(int lightID = 0) const = 0;

    virtual void uploadLightAmbient(int lightID  = 0) = 0;
    virtual void uploadLightDiffuse(int lightID  = 0) = 0;
    virtual void uploadLightSpecular(int lightID = 0) = 0;

    void bindUniformBuffer();
    GLuint getBufferBindingPoint();
    size_t getUniformBufferSize() const;
    size_t getLightDataSize() const;

    ////////////////////////////////////////////////////////////////////////////////
    virtual void uploadDataToGPU()              = 0;
    virtual size_t getLightSize() const         = 0;

protected:
    GLint          m_NumActiveLights;
    OpenGLBuffer   m_UniformBuffer;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DirectionalLights : public Lights
{
public:
    DirectionalLights()
    {}

    struct DirectionalLightData
    {
        DirectionalLightData()
        {
            setAmbient(glm::vec4(0.2));
            setDiffuse(glm::vec4(1));
            setSpecular(glm::vec4(1));
            setDirection(glm::vec4(-1));
        }

        void setAmbient(const glm::vec4& ambient_)
        {
            for(int i = 0; i < 4; ++i)
                ambient[i] = ambient_[i];
        }
        void setDiffuse(const glm::vec4& diffuse_)
        {
            for(int i = 0; i < 4; ++i)
                diffuse[i] = diffuse_[i];
        }
        void setSpecular(const glm::vec4& specular_)
        {
            for(int i = 0; i < 4; ++i)
                specular[i] = specular_[i];
        }
        void setDirection(const glm::vec4& direction_)
        {
            for(int i = 0; i < 4; ++i)
                direction[i] = direction_[i];
        }

        GLfloat ambient[4];
        GLfloat diffuse[4];
        GLfloat specular[4];
        GLfloat direction[4];
    };

    void setLight(const DirectionalLightData& lightData, int lightID = 0);
    DirectionalLightData getLight(int lightID = 0) const;

    void setLightDirection(const glm::vec4& direction, int lightID = 0);
    glm::vec4 getLightDirection(int lightID = 0) const;
    void uploadLightDirection(int lightID = 0);

    virtual void setLightAmbient(const glm::vec4& ambient, int lightID = 0) override;
    virtual void setLightDiffuse(const glm::vec4& diffuse, int lightID = 0) override;
    virtual void setLightSpecular(const glm::vec4& specular, int lightID = 0) override;

    virtual glm::vec4 getLightAmbient(int lightID = 0) const override;
    virtual glm::vec4 getLightDiffuse(int lightID = 0) const override;
    virtual glm::vec4 getLightSpecular(int lightID = 0) const override;

    virtual void uploadLightAmbient(int lightID = 0) override;
    virtual void uploadLightDiffuse(int lightID = 0) override;
    virtual void uploadLightSpecular(int lightID = 0) override;

    virtual void uploadDataToGPU() override;
    virtual size_t getLightSize() const override
    {
        return 4 * sizeof(glm::vec4);
    }

private:
    DirectionalLightData m_Lights[MAX_NUM_LIGHTS];
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PointLights : public Lights
{
public:
    PointLights()
    {}

    struct PointLightData
    {
        PointLightData()
        {
            setAmbient(glm::vec4(0.2));
            setDiffuse(glm::vec4(1));
            setSpecular(glm::vec4(1));
            setPosition(glm::vec4(-1));
        }

        void setAmbient(const glm::vec4& ambient_)
        {
            for(int i = 0; i < 4; ++i)
                ambient[i] = ambient_[i];
        }
        void setDiffuse(const glm::vec4& diffuse_)
        {
            for(int i = 0; i < 4; ++i)
                diffuse[i] = diffuse_[i];
        }
        void setSpecular(const glm::vec4& specular_)
        {
            for(int i = 0; i < 4; ++i)
                specular[i] = specular_[i];
        }
        void setPosition(const glm::vec4& position_)
        {
            for(int i = 0; i < 4; ++i)
                position[i] = position_[i];
        }

        GLfloat ambient[4];
        GLfloat diffuse[4];
        GLfloat specular[4];
        GLfloat position[4];
    };

    virtual void setLight(const PointLightData& lightData, int lightID = 0);
    PointLightData getLight(int lightID) const;

    void setLightPosition(const glm::vec4& direction, int lightID = 0);
    glm::vec4 getLightPosition(int lightID = 0) const;
    void uploadLightPosition(int lightID = 0);

    virtual void setLightAmbient(const glm::vec4& ambient, int lightID = 0) override;
    virtual void setLightDiffuse(const glm::vec4& diffuse, int lightID = 0) override;
    virtual void setLightSpecular(const glm::vec4& specular, int lightID = 0) override;

    virtual glm::vec4 getLightAmbient(int lightID = 0) const override;
    virtual glm::vec4 getLightDiffuse(int lightID = 0) const override;
    virtual glm::vec4 getLightSpecular(int lightID = 0) const override;

    virtual void uploadLightAmbient(int lightID = 0) override;
    virtual void uploadLightDiffuse(int lightID = 0) override;
    virtual void uploadLightSpecular(int lightID = 0) override;

    virtual void uploadDataToGPU() override;
    virtual size_t getLightSize() const override
    {
        return 4 * sizeof(glm::vec4);
    }

private:
    PointLightData m_Lights[MAX_NUM_LIGHTS];
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class SpotLights : public Lights
{
public:
    SpotLights()
    {}

    struct SpotLightData
    {
        SpotLightData()
        {
            setAmbient(glm::vec4(0.2));
            setDiffuse(glm::vec4(1));
            setSpecular(glm::vec4(1));
            setPosition(glm::vec4(10));
            setDirection(glm::vec4(-1));

            innerCutOffAngle = M_PI;
            outerCutOffAngle = M_PI;
        }

        void setAmbient(const glm::vec4& ambient_)
        {
            for(int i = 0; i < 4; ++i)
                ambient[i] = ambient_[i];
        }
        void setDiffuse(const glm::vec4& diffuse_)
        {
            for(int i = 0; i < 4; ++i)
                diffuse[i] = diffuse_[i];
        }
        void setSpecular(const glm::vec4& specular_)
        {
            for(int i = 0; i < 4; ++i)
                specular[i] = specular_[i];
        }
        void setPosition(const glm::vec4& position_)
        {
            for(int i = 0; i < 4; ++i)
                position[i] = position_[i];
        }
        void setDirection(const glm::vec4& direction_)
        {
            for(int i = 0; i < 4; ++i)
                direction[i] = direction_[i];
        }

        GLfloat ambient[4];
        GLfloat diffuse[4];
        GLfloat specular[4];
        GLfloat position[4];
        GLfloat direction[4];
        GLfloat innerCutOffAngle;
        GLfloat outerCutOffAngle;
    };

    void setLight(const SpotLightData& lightData, int lightID = 0);
    SpotLightData getLight(int lightID = 0);

    void setLightDirection(const glm::vec4& direction, int lightID = 0);
    void setLightPosition(const glm::vec4& position, int lightID = 0);
    void setLightCuffOffAngles(float innerAngle, float outerAngle, int lightID = 0);
    glm::vec4 getLightPosition(int lightID = 0) const;
    glm::vec4 getLightDirection(int lightID = 0) const;
    float getInnerCutOffAngle(int lightID = 0) const;
    float getOuterCutOffAngle(int lightID = 0) const;
    void uploadLightPosition(int lightID = 0);
    void uploadLightDirection(int lightID = 0);
    void uploadLightCutOffAngles(int lightID = 0);

    virtual void setLightAmbient(const glm::vec4& ambient, int lightID = 0) override;
    virtual void setLightDiffuse(const glm::vec4& diffuse, int lightID = 0) override;
    virtual void setLightSpecular(const glm::vec4& specular, int lightID = 0) override;

    virtual glm::vec4 getLightAmbient(int lightID = 0) const override;
    virtual glm::vec4 getLightDiffuse(int lightID = 0) const override;
    virtual glm::vec4 getLightSpecular(int lightID = 0) const override;

    virtual void uploadLightAmbient(int lightID = 0) override;
    virtual void uploadLightDiffuse(int lightID = 0) override;
    virtual void uploadLightSpecular(int lightID = 0) override;

    virtual void uploadDataToGPU() override;
    virtual size_t getLightSize() const override
    {
        return 5 * sizeof(glm::vec4) + 2 * sizeof(GLfloat);
    }

private:
    SpotLightData m_Lights[MAX_NUM_LIGHTS];
};