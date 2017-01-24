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
    Light();

    void createUniformBuffer();
    void uploadLightAmbient();
    void uploadLightDiffuse();
    void uploadLightSpecular();
    void uploadLightShininess();

    void setLightAmbient(const glm::vec3& ambient);
    void setLightDiffuse(const glm::vec3& diffuse);
    void setLightSpecular(const glm::vec3& specular);
    void setLightShininess(GLfloat shininess);

    glm::vec3 getLightAmbient() const;
    glm::vec3 getLightDiffuse() const;
    glm::vec3 getLightSpecular() const;
    GLfloat getLightShininess() const;

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

    void uploadBasicData();

    OpenGLBuffer   m_UniformBuffer;
    BasicLightData m_BasicData;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DirectionalLight : public Light
{
public:
    DirectionalLight() : m_Direction(-1.0, -1.0, -1.0)
    {}

    void setLightDirection(const glm::vec3& direction);
    glm::vec3 getLightDirection() const;

    void uploadLightDirection();
    virtual void uploadBuffer() override;
    virtual size_t getUniformBufferSize() override;

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

    void setLightPosition(const glm::vec3& position);
    void setAttennulation(bool attennuation);
    void setATConstantCoeff(GLfloat atConstant);
    void setATLinearCoeff(GLfloat atLinear);
    void setATQuadraticCoeff(GLfloat atQuadratic);

    glm::vec3 getLightPosition() const;
    bool isAttennuating() const;
    GLfloat getATConstantCoeff();
    GLfloat getATLinearCoeff();
    GLfloat getATQuadraticCoeff();

    void uploadLightPosition();
    void uploadAttennuationCoeffs();

    virtual void uploadBuffer() override;
    virtual size_t getUniformBufferSize() override;

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

    void setLightDirection(const glm::vec3& direction);
    void setLightPosition(const glm::vec3& position);

    glm::vec3 getLightPosition() const;
    glm::vec3 getLightDirection() const;

    void uploadLightPosition();
    void uploadLightDirection();
    void uploadLightCutOffAngles();

    virtual void uploadBuffer() override;
    virtual size_t getUniformBufferSize() override;

private:
    glm::vec3 m_Position;
    glm::vec3 m_Direction;
    GLfloat   m_InnerCutOffAngle;
    GLfloat   m_OuterCutOffAngle;
};