//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 10/15/2016
//            `)  ('                                                    Author: Nghia Truong
//         ,  /::::\  ,
//         |'.\::::/.'|
//        _|  ';::;'  |_
//       (::)   ||   (::)                       _.
//        "|    ||    |"                      _(:)
//         '.   ||   .'                       /::\
//           '._||_.'                         \::/
//            /::::\                         /:::\
//            \::::/                        _\:::/
//             /::::\_.._  _.._  _.._  _.._/::::\
//             \::::/::::\/::::\/::::\/::::\::::/
//               `""`\::::/\::::/\::::/\::::/`""`
//                    `""`  `""`  `""`  `""`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <OpenGLHelpers/OpenGLMacros.h>
#include <OpenGLHelpers/OpenGLBuffer.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Light
{
public:
    Light()
    {
        static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3,
                      "Size of glm::vec3 != 3*sizeof(GLfloat).");

        m_BasicData.ambient   = glm::vec3(0, 0, 0);
        m_BasicData.diffuse   = glm::vec3(1, 1, 1);
        m_BasicData.specular  = glm::vec3(1, 1, 1);
        m_BasicData.shininess = 100.0;

        createUniformBuffer();
    }

    void createUniformBuffer()
    {
        size_t bufferSize = getUniformBufferSize();
        m_UniformBuffer.createBuffer(GL_UNIFORM_BUFFER, bufferSize);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void uploadLightAmbient()
    {
        assert(m_UniformBuffer.isCreated());

        m_UniformBuffer.bind();
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat3),
               glm::value_ptr(m_BasicData.ambient)));
        m_UniformBuffer.release();
    }

    void uploadLightDiffuse()
    {
        assert(m_UniformBuffer.isCreated());

        m_UniformBuffer.bind();
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat3), sizeof(glm::mat3),
               glm::value_ptr(m_BasicData.diffuse)));
        m_UniformBuffer.release();
    }

    void uploadLightSpecular()
    {
        assert(m_UniformBuffer.isCreated());

        m_UniformBuffer.bind();
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat3), sizeof(glm::mat3),
               glm::value_ptr(m_BasicData.specular)));
        m_UniformBuffer.release();
    }

    void uploadLightShininess()
    {
        assert(m_UniformBuffer.isCreated());

        m_UniformBuffer.bind();
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat3), sizeof(GLfloat),
               &m_BasicData.shininess));
        m_UniformBuffer.release();
    }

    ////////////////////////////////////////////////////////////////////////////////
    void setLightAmbient(const glm::vec3& ambient)
    {
        m_BasicData.ambient = ambient;
    }
    void setLightDiffuse(const glm::vec3& diffuse)
    {
        m_BasicData.diffuse = diffuse;
    }
    void setLightSpecular(const glm::vec3& specular)
    {
        m_BasicData.specular = specular;
    }
    void setLightShininess(GLfloat shininess)
    {
        m_BasicData.shininess = shininess;
    }

    ////////////////////////////////////////////////////////////////////////////////
    glm::vec3 getLightAmbient() const
    {
        return m_BasicData.ambient;
    }
    glm::vec3 getLightDiffuse() const
    {
        return m_BasicData.diffuse;
    }
    glm::vec3 getLightSpecular() const
    {
        return m_BasicData.specular;
    }
    GLfloat getLightShininess() const
    {
        return m_BasicData.shininess;
    }

    ////////////////////////////////////////////////////////////////////////////////
    virtual void uploadBuffer()           = 0;
    virtual size_t getUniformBufferSize() = 0;

protected:
    struct BasicLightData
    {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        GLfloat shininess;
    };

    void uploadBasicData()
    {
        assert(m_UniformBuffer.isCreated());

        m_UniformBuffer.bind();
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(BasicLightData),
               &m_BasicData));
        m_UniformBuffer.release();
    }

    OpenGLBuffer   m_UniformBuffer;
    BasicLightData m_BasicData;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DirectionalLight : public Light
{
public:
    DirectionalLight() : m_Direction(-1.0, -1.0, -1.0)
    {}

    void setLightDirection(const glm::vec3& direction)
    {
        m_Direction = direction;
    }
    glm::vec3 getLightDirection() const
    {

        return m_Direction;
    }

    ////////////////////////////////////////////////////////////////////////////////
    void uploadLightDirection()
    {
        m_UniformBuffer.bind();
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(BasicLightData),
               sizeof(glm::vec3), glm::value_ptr(m_Direction)));
        m_UniformBuffer.release();
    }

    virtual void uploadBuffer() override
    {
        uploadBasicData();
        uploadLightDirection();
    }

    ////////////////////////////////////////////////////////////////////////////////
    virtual size_t getUniformBufferSize() override
    {
        return static_cast<size_t>(sizeof(BasicLightData) + sizeof(glm::vec3));
    }

private:
    glm::vec3 m_Direction;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/*
Range Constant Linear Quadratic
3250, 1.0, 0.0014, 0.000007
600, 1.0, 0.007, 0.0002
325, 1.0, 0.014, 0.0007
200, 1.0, 0.022, 0.0019
160, 1.0, 0.027, 0.0028
100, 1.0, 0.045, 0.0075
65, 1.0, 0.07, 0.017
50, 1.0, 0.09, 0.032
32, 1.0, 0.14, 0.07
20, 1.0, 0.22, 0.20
13, 1.0, 0.35, 0.44
7, 1.0, 0.7, 1.8
*/
class PointLight : public Light
{
public:
    PointLight() :
        m_Position(10, 10, 10),
        m_isAttennuating(0),
        m_atConstant(1.0),
        m_atLinear(0.7),
        m_atQuadratic(1.8)
    {}

    ////////////////////////////////////////////////////////////////////////////////
    void setLightPosition(const glm::vec3& position)
    {
        m_Position = position;
    }

    void setAttennulation(bool attennuation)
    {
        m_isAttennuating = attennuation ? 1 : 0;
    }

    void setATConstantCoeff(GLfloat atConstant)
    {
        m_atConstant = atConstant;
    }
    void setATLinearCoeff(GLfloat atLinear)
    {
        m_atLinear = atLinear;
    }
    void setATQuadraticCoeff(GLfloat atQuadratic)
    {
        m_atQuadratic = atQuadratic;
    }

    ////////////////////////////////////////////////////////////////////////////////
    glm::vec3 getLightPosition() const
    {
        return m_Position;
    }
    bool isAttennuating() const
    {
        return (m_isAttennuating > 0);
    }
    GLfloat getATConstantCoeff()
    {
        return m_atConstant;
    }
    GLfloat getATLinearCoeff()
    {
        return m_atLinear;
    }
    GLfloat getATQuadraticCoeff()
    {
        return m_atQuadratic;
    }

    ////////////////////////////////////////////////////////////////////////////////
    void uploadLightPosition()
    {
        m_UniformBuffer.bind();
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(BasicLightData),
               sizeof(glm::vec3), glm::value_ptr(m_Position)));
        m_UniformBuffer.release();
    }

    void uploadAttennuationCoeffs()
    {
        size_t offset   = sizeof(BasicLightData) + sizeof(glm::vec3);
        size_t dataSize = sizeof(GLint);

        m_UniformBuffer.bind();
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, offset,
               dataSize, &m_isAttennuating));

        offset  += dataSize;
        dataSize = sizeof(GLfloat);
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, offset,
               dataSize, &m_atConstant));

        offset  += dataSize;
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, offset,
               dataSize, &m_atLinear));

        offset  += dataSize;
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, offset,
               dataSize, &m_atQuadratic));

        m_UniformBuffer.release();
    }

    virtual void uploadBuffer() override
    {
        uploadBasicData();
        uploadAttennuationCoeffs();
    }

    ////////////////////////////////////////////////////////////////////////////////
    virtual size_t getUniformBufferSize() override
    {
        return static_cast<size_t>(sizeof(BasicLightData) + sizeof(glm::vec3) +
                                   sizeof(GLint) + 3 * sizeof(GLfloat));
    }


private:
    glm::vec3 m_Position;
    GLint     m_isAttennuating;
    GLfloat   m_atConstant;
    GLfloat   m_atLinear;
    GLfloat   m_atQuadratic;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class SpotLight : public Light
{
public:
    SpotLight() :
        m_Position(0, 0, 0),
        m_Direction(-1, -1, -1),
        m_InnerCutOffAngle(M_PI),
        m_OuterCutOffAngle(M_PI)
    {}

    void setLightDirection(const glm::vec3& direction)
    {
        m_Direction = direction;
    }
    void setLightPosition(const glm::vec3& position)
    {
        m_Position = position;
    }

    ////////////////////////////////////////////////////////////////////////////////
    glm::vec3 getLightPosition() const
    {
        return m_Position;
    }
    glm::vec3 getLightDirection() const
    {

        return m_Direction;
    }

    ////////////////////////////////////////////////////////////////////////////////
    void uploadLightPosition()
    {
        m_UniformBuffer.bind();
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(BasicLightData),
               sizeof(glm::vec3), glm::value_ptr(m_Position)));
        m_UniformBuffer.release();
    }
    void uploadLightDirection()
    {
        m_UniformBuffer.bind();
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(BasicLightData) + sizeof(glm::vec3),
               sizeof(glm::vec3), glm::value_ptr(m_Direction)));
        m_UniformBuffer.release();
    }
    void uploadLightCutOffAngles()
    {
        m_UniformBuffer.bind();
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(BasicLightData) + 2 * sizeof(glm::vec3),
               sizeof(GLfloat), &m_InnerCutOffAngle));
        glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(BasicLightData) + 2 * sizeof(glm::vec3) +
               sizeof(GLfloat), sizeof(GLfloat), &m_OuterCutOffAngle));
        m_UniformBuffer.release();
    }

    virtual void uploadBuffer() override
    {
        uploadBasicData();
        uploadLightDirection();
        uploadLightCutOffAngles();
    }

    ////////////////////////////////////////////////////////////////////////////////
    virtual size_t getUniformBufferSize() override
    {
        return static_cast<size_t>(sizeof(BasicLightData) + 2 * sizeof(glm::vec3) +
                                   2 * sizeof(GLfloat));
    }

private:
    glm::vec3 m_Position;
    glm::vec3 m_Direction;
    GLfloat   m_InnerCutOffAngle;
    GLfloat   m_OuterCutOffAngle;
};