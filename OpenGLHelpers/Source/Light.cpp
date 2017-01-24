//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/24/2017
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

#include <OpenGLHelpers/Light.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Light base class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Light::Light()
{
    static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3,
                  "Size of glm::vec3 != 3*sizeof(GLfloat).");

    m_BasicData.ambient   = glm::vec3(0, 0, 0);
    m_BasicData.diffuse   = glm::vec3(1, 1, 1);
    m_BasicData.specular  = glm::vec3(1, 1, 1);
    m_BasicData.shininess = 100.0;

    createUniformBuffer();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::createUniformBuffer()
{
    size_t bufferSize = getUniformBufferSize();
    m_UniformBuffer.createBuffer(GL_UNIFORM_BUFFER, bufferSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::uploadLightAmbient()
{
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat3),
           glm::value_ptr(m_BasicData.ambient)));
    m_UniformBuffer.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::uploadLightDiffuse()
{
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat3), sizeof(glm::mat3),
           glm::value_ptr(m_BasicData.diffuse)));
    m_UniformBuffer.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::uploadLightSpecular()
{
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat3), sizeof(glm::mat3),
           glm::value_ptr(m_BasicData.specular)));
    m_UniformBuffer.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::uploadLightShininess()
{
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat3), sizeof(GLfloat),
           &m_BasicData.shininess));
    m_UniformBuffer.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::setLightAmbient(const glm::vec3 & ambient)
{
    m_BasicData.ambient = ambient;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::setLightDiffuse(const glm::vec3 & diffuse)
{
    m_BasicData.diffuse = diffuse;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::setLightSpecular(const glm::vec3 & specular)
{
    m_BasicData.specular = specular;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::setLightShininess(GLfloat shininess)
{
    m_BasicData.shininess = shininess;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec3 Light::getLightAmbient() const
{
    return m_BasicData.ambient;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec3 Light::getLightDiffuse() const
{
    return m_BasicData.diffuse;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec3 Light::getLightSpecular() const
{
    return m_BasicData.specular;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLfloat Light::getLightShininess() const
{
    return m_BasicData.shininess;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::uploadBasicData()
{
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(BasicLightData),
           &m_BasicData));
    m_UniformBuffer.release();
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// DirectionalLight class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLight::setLightDirection(const glm::vec3 & direction)
{
    m_Direction = direction;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec3 DirectionalLight::getLightDirection() const
{

    return m_Direction;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLight::uploadLightDirection()
{
    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(BasicLightData),
           sizeof(glm::vec3), glm::value_ptr(m_Direction)));
    m_UniformBuffer.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLight::uploadBuffer()
{
    uploadBasicData();
    uploadLightDirection();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t DirectionalLight::getUniformBufferSize()
{
    return static_cast<size_t>(sizeof(BasicLightData) + sizeof(glm::vec3));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PointLight class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLight::setLightPosition(const glm::vec3 & position)
{
    m_Position = position;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLight::setAttennulation(bool attennuation)
{
    m_isAttennuating = attennuation ? 1 : 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLight::setATConstantCoeff(GLfloat atConstant)
{
    m_atConstant = atConstant;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLight::setATLinearCoeff(GLfloat atLinear)
{
    m_atLinear = atLinear;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLight::setATQuadraticCoeff(GLfloat atQuadratic)
{
    m_atQuadratic = atQuadratic;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec3 PointLight::getLightPosition() const
{
    return m_Position;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PointLight::isAttennuating() const
{
    return (m_isAttennuating > 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLfloat PointLight::getATConstantCoeff()
{
    return m_atConstant;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLfloat PointLight::getATLinearCoeff()
{
    return m_atLinear;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLfloat PointLight::getATQuadraticCoeff()
{
    return m_atQuadratic;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLight::uploadLightPosition()
{
    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(BasicLightData),
           sizeof(glm::vec3), glm::value_ptr(m_Position)));
    m_UniformBuffer.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLight::uploadAttennuationCoeffs()
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLight::uploadBuffer()
{
    uploadBasicData();
    uploadAttennuationCoeffs();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t PointLight::getUniformBufferSize()
{
    return static_cast<size_t>(sizeof(BasicLightData) + sizeof(glm::vec3) +
                               sizeof(GLint) + 3 * sizeof(GLfloat));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SpotLight class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLight::setLightDirection(const glm::vec3 & direction)
{
    m_Direction = direction;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLight::setLightPosition(const glm::vec3 & position)
{
    m_Position = position;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec3 SpotLight::getLightPosition() const
{
    return m_Position;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec3 SpotLight::getLightDirection() const
{

    return m_Direction;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLight::uploadLightPosition()
{
    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(BasicLightData),
           sizeof(glm::vec3), glm::value_ptr(m_Position)));
    m_UniformBuffer.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLight::uploadLightDirection()
{
    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(BasicLightData) + sizeof(glm::vec3),
           sizeof(glm::vec3), glm::value_ptr(m_Direction)));
    m_UniformBuffer.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLight::uploadLightCutOffAngles()
{
    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(BasicLightData) + 2 * sizeof(glm::vec3),
           sizeof(GLfloat), &m_InnerCutOffAngle));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(BasicLightData) + 2 * sizeof(glm::vec3) +
           sizeof(GLfloat), sizeof(GLfloat), &m_OuterCutOffAngle));
    m_UniformBuffer.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLight::uploadBuffer()
{
    uploadBasicData();
    uploadLightDirection();
    uploadLightCutOffAngles();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t SpotLight::getUniformBufferSize()
{
    return static_cast<size_t>(sizeof(BasicLightData) + 2 * sizeof(glm::vec3) +
                               2 * sizeof(GLfloat));
}
