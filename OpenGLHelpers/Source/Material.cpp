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

#include <OpenGLHelpers/Material.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Material::Material()
{
    static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3,
                  "Size of glm::vec3 != 3*sizeof(GLfloat).");

    m_MaterialData.ambient   = glm::vec3(0.0215, 0.1745, 0.0215);
    m_MaterialData.diffuse   = glm::vec3(0.07568, 0.61424, 0.07568);
    m_MaterialData.specular  = glm::vec3(0.633, 0.727811, 0.633);
    m_MaterialData.shininess = 0.6;

    createUniformBuffer();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::createUniformBuffer()
{
    size_t bufferSize = 3 * sizeof(glm::vec3) + sizeof(GLfloat);
    m_UniformBuffer.createBuffer(GL_UNIFORM_BUFFER, bufferSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadAmbientColor()
{
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat3),
           glm::value_ptr(m_MaterialData.ambient)));
    m_UniformBuffer.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadDiffuseColor()
{
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat3), sizeof(glm::mat3),
           glm::value_ptr(m_MaterialData.diffuse)));
    m_UniformBuffer.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadSpecularColor()
{
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat3), sizeof(glm::mat3),
           glm::value_ptr(m_MaterialData.specular)));
    m_UniformBuffer.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadShininess()
{
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat3), sizeof(GLfloat),
           &m_MaterialData.shininess));
    m_UniformBuffer.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setAmbientColor(const glm::vec3 & ambient)
{
    m_MaterialData.ambient = ambient;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setDiffuseColor(const glm::vec3 & diffuse)
{
    m_MaterialData.diffuse = diffuse;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setSpecularColor(const glm::vec3 & specular)
{
    m_MaterialData.specular = specular;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setShininess(GLint shininess)
{
    m_MaterialData.shininess = shininess;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec3 Material::getAmbientColor() const
{
    return m_MaterialData.ambient;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec3 Material::getDiffuseColor() const
{
    return m_MaterialData.diffuse;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec3 Material::getSpecularColor() const
{
    return m_MaterialData.specular;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLfloat Material::getShininess() const
{
    return m_MaterialData.shininess;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadMaterial()
{
    assert(m_UniformBuffer.isCreated());

    m_UniformBuffer.bind();
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialData),
           &m_MaterialData));
    m_UniformBuffer.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setMaterial(MaterialData material)
{
    m_MaterialData = material;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Prebuild materials
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Material::MaterialData Material::MT_Emerald       ={glm::vec3(0.0125, 0.1745, 0.0215),
                                                          glm::vec3(0.07568, 0.61424, 0.07568),
                                                          glm::vec3(0.633, 0.727811, 0.633),
                                                          0.6};
const Material::MaterialData Material::MT_Jade          ={glm::vec3(0.135,0.2225,0.1575),
                                                          glm::vec3(0.54, 0.89, 0.63),
                                                          glm::vec3(0.316228, 0.316228, 0.316228),
                                                          0.1};
const Material::MaterialData Material::MT_Obsidian      ={glm::vec3(0.05375, 0.05, 0.06625),
                                                          glm::vec3(0.18275, 0.17, 0.22525),
                                                          glm::vec3(0.332741, 0.328634, 0.346435),
                                                          0.3};
const Material::MaterialData Material::MT_Pearl         ={glm::vec3(0.25, 0.20725, 0.20725),
                                                          glm::vec3(1, 0.829, 0.829),
                                                          glm::vec3(0.296648, 0.296648, 0.296648),
                                                          0.088};
const Material::MaterialData Material::MT_Ruby          ={glm::vec3(0.1745, 0.01175, 0.01175),
                                                          glm::vec3(0.61424, 0.04136, 0.04136),
                                                          glm::vec3(0.727811, 0.626959, 0.626959),
                                                          0.6};
const Material::MaterialData Material::MT_Turquoise     ={glm::vec3(0.1, 0.18725, 0.1745),
                                                          glm::vec3(0.396, 0.74151, 0.69102),
                                                          glm::vec3(0.297254, 0.30829, 0.306678),
                                                          0.1};
const Material::MaterialData Material::MT_Brass         ={glm::vec3(0.329412, 0.223529, 0.027451),
                                                          glm::vec3(0.780392, 0.568627, 0.113725),
                                                          glm::vec3(0.992157, 0.941176, 0.807843),
                                                          0.21794872};
const Material::MaterialData Material::MT_Bronze        ={glm::vec3(0.2125, 0.1275, 0.054),
                                                          glm::vec3(0.714, 0.4284, 0.18144),
                                                          glm::vec3(0.393548, 0.271906, 0.166721),
                                                          0.2};
const Material::MaterialData Material::MT_Chrome        ={glm::vec3(0.25, 0.25, 0.25),
                                                          glm::vec3(0.4, 0.4, 0.4),
                                                          glm::vec3(0.774597, 0.774597, 0.774597),
                                                          0.6};
const Material::MaterialData Material::MT_Copper        ={glm::vec3(0.19125, 0.0735, 0.0225),
                                                          glm::vec3(0.7038, 0.27048, 0.0828),
                                                          glm::vec3(0.256777, 0.137622, 0.086014),
                                                          0.1};
const Material::MaterialData Material::MT_Gold          ={glm::vec3(0.24725, 0.1995, 0.0745),
                                                          glm::vec3(0.75164, 0.60648, 0.22648),
                                                          glm::vec3(0.628281, 0.555802, 0.366065),
                                                          0.4};
const Material::MaterialData Material::MT_Silver        ={glm::vec3(0.19225, 0.19225, 0.19225),
                                                          glm::vec3(0.50754, 0.50754, 0.50754),
                                                          glm::vec3(0.508273, 0.508273, 0.508273),
                                                          0.4};
const Material::MaterialData Material::MT_BlackPlastic  ={glm::vec3(0,0,0),
                                                          glm::vec3(0.01, 0.01, 0.01),
                                                          glm::vec3(0.50, 0.50, 0.50),
                                                          0.25};
const Material::MaterialData Material::MT_CyanPlastic   ={glm::vec3(0.0, 0.1, 0.06),
                                                          glm::vec3(0.0, 0.50980392, 0.50980392),
                                                          glm::vec3(0.50196078, 0.50196078, 0.50196078),
                                                          0.25};
const Material::MaterialData Material::MT_GreenPlastic  ={glm::vec3(0.0, 0.0, 0.0),
                                                          glm::vec3(0.1, 0.35, 0.1),
                                                          glm::vec3(0.45, 0.55, 0.45),
                                                          0.25};
const Material::MaterialData Material::MT_RedPlastic    ={glm::vec3(0.0, 0.0, 0.0),
                                                          glm::vec3(0.5, 0.0, 0.0),
                                                          glm::vec3(0.7, 0.6, 0.6),
                                                          0.25};
const Material::MaterialData Material::MT_WhitePlastic  ={glm::vec3(0.0, 0.0, 0.0),
                                                          glm::vec3(0.55, 0.55, 0.55),
                                                          glm::vec3(0.70, 0.70, 0.70),
                                                          0.25};
const Material::MaterialData Material::MT_YellowPlastic ={glm::vec3(0.0, 0.0, 0.0),
                                                          glm::vec3(0.5, 0.5, 0.0),
                                                          glm::vec3(0.60, 0.60, 0.50),
                                                          0.25};
const Material::MaterialData Material::MT_BlackRubber   ={glm::vec3(0.02, 0.02, 0.02),
                                                          glm::vec3(0.01, 0.01, 0.01),
                                                          glm::vec3(0.4, 0.4, 0.4),
                                                          0.078125};
const Material::MaterialData Material::MT_CyanRubber    ={glm::vec3(0.0, 0.05, 0.05),
                                                          glm::vec3(0.4, 0.5, 0.5),
                                                          glm::vec3(0.04, 0.7, 0.7),
                                                          0.078125};
const Material::MaterialData Material::MT_GreenRubber   ={glm::vec3(0.0, 0.05, 0.0),
                                                          glm::vec3(0.4, 0.5, 0.4),
                                                          glm::vec3(0.04, 0.7, 0.04),
                                                          0.078125};
const Material::MaterialData Material::MT_RedRubber     ={glm::vec3(0.05, 0.0, 0.0),
                                                          glm::vec3(0.5, 0.4, 0.4),
                                                          glm::vec3(0.7, 0.04, 0.04),
                                                          0.078125};
const Material::MaterialData Material::MT_WhiteRubber   ={glm::vec3(0.05, 0.05, 0.05),
                                                          glm::vec3(0.5, 0.5, 0.5),
                                                          glm::vec3(0.7, 0.7, 0.7),
                                                          0.078125};
const Material::MaterialData Material::MT_YellowRubber  ={glm::vec3(0.05, 0.05, 0.0),
                                                          glm::vec3(0.5, 0.5, 0.4),
                                                          glm::vec3(0.7, 0.7, 0.04),
                                                          0.078125};
