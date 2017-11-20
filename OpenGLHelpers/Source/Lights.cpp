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

#include <OpenGLHelpers/Lights.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Light base class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Lights::setNumLights(int numLights)
{
    m_NumActiveLights = numLights;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int Lights::getNumLights() const
{
    return m_NumActiveLights;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Lights::createUniformBuffer()
{
    size_t bufferSize = getUniformBufferSize();
    m_UniformBuffer.createBuffer(GL_UNIFORM_BUFFER, bufferSize);

    m_UniformBufferLightMatrix.createBuffer(GL_UNIFORM_BUFFER, 2 * MAX_NUM_LIGHTS * sizeof(glm::mat4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Lights::bindUniformBuffer()
{
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.bindBufferBase();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint Lights::getBufferBindingPoint()
{
    if(!m_UniformBuffer.isCreated()) {
        createUniformBuffer();
    }

    return m_UniformBuffer.getBindingPoint();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t Lights::getUniformBufferSize() const
{
    return MAX_NUM_LIGHTS * getLightSize() + sizeof(GLint);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t Lights::getLightDataSize() const
{
    return MAX_NUM_LIGHTS * getLightSize();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Lights::setSceneCenter(const Vec3f& sceneCenter)
{
    m_SceneCenter = sceneCenter;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Lights::bindUniformBufferLightMatrix()
{
    assert(m_UniformBufferLightMatrix.isCreated());
    m_UniformBufferLightMatrix.bindBufferBase();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint Lights::getBufferLightMatrixBindingPoint()
{
    if(!m_UniformBufferLightMatrix.isCreated()) {
        createUniformBuffer();
    }

    return m_UniformBufferLightMatrix.getBindingPoint();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Lights::uploadLightMatrixToGPU()
{
    assert(m_UniformBufferLightMatrix.isCreated());
    m_UniformBufferLightMatrix.uploadData(m_LightMatrices, 0, 2 * MAX_NUM_LIGHTS * sizeof(glm::mat4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// DirectionalLight class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLights::setLight(const DirectionalLightData& lightData, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID] = lightData;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
DirectionalLights::DirectionalLightData DirectionalLights::getLight(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return m_Lights[lightID];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLights::setLightDirection(const Vec4f& direction, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID].setDirection(direction);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f DirectionalLights::getLightDirection(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return glm::make_vec4(m_Lights[lightID].direction);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLights::uploadLightDirection(int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.uploadData(m_Lights[lightID].direction, getLightSize() * lightID + 3 * sizeof(Vec4f), sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLights::setLightAmbient(const Vec4f& ambient, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID].setAmbient(ambient);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLights::setLightDiffuse(const Vec4f& diffuse, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID].setDiffuse(diffuse);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLights::setLightSpecular(const Vec4f& specular, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID].setSpecular(specular);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f DirectionalLights::getLightAmbient(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return glm::make_vec4(m_Lights[lightID].ambient);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f DirectionalLights::getLightDiffuse(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return glm::make_vec4(m_Lights[lightID].diffuse);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f DirectionalLights::getLightSpecular(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return glm::make_vec4(m_Lights[lightID].specular);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLights::uploadLightAmbient(int lightID /*= -1*/)
{
    assert(lightID < m_NumActiveLights);
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.uploadData(m_Lights[lightID].ambient, getLightSize() * lightID, sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLights::uploadLightDiffuse(int lightID /*= -1*/)
{
    assert(lightID < m_NumActiveLights);
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.uploadData(m_Lights[lightID].diffuse, getLightSize() * lightID + sizeof(Vec4f), sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLights::uploadLightSpecular(int lightID /*= -1*/)
{
    assert(lightID < m_NumActiveLights);
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.uploadData(m_Lights[lightID].specular, getLightSize() * lightID + 2 * sizeof(Vec4f), sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLights::uploadDataToGPU()
{
    if(!m_UniformBuffer.isCreated()) {
        createUniformBuffer();
    }

    m_UniformBuffer.uploadData(m_Lights,           0,        getLightDataSize());
    m_UniformBuffer.uploadData(&m_NumActiveLights, getLightDataSize(), sizeof(GLint));

    updateLightMatrixBuffer();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLights::updateLightMatrixBuffer()
{
    for(int i = 0; i < m_NumActiveLights; ++i) {
        glm::mat4 lightView       = glm::lookAt(m_SceneCenter - glm::make_vec3(m_Lights[i].direction), m_SceneCenter, Vec3f(0.0f, -0.91f, 0.01f));
        glm::mat4 lightProjection = glm::ortho(m_ShadowMinX, m_ShadowMaxX, m_ShadowMinY, m_ShadowMaxY, m_ShadowMinZ, m_ShadowMaxZ);

        m_LightMatrices[i].setViewMatrix(lightView);
        m_LightMatrices[i].setProjectionMatrix(lightProjection);
    }

    uploadLightMatrixToGPU();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DirectionalLights::setShadowMapBox(GLfloat minX, GLfloat maxX, GLfloat minY, GLfloat maxY, GLfloat minZ, GLfloat maxZ)
{
    m_ShadowMinX = minX;
    m_ShadowMaxX = maxX;
    m_ShadowMinY = minY;
    m_ShadowMaxY = maxY;
    m_ShadowMinZ = minZ;
    m_ShadowMaxZ = maxZ;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PointLight class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLights::setLight(const PointLightData& lightData, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID] = lightData;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
PointLights::PointLightData PointLights::getLight(int lightID) const
{
    assert(lightID < m_NumActiveLights);
    return m_Lights[lightID];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLights::setLightPosition(const Vec4f& position, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID].setPosition(position);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f PointLights::getLightPosition(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return glm::make_vec4(m_Lights[lightID].position);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLights::uploadLightPosition(int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.uploadData(m_Lights[lightID].position, getLightSize() * lightID + 3 * sizeof(Vec4f), sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLights::setLightAmbient(const Vec4f& ambient, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID].setAmbient(ambient);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLights::setLightDiffuse(const Vec4f& diffuse, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID].setDiffuse(diffuse);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLights::setLightSpecular(const Vec4f& specular, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID].setSpecular(specular);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f PointLights::getLightAmbient(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return glm::make_vec4(m_Lights[lightID].ambient);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f PointLights::getLightDiffuse(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return glm::make_vec4(m_Lights[lightID].diffuse);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f PointLights::getLightSpecular(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return glm::make_vec4(m_Lights[lightID].specular);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLights::uploadLightAmbient(int lightID /*= -1*/)
{
    assert(lightID < m_NumActiveLights);
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.uploadData(m_Lights[lightID].ambient, getLightSize() * lightID, sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLights::uploadLightDiffuse(int lightID /*= -1*/)
{
    assert(lightID < m_NumActiveLights);
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.uploadData(m_Lights[lightID].diffuse, getLightSize() * lightID + sizeof(Vec4f), sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLights::uploadLightSpecular(int lightID /*= -1*/)
{
    assert(lightID < m_NumActiveLights);
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.uploadData(m_Lights[lightID].specular, getLightSize() * lightID + 2 * sizeof(Vec4f), sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLights::uploadDataToGPU()
{
    if(!m_UniformBuffer.isCreated()) {
        createUniformBuffer();
    }

    m_UniformBuffer.uploadData(m_Lights,           0,        getLightDataSize());
    m_UniformBuffer.uploadData(&m_NumActiveLights, getLightDataSize(), sizeof(GLint));

    updateLightMatrixBuffer();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLights::updateLightMatrixBuffer()
{
    for(int i = 0; i < m_NumActiveLights; ++i) {
        glm::mat4 lightView       = glm::lookAt(glm::make_vec3(m_Lights[i].position), m_SceneCenter, Vec3f(0.0f, -0.91f, 0.01f));
        glm::mat4 lightProjection = glm::perspective(glm::radians(m_ShadowFOV), m_ShadowAspect, m_ShadowNearZ, m_ShadowFarZ);

        m_LightMatrices[i].setViewMatrix(lightView);
        m_LightMatrices[i].setProjectionMatrix(lightProjection);
    }

    uploadLightMatrixToGPU();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLights::setLightViewPerspective(GLfloat fov, GLfloat asspect /*= 1.0*/, GLfloat nearZ /*= 0.1f*/, GLfloat farZ /*= 1000.0f*/)
{
    m_ShadowFOV    = fov;
    m_ShadowAspect = asspect;
    m_ShadowNearZ  = nearZ;
    m_ShadowFarZ   = farZ;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SpotLight class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::setLight(const SpotLightData& lightData, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID] = lightData;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SpotLights::SpotLightData SpotLights::getLight(int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    return m_Lights[lightID];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::setLightDirection(const Vec4f& direction, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID].setDirection(direction);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::setLightPosition(const Vec4f& position, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID].setPosition(position);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::setLightCuffOffAngles(float innerAngle, float outerAngle, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID].innerCutOffAngle = innerAngle;
    m_Lights[lightID].outerCutOffAngle = outerAngle;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f SpotLights::getLightPosition(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return glm::make_vec4(m_Lights[lightID].position);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f SpotLights::getLightDirection(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return glm::make_vec4(m_Lights[lightID].direction);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
float SpotLights::getInnerCutOffAngle(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return m_Lights[lightID].innerCutOffAngle;
}

float SpotLights::getOuterCutOffAngle(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return m_Lights[lightID].outerCutOffAngle;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::uploadLightPosition(int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.uploadData(m_Lights[lightID].position, getLightSize() * lightID + 3 * sizeof(Vec4f), sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::uploadLightDirection(int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.uploadData(m_Lights[lightID].direction, getLightSize() * lightID + 4 * sizeof(Vec4f), sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::uploadLightCutOffAngles(int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.uploadData(&m_Lights[lightID].innerCutOffAngle, getLightSize() * lightID + 5 * sizeof(Vec4f),                   sizeof(GLfloat));
    m_UniformBuffer.uploadData(&m_Lights[lightID].outerCutOffAngle, getLightSize() * lightID + 5 * sizeof(Vec4f) + sizeof(GLfloat), sizeof(GLfloat));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::setLightAmbient(const Vec4f& ambient, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID].setAmbient(ambient);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::setLightDiffuse(const Vec4f& diffuse, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID].setDiffuse(diffuse);
}

void SpotLights::setLightSpecular(const Vec4f& specular, int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    m_Lights[lightID].setSpecular(specular);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f SpotLights::getLightAmbient(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return glm::make_vec4(m_Lights[lightID].ambient);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f SpotLights::getLightDiffuse(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return glm::make_vec4(m_Lights[lightID].diffuse);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f SpotLights::getLightSpecular(int lightID /*= 0*/) const
{
    assert(lightID < m_NumActiveLights);
    return glm::make_vec4(m_Lights[lightID].specular);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::uploadLightAmbient(int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.uploadData(m_Lights[lightID].ambient, getLightSize() * lightID, sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::uploadLightDiffuse(int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.uploadData(m_Lights[lightID].diffuse, getLightSize() * lightID + sizeof(Vec4f), sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::uploadLightSpecular(int lightID /*= 0*/)
{
    assert(lightID < m_NumActiveLights);
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.uploadData(m_Lights[lightID].specular, getLightSize() * lightID + 2 * sizeof(Vec4f), sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::uploadDataToGPU()
{
    if(!m_UniformBuffer.isCreated()) {
        createUniformBuffer();
    }

    m_UniformBuffer.uploadData(m_Lights,           0,        getLightDataSize());
    m_UniformBuffer.uploadData(&m_NumActiveLights, getLightDataSize(), sizeof(GLint));

    updateLightMatrixBuffer();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::updateLightMatrixBuffer()
{
    for(int i = 0; i < m_NumActiveLights; ++i) {
        glm::mat4 lightView       = glm::lookAt(glm::make_vec3(m_Lights[i].position), m_SceneCenter, Vec3f(0.0f, -0.91f, 0.01f));
        glm::mat4 lightProjection = glm::perspective(glm::radians(m_ShadowFOV), m_ShadowAspect, m_ShadowNearZ, m_ShadowFarZ);

        m_LightMatrices[i].setViewMatrix(lightView);
        m_LightMatrices[i].setProjectionMatrix(lightProjection);
    }

    uploadLightMatrixToGPU();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SpotLights::setLightViewPerspective(GLfloat fov, GLfloat asspect /*= 1.0*/, GLfloat nearZ /*= 0.1f*/, GLfloat farZ /*= 1000.0f*/)
{
    m_ShadowFOV    = fov;
    m_ShadowAspect = asspect;
    m_ShadowNearZ  = nearZ;
    m_ShadowFarZ   = farZ;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana