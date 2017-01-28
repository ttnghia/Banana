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
    static_assert(sizeof(glm::vec4) == sizeof(GLfloat) * 4,
                  "Size of glm::vec4 != 4 * sizeof(GLfloat).");

    m_MaterialData.ambient   = glm::vec4(0.0215, 0.1745, 0.0215, 1.0);
    m_MaterialData.diffuse   = glm::vec4(0.07568, 0.61424, 0.07568, 1.0);
    m_MaterialData.specular  = glm::vec4(0.633, 0.727811, 0.633, 1.0);
    m_MaterialData.shininess = 0.6;
    m_MaterialData.name      = std::string("NoName");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::createUniformBuffer()
{
    size_t bufferSize = 3 * sizeof(glm::vec4) + sizeof(GLfloat);
    m_UniformBuffer.createBuffer(GL_UNIFORM_BUFFER, bufferSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadAmbientColor()
{
    if(!m_UniformBuffer.isCreated())
    {
        createUniformBuffer();
    }

    m_UniformBuffer.uploadData(glm::value_ptr(m_MaterialData.ambient), 0, sizeof(glm::vec4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadDiffuseColor()
{
    if(!m_UniformBuffer.isCreated())
    {
        createUniformBuffer();
    }

    m_UniformBuffer.uploadData(glm::value_ptr(m_MaterialData.diffuse),
                               sizeof(glm::vec4), sizeof(glm::vec4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadSpecularColor()
{
    if(!m_UniformBuffer.isCreated())
    {
        createUniformBuffer();
    }

    m_UniformBuffer.uploadData(glm::value_ptr(m_MaterialData.specular),
                               2 * sizeof(glm::vec4), sizeof(glm::vec4));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadShininess()
{
    if(!m_UniformBuffer.isCreated())
    {
        createUniformBuffer();
    }

    m_UniformBuffer.uploadData(&m_MaterialData.shininess,
                               3 * sizeof(glm::vec4), sizeof(GLfloat));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setAmbientColor(const glm::vec4& ambient)
{
    m_MaterialData.ambient = ambient;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setDiffuseColor(const glm::vec4& diffuse)
{
    m_MaterialData.diffuse = diffuse;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setSpecularColor(const glm::vec4& specular)
{
    m_MaterialData.specular = specular;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setShininess(GLint shininess)
{
    m_MaterialData.shininess = shininess;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec4 Material::getAmbientColor() const
{
    return m_MaterialData.ambient;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec4 Material::getDiffuseColor() const
{
    return m_MaterialData.diffuse;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
glm::vec4 Material::getSpecularColor() const
{
    return m_MaterialData.specular;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLfloat Material::getShininess() const
{
    return m_MaterialData.shininess;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::uploadBuffer()
{
    uploadAmbientColor();
    uploadDiffuseColor();
    uploadSpecularColor();
    uploadShininess();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::bindUniformBuffer()
{
    assert(m_UniformBuffer.isCreated());
    m_UniformBuffer.bindBufferBase();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
GLuint Material::getBufferBindingPoint()
{
    assert(m_UniformBuffer.isCreated());
    return m_UniformBuffer.getBindingPoint();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Material::setMaterial(const MaterialData& material)
{
    m_MaterialData = material;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
std::string Material::getName()
{
    return m_MaterialData.name;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Prebuild materials
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Material::MaterialData Material::MT_Emerald       ={glm::vec4(0.0125, 0.1745, 0.0215, 1.0),
                                                          glm::vec4(0.07568, 0.61424, 0.07568, 1.0),
                                                          glm::vec4(0.633, 0.727811, 0.633, 1.0),
                                                          0.6 * 128.0, "Emerald"};
const Material::MaterialData Material::MT_Jade          ={glm::vec4(0.135,0.2225,0.1575, 1.0),
                                                          glm::vec4(0.54, 0.89, 0.63, 1.0),
                                                          glm::vec4(0.316228, 0.316228, 0.316228, 1.0),
                                                          0.1 * 128.0, "Jade"};
const Material::MaterialData Material::MT_Obsidian      ={glm::vec4(0.05375, 0.05, 0.06625, 1.0),
                                                          glm::vec4(0.18275, 0.17, 0.22525, 1.0),
                                                          glm::vec4(0.332741, 0.328634, 0.346435, 1.0),
                                                          0.3 * 128.0, "Obsidian"};
const Material::MaterialData Material::MT_Pearl         ={glm::vec4(0.25, 0.20725, 0.20725, 1.0),
                                                          glm::vec4(1, 0.829, 0.829, 1.0),
                                                          glm::vec4(0.296648, 0.296648, 0.296648, 1.0),
                                                          0.088 * 128.0, "Pearl"};
const Material::MaterialData Material::MT_Ruby          ={glm::vec4(0.1745, 0.01175, 0.01175, 1.0),
                                                          glm::vec4(0.61424, 0.04136, 0.04136, 1.0),
                                                          glm::vec4(0.727811, 0.626959, 0.626959, 1.0),
                                                          0.6 * 128.0, "Ruby"};
const Material::MaterialData Material::MT_Turquoise     ={glm::vec4(0.1, 0.18725, 0.1745, 1.0),
                                                          glm::vec4(0.396, 0.74151, 0.69102, 1.0),
                                                          glm::vec4(0.297254, 0.30829, 0.306678, 1.0),
                                                          0.1 * 128.0, "Turquoise"};
const Material::MaterialData Material::MT_Brass         ={glm::vec4(0.329412, 0.223529, 0.027451, 1.0),
                                                          glm::vec4(0.780392, 0.568627, 0.113725, 1.0),
                                                          glm::vec4(0.992157, 0.941176, 0.807843, 1.0),
                                                          0.21794872 * 128.0, "Brass"};
const Material::MaterialData Material::MT_Bronze        ={glm::vec4(0.2125, 0.1275, 0.054, 1.0),
                                                          glm::vec4(0.714, 0.4284, 0.18144, 1.0),
                                                          glm::vec4(0.393548, 0.271906, 0.166721, 1.0),
                                                          0.2 * 128.0, "Bronze"};
const Material::MaterialData Material::MT_Chrome        ={glm::vec4(0.25, 0.25, 0.25, 1.0),
                                                          glm::vec4(0.4, 0.4, 0.4, 1.0),
                                                          glm::vec4(0.774597, 0.774597, 0.774597, 1.0),
                                                          0.6 * 128.0, "Chrome"};
const Material::MaterialData Material::MT_Copper        ={glm::vec4(0.19125, 0.0735, 0.0225, 1.0),
                                                          glm::vec4(0.7038, 0.27048, 0.0828, 1.0),
                                                          glm::vec4(0.256777, 0.137622, 0.086014, 1.0),
                                                          0.1 * 128.0, "Copper"};
const Material::MaterialData Material::MT_Gold          ={glm::vec4(0.24725, 0.1995, 0.0745, 1.0),
                                                          glm::vec4(0.75164, 0.60648, 0.22648, 1.0),
                                                          glm::vec4(0.628281, 0.555802, 0.366065, 1.0),
                                                          0.4 * 128.0, "Gold"};
const Material::MaterialData Material::MT_Silver        ={glm::vec4(0.19225, 0.19225, 0.19225, 1.0),
                                                          glm::vec4(0.50754, 0.50754, 0.50754, 1.0),
                                                          glm::vec4(0.508273, 0.508273, 0.508273, 1.0),
                                                          0.4 * 128.0, "Silver"};
const Material::MaterialData Material::MT_BlackPlastic  ={glm::vec4(0,0,0, 1.0),
                                                          glm::vec4(0.01, 0.01, 0.01, 1.0),
                                                          glm::vec4(0.50, 0.50, 0.50, 1.0),
                                                          0.25 * 128.0, "BlackPlastic"};
const Material::MaterialData Material::MT_CyanPlastic   ={glm::vec4(0.0, 0.1, 0.06, 1.0),
                                                          glm::vec4(0.0, 0.50980392, 0.50980392, 1.0),
                                                          glm::vec4(0.50196078, 0.50196078, 0.50196078, 1.0),
                                                          0.25 * 128.0, "CyanPlastic"};
const Material::MaterialData Material::MT_GreenPlastic  ={glm::vec4(0.0, 0.0, 0.0, 1.0),
                                                          glm::vec4(0.1, 0.35, 0.1, 1.0),
                                                          glm::vec4(0.45, 0.55, 0.45, 1.0),
                                                          0.25 * 128.0, "GreenPlastic"};
const Material::MaterialData Material::MT_RedPlastic    ={glm::vec4(0.0, 0.0, 0.0, 1.0),
                                                          glm::vec4(0.5, 0.0, 0.0, 1.0),
                                                          glm::vec4(0.7, 0.6, 0.6, 1.0),
                                                          0.25 * 128.0, "RedPlastic"};
const Material::MaterialData Material::MT_WhitePlastic  ={glm::vec4(0.0, 0.0, 0.0, 1.0),
                                                          glm::vec4(0.55, 0.55, 0.55, 1.0),
                                                          glm::vec4(0.70, 0.70, 0.70, 1.0),
                                                          0.25 * 128.0, "WhitePlastic"};
const Material::MaterialData Material::MT_YellowPlastic ={glm::vec4(0.0, 0.0, 0.0, 1.0),
                                                          glm::vec4(0.5, 0.5, 0.0, 1.0),
                                                          glm::vec4(0.60, 0.60, 0.50, 1.0),
                                                          0.25 * 128.0, "YellowPlastic"};
const Material::MaterialData Material::MT_BlackRubber   ={glm::vec4(0.02, 0.02, 0.02, 1.0),
                                                          glm::vec4(0.01, 0.01, 0.01, 1.0),
                                                          glm::vec4(0.4, 0.4, 0.4, 1.0),
                                                          0.078125 * 128.0, "BlackRubber"};
const Material::MaterialData Material::MT_CyanRubber    ={glm::vec4(0.0, 0.05, 0.05, 1.0),
                                                          glm::vec4(0.4, 0.5, 0.5, 1.0),
                                                          glm::vec4(0.04, 0.7, 0.7, 1.0),
                                                          0.078125 * 128.0, "CyanRubber"};
const Material::MaterialData Material::MT_GreenRubber   ={glm::vec4(0.0, 0.05, 0.0, 1.0),
                                                          glm::vec4(0.4, 0.5, 0.4, 1.0),
                                                          glm::vec4(0.04, 0.7, 0.04, 1.0),
                                                          0.078125 * 128.0, "GreenRubber"};
const Material::MaterialData Material::MT_RedRubber     ={glm::vec4(0.05, 0.0, 0.0, 1.0),
                                                          glm::vec4(0.5, 0.4, 0.4, 1.0),
                                                          glm::vec4(0.7, 0.04, 0.04, 1.0),
                                                          0.078125 * 128.0, "RedRubber"};
const Material::MaterialData Material::MT_WhiteRubber   ={glm::vec4(0.05, 0.05, 0.05, 1.0),
                                                          glm::vec4(0.5, 0.5, 0.5, 1.0),
                                                          glm::vec4(0.7, 0.7, 0.7, 1.0),
                                                          0.078125 * 128.0, "WhiteRubber"};
const Material::MaterialData Material::MT_YellowRubber  ={glm::vec4(0.05, 0.05, 0.0, 1.0),
                                                          glm::vec4(0.5, 0.5, 0.4, 1.0),
                                                          glm::vec4(0.7, 0.7, 0.04, 1.0),
                                                          0.078125 * 128.0, "YellowRubber"};
