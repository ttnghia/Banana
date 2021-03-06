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

#include <OpenGLHelpers/Material.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Material::Material(String materialName /*= String("NoName")*/)
{
    static_assert(sizeof(Vec4f) == sizeof(GLfloat) * 4, "Size of Vec4f != 4 * sizeof(GLfloat).");

    setMaterial(MT_Emerald);
    m_MaterialData.name = materialName;
}

Material::Material(const MaterialData& material, String materialName /*= String("NoName")*/)
{
    static_assert(sizeof(Vec4f) == sizeof(GLfloat) * 4, "Size of Vec4f != 4 * sizeof(GLfloat).");

    m_MaterialData      = material;
    m_MaterialData.name = materialName;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::createUniformBuffer()
{
    size_t bufferSize = 3 * sizeof(Vec4f) + sizeof(GLfloat);
    m_UniformBuffer.createBuffer(GL_UNIFORM_BUFFER, bufferSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadAmbientColor()
{
    if(!m_UniformBuffer.isCreated()) {
        createUniformBuffer();
    }

    m_UniformBuffer.uploadData(glm::value_ptr(m_MaterialData.ambient), 0, sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadDiffuseColor()
{
    if(!m_UniformBuffer.isCreated()) {
        createUniformBuffer();
    }

    m_UniformBuffer.uploadData(glm::value_ptr(m_MaterialData.diffuse), sizeof(Vec4f), sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadSpecularColor()
{
    if(!m_UniformBuffer.isCreated()) {
        createUniformBuffer();
    }

    m_UniformBuffer.uploadData(glm::value_ptr(m_MaterialData.specular), 2 * sizeof(Vec4f), sizeof(Vec4f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadShininess()
{
    if(!m_UniformBuffer.isCreated()) {
        createUniformBuffer();
    }

    m_UniformBuffer.uploadData(&m_MaterialData.shininess, 3 * sizeof(Vec4f), sizeof(GLfloat));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setAmbientColor(const Vec4f& ambient)
{
    m_MaterialData.ambient = ambient;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setDiffuseColor(const Vec4f& diffuse)
{
    m_MaterialData.diffuse = diffuse;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setSpecularColor(const Vec4f& specular)
{
    m_MaterialData.specular = specular;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setShininess(GLfloat shininess)
{
    m_MaterialData.shininess = shininess;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f Material::getAmbientColor() const
{
    return m_MaterialData.ambient;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f Material::getDiffuseColor() const
{
    return m_MaterialData.diffuse;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Vec4f Material::getSpecularColor() const
{
    return m_MaterialData.specular;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLfloat Material::getShininess() const
{
    return m_MaterialData.shininess;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadDataToGPU()
{
    uploadAmbientColor();
    uploadDiffuseColor();
    uploadSpecularColor();
    uploadShininess();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::bindUniformBuffer()
{
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.bindBufferBase();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint Material::getBufferBindingPoint()
{
    assert(m_UniformBuffer.isCreated());
    return m_UniformBuffer.getBindingPoint();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setMaterial(const MaterialData& material)
{
    m_MaterialData = material;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
String Material::getName()
{
    return m_MaterialData.name;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
std::vector<Material::MaterialData> Material::getBuildInMaterials()
{
    std::vector<MaterialData> materials;

    materials.push_back(Material::MT_Emerald);
    materials.push_back(Material::MT_Jade);
    materials.push_back(Material::MT_Obsidian);
    materials.push_back(Material::MT_Pearl);
    materials.push_back(Material::MT_Ruby);
    materials.push_back(Material::MT_Turquoise);
    materials.push_back(Material::MT_Brass);
    materials.push_back(Material::MT_Bronze);
    materials.push_back(Material::MT_Chrome);
    materials.push_back(Material::MT_Copper);
    materials.push_back(Material::MT_Gold);
    materials.push_back(Material::MT_Silver);
    materials.push_back(Material::MT_BlackPlastic);
    materials.push_back(Material::MT_CyanPlastic);
    materials.push_back(Material::MT_GreenPlastic);
    materials.push_back(Material::MT_RedPlastic);
    materials.push_back(Material::MT_WhitePlastic);
    materials.push_back(Material::MT_YellowPlastic);
    materials.push_back(Material::MT_BlackRubber);
    materials.push_back(Material::MT_CyanRubber);
    materials.push_back(Material::MT_GreenRubber);
    materials.push_back(Material::MT_RedRubber);
    materials.push_back(Material::MT_WhiteRubber);
    materials.push_back(Material::MT_YellowRubber);

    return materials;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Prebuild materials
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Material::MaterialData Material::MT_Emerald = { Vec4f(0.0125f,    0.1745f,  0.0215f, 1.0f),
                                                      Vec4f(0.07568f, 0.61424f,  0.07568f, 1.0f),
                                                      Vec4f(0.633f,   0.727811f, 0.633f,   1.0f),
                                                      0.6f * 128.0f,      "Emerald" };
const Material::MaterialData Material::MT_Jade = { Vec4f(0.135f,      0.2225f,   0.1575f, 1.0f),
                                                   Vec4f(0.54f,     0.89f,     0.63f,     1.0f),
                                                   Vec4f(0.316228f, 0.316228f, 0.316228f, 1.0f),
                                                   0.1f * 128.0f,       "Jade" };
const Material::MaterialData Material::MT_Obsidian = { Vec4f(0.05375f,      0.05f,  0.06625f, 1.0f),
                                                       Vec4f(0.18275f,  0.17f,     0.22525f,  1.0f),
                                                       Vec4f(0.332741f, 0.328634f, 0.346435f, 1.0f),
                                                       0.3f * 128.0f,       "Obsidian" };
const Material::MaterialData Material::MT_Pearl = { Vec4f(0.25f,      0.20725f,  0.20725f, 1.0f),
                                                    Vec4f(1.0f,      0.829f,    0.829f,    1.0f),
                                                    Vec4f(0.296648f, 0.296648f, 0.296648f, 1.0f),
                                                    0.088f * 128.0f,     "Pearl" };
const Material::MaterialData Material::MT_Ruby = { Vec4f(0.1745f,    0.01175f,  0.01175f, 1.0f),
                                                   Vec4f(0.61424f,  0.04136f,  0.04136f,  1.0f),
                                                   Vec4f(0.727811f, 0.626959f, 0.626959f, 1.0f),
                                                   0.6f * 128.0f,       "Ruby" };
const Material::MaterialData Material::MT_Turquoise = { Vec4f(0.1f,      0.18725f,   0.1745f, 1.0f),
                                                        Vec4f(0.396f,    0.74151f, 0.69102f,  1.0f),
                                                        Vec4f(0.297254f, 0.30829f, 0.306678f, 1.0f),
                                                        0.1f * 128.0f,       "Turquoise" };
const Material::MaterialData Material::MT_Brass = { Vec4f(0.329412f,  0.223529f, 0.027451f, 1.0f),
                                                    Vec4f(0.780392f, 0.568627f, 0.113725f, 1.0f),
                                                    Vec4f(0.992157f, 0.941176f, 0.807843f, 1.0f),
                                                    0.21794872f * 128.0f, "Brass" };
const Material::MaterialData Material::MT_Bronze = { Vec4f(0.2125f,     0.1275f,    0.054f, 1.0f),
                                                     Vec4f(0.714f,    0.4284f,   0.18144f,  1.0f),
                                                     Vec4f(0.393548f, 0.271906f, 0.166721f, 1.0f),
                                                     0.2f * 128.0f,       "Bronze" };
const Material::MaterialData Material::MT_Chrome = { Vec4f(0.25f,         0.25f,     0.25f, 1.0f),
                                                     Vec4f(0.4f,      0.4f,      0.4f,      1.0f),
                                                     Vec4f(0.774597f, 0.774597f, 0.774597f, 1.0f),
                                                     0.6f * 128.0f,       "Chrome" };
const Material::MaterialData Material::MT_Copper = { Vec4f(0.19125f,    0.0735f,   0.0225f, 1.0f),
                                                     Vec4f(0.7038f,   0.27048f,  0.0828f,   1.0f),
                                                     Vec4f(0.256777f, 0.137622f, 0.086014f, 1.0f),
                                                     0.1f * 128.0f,       "Copper" };
const Material::MaterialData Material::MT_Gold = { Vec4f(0.24725f,    0.1995f,   0.0745f, 1.0f),
                                                   Vec4f(0.75164f,  0.60648f,  0.22648f,  1.0f),
                                                   Vec4f(0.628281f, 0.555802f, 0.366065f, 1.0f),
                                                   0.4f * 128.0f,       "Gold" };
const Material::MaterialData Material::MT_Silver = { Vec4f(0.19225f,   0.19225f,  0.19225f, 1.0f),
                                                     Vec4f(0.50754f,  0.50754f,  0.50754f,  1.0f),
                                                     Vec4f(0.508273f, 0.508273f, 0.508273f, 1.0f),
                                                     0.4f * 128.0f,       "Silver" };
const Material::MaterialData Material::MT_BlackPlastic = { Vec4f(0.0f,   0.0f,  0.0f, 1.0f),
                                                           Vec4f(0.01f, 0.01f, 0.01f, 1.0f),
                                                           Vec4f(0.50f, 0.50f, 0.50f, 1.0f),
                                                           0.25f * 128.0f,  "BlackPlastic" };
const Material::MaterialData Material::MT_CyanPlastic = { Vec4f(0.0f,               0.1f,       0.06f, 1.0f),
                                                          Vec4f(0.0f,        0.50980392f, 0.50980392f, 1.0f),
                                                          Vec4f(0.50196078f, 0.50196078f, 0.50196078f, 1.0f),
                                                          0.25f * 128.0f,        "CyanPlastic" };
const Material::MaterialData Material::MT_GreenPlastic = { Vec4f(0.0f,   0.0f,  0.0f, 1.0f),
                                                           Vec4f(0.1f,  0.35f, 0.1f,  1.0f),
                                                           Vec4f(0.45f, 0.55f, 0.45f, 1.0f),
                                                           0.25f * 128.0f,  "GreenPlastic" };
const Material::MaterialData Material::MT_RedPlastic = { Vec4f(0.0f, 0.0f, 0.0f, 1.0f),
                                                         Vec4f(0.5f, 0.0f, 0.0f, 1.0f),
                                                         Vec4f(0.7f, 0.6f, 0.6f, 1.0f),
                                                         0.25f * 128.0f, "RedPlastic" };
const Material::MaterialData Material::MT_WhitePlastic = { Vec4f(0.0f,   0.0f,  0.0f, 1.0f),
                                                           Vec4f(0.55f, 0.55f, 0.55f, 1.0f),
                                                           Vec4f(0.70f, 0.70f, 0.70f, 1.0f),
                                                           0.25f * 128.0f,  "WhitePlastic" };
const Material::MaterialData Material::MT_YellowPlastic = { Vec4f(0.0f,   0.0f,  0.0f, 1.0f),
                                                            Vec4f(0.5f,  0.5f,  0.0f,  1.0f),
                                                            Vec4f(0.60f, 0.60f, 0.50f, 1.0f),
                                                            0.25f * 128.0f,  "YellowPlastic" };
const Material::MaterialData Material::MT_BlackRubber = { Vec4f(0.02f,    0.02f, 0.02f, 1.0f),
                                                          Vec4f(0.01f, 0.01f, 0.01f, 1.0f),
                                                          Vec4f(0.4f,  0.4f,  0.4f,  1.0f),
                                                          0.078125f * 128.0f, "BlackRubber" };
const Material::MaterialData Material::MT_CyanRubber = { Vec4f(0.0f,     0.05f, 0.05f, 1.0f),
                                                         Vec4f(0.4f,  0.5f, 0.5f, 1.0f),
                                                         Vec4f(0.04f, 0.7f, 0.7f, 1.0f),
                                                         0.078125f * 128.0f, "CyanRubber" };
const Material::MaterialData Material::MT_GreenRubber = { Vec4f(0.0f,     0.05f,  0.0f, 1.0f),
                                                          Vec4f(0.4f,  0.5f, 0.4f,  1.0f),
                                                          Vec4f(0.04f, 0.7f, 0.04f, 1.0f),
                                                          0.078125f * 128.0f, "GreenRubber" };
const Material::MaterialData Material::MT_RedRubber = { Vec4f(0.05f,     0.0f,  0.0f, 1.0f),
                                                        Vec4f(0.5f, 0.4f,  0.4f,  1.0f),
                                                        Vec4f(0.7f, 0.04f, 0.04f, 1.0f),
                                                        0.078125f * 128.0f, "RedRubber" };
const Material::MaterialData Material::MT_WhiteRubber = { Vec4f(0.05f,    0.05f, 0.05f, 1.0f),
                                                          Vec4f(0.5f, 0.5f, 0.5f, 1.0f),
                                                          Vec4f(0.7f, 0.7f, 0.7f, 1.0f),
                                                          0.078125f * 128.0f, "WhiteRubber" };
const Material::MaterialData Material::MT_YellowRubber = { Vec4f(0.05f,    0.05f,  0.0f, 1.0f),
                                                           Vec4f(0.5f, 0.5f, 0.4f,  1.0f),
                                                           Vec4f(0.7f, 0.7f, 0.04f, 1.0f),
                                                           0.078125f * 128.0f, "YellowRubber" };

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana