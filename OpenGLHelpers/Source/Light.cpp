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
    static_assert(sizeof(glm::vec4) == sizeof(GLfloat) * 4,
                  "Size of glm::vec4 != 4 * sizeof(GLfloat).");

    m_BasicData.ambient  = glm::vec4(0.2, 0.2, 0.2, 1.0);
    m_BasicData.diffuse  = glm::vec4(1, 1, 1, 1.0);
    m_BasicData.specular = glm::vec4(1, 1, 1, 1.0);
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
    if(!m_UniformBuffer.isCreated())
    {
        createUniformBuffer();
    }

    m_UniformBuffer.uploadData(glm::value_ptr(m_BasicData.ambient), 0, sizeof(glm::vec4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::uploadLightDiffuse()
{
    if(!m_UniformBuffer.isCreated())
    {
        createUniformBuffer();
    }

    m_UniformBuffer.uploadData(glm::value_ptr(m_BasicData.diffuse),
                               sizeof(glm::vec4), sizeof(glm::vec4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::uploadLightSpecular()
{
    if(!m_UniformBuffer.isCreated())
    {
        createUniformBuffer();
    }

    m_UniformBuffer.uploadData(glm::value_ptr(m_BasicData.specular),
                               2 * sizeof(glm::vec4), sizeof(glm::vec4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::setLightAmbient(const glm::vec4& ambient)
{
    m_BasicData.ambient = ambient;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::setLightDiffuse(const glm::vec4& diffuse)
{
    m_BasicData.diffuse = diffuse;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::setLightSpecular(const glm::vec4& specular)
{
    m_BasicData.specular = specular;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::bindUniformBuffer()
{
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.bindBufferBase();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint Light::getBufferBindingPoint()
{
    if(!m_UniformBuffer.isCreated())
    {
        createUniformBuffer();
    }

    return m_UniformBuffer.getBindingPoint();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec4 Light::getLightAmbient() const
{
    return m_BasicData.ambient;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec4 Light::getLightDiffuse() const
{
    return m_BasicData.diffuse;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec4 Light::getLightSpecular() const
{
    return m_BasicData.specular;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Light::uploadBasicData()
{
    uploadLightAmbient();
    uploadLightDiffuse();
    uploadLightSpecular();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// DirectionalLight class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLight::setLightDirection(const glm::vec4& direction)
{
    m_Direction = direction;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec4 DirectionalLight::getLightDirection() const
{
    return m_Direction;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLight::uploadLightDirection()
{
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.uploadData(glm::value_ptr(m_Direction),
                               BasicLightData::getSize(), sizeof(glm::vec4));
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
    return static_cast<size_t>(BasicLightData::getSize() + sizeof(glm::vec4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PointLight class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLight::setLightPosition(const glm::vec4& position)
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
glm::vec4 PointLight::getLightPosition() const
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
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.uploadData(glm::value_ptr(m_Position),
                               BasicLightData::getSize(), sizeof(glm::vec4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLight::uploadAttennuationCoeffs()
{
    assert(m_UniformBuffer.isCreated());

    size_t offset   = BasicLightData::getSize() + sizeof(glm::vec4);
    size_t dataSize = sizeof(GLint);
    m_UniformBuffer.uploadData(&m_isAttennuating, offset, dataSize);

    offset  += dataSize;
    dataSize = sizeof(GLfloat);
    m_UniformBuffer.uploadData(&m_atConstant, offset, dataSize);

    offset  += dataSize;
    m_UniformBuffer.uploadData(&m_atLinear, offset, dataSize);

    offset  += dataSize;
    m_UniformBuffer.uploadData(&m_atQuadratic, offset, dataSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLight::uploadBuffer()
{
    uploadBasicData();
    uploadLightPosition();
    uploadAttennuationCoeffs();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t PointLight::getUniformBufferSize()
{
    return static_cast<size_t>(BasicLightData::getSize() + sizeof(glm::vec4) +
                               sizeof(GLint) + 3 * sizeof(GLfloat));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SpotLight class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLight::setLightDirection(const glm::vec4& direction)
{
    m_Direction = direction;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLight::setLightPosition(const glm::vec4& position)
{
    m_Position = position;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec4 SpotLight::getLightPosition() const
{
    return m_Position;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec4 SpotLight::getLightDirection() const
{

    return m_Direction;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLight::uploadLightPosition()
{
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.uploadData(glm::value_ptr(m_Position),
                               BasicLightData::getSize(), sizeof(glm::vec4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLight::uploadLightDirection()
{
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.uploadData(glm::value_ptr(m_Direction),
                               BasicLightData::getSize() + sizeof(glm::vec4), sizeof(glm::vec4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLight::uploadLightCutOffAngles()
{
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.uploadData(&m_InnerCutOffAngle,
                               BasicLightData::getSize() + 2 * sizeof(glm::vec4),
                               sizeof(GLfloat));
    m_UniformBuffer.uploadData(&m_OuterCutOffAngle,
                               BasicLightData::getSize() + 2 * sizeof(glm::vec4) + sizeof(GLfloat),
                               sizeof(GLfloat));
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
    return static_cast<size_t>(BasicLightData::getSize() + 2 * sizeof(glm::vec4) +
                               2 * sizeof(GLfloat));
}
