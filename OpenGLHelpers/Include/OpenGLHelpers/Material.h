//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/23/2017
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

#include <vector>

#include <OpenGLHelpers/OpenGLMacros.h>
#include <OpenGLHelpers/OpenGLBuffer.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/*
Name           Ambient                    Diffuse                        Specular                         Shininess
Emerald        0.0215   0.1745   0.0215   0.07568  0.61424    0.07568    0.633      0.727811   0.633      0.6
Jade           0.135    0.2225   0.1575   0.54     0.89       0.63       0.316228   0.316228   0.316228   0.1
Obsidian       0.05375  0.05     0.06625  0.18275  0.17       0.22525    0.332741   0.328634   0.346435   0.3
Pearl          0.25     0.20725  0.20725  1 0.829  0.829      0.296648   0.296648   0.296648   0.088
Ruby           0.1745   0.01175  0.01175  0.61424  0.04136    0.04136    0.727811   0.626959   0.626959   0.6
Turquoise      0.1      0.18725  0.1745   0.396    0.74151    0.69102    0.297254   0.30829    0.306678   0.1
Brass          0.329412 0.223529 0.027451 0.780392 0.568627   0.113725   0.992157   0.941176   0.807843   0.21794872
Bronze         0.2125   0.1275   0.054    0.714    0.4284     0.18144    0.393548   0.271906   0.166721   0.2
Chrome         0.25     0.25     0.25     0.4      0.4        0.4        0.774597   0.774597   0.774597   0.6
Copper         0.19125  0.0735   0.0225   0.7038   0.27048    0.0828     0.256777   0.137622   0.086014   0.1
Gold           0.24725  0.1995   0.0745   0.75164  0.60648    0.22648    0.628281   0.555802   0.366065   0.4
Silver         0.19225  0.19225  0.19225  0.50754  0.50754    0.50754    0.508273   0.508273   0.508273   0.4
BlackPlastic   0.0      0.0      0.0      0.01     0.01       0.01       0.50       0.50       0.50       .25
CyanPlastic    0.0      0.1      0.06     0.0      0.50980392 0.50980392 0.50196078 0.50196078 0.50196078 .25
GreenPlastic   0.0      0.0      0.0      0.1      0.35       0.1        0.45       0.55       0.45       .25
RedPlastic     0.0      0.0      0.0      0.5      0.0        0.0        0.7        0.6        0.6        .25
WhitePlastic   0.0      0.0      0.0      0.55     0.55       0.55       0.70       0.70       0.70       .25
YellowPlastic  0.0      0.0      0.0      0.5      0.5        0.0        0.60       0.60       0.50       .25
BlackRubber    0.02     0.02     0.02     0.01     0.01       0.01       0.4        0.4        0.4        .078125
CyanRubber     0.0      0.05     0.05     0.4      0.5        0.5        0.04       0.7        0.7        .078125
GreenRubber    0.0      0.05     0.0      0.4      0.5        0.4        0.04       0.7        0.04       .078125
RedRubber      0.05     0.0      0.0      0.5      0.4        0.4        0.7        0.04       0.04       .078125
WhiteRubber    0.05     0.05     0.05     0.5      0.5        0.5        0.7        0.7        0.7        .078125
YellowRubber   0.05     0.05     0.0      0.5      0.5        0.4        0.7        0.7        0.04       .078125
*/
#ifdef __Banana_Qt__
class Material : public OpenGLFunctions
{
#else
class Material
{
#endif

public:
    struct MaterialData
    {
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        GLfloat shininess;
        std::string name;

        static size_t getSize()
        {
            return 3 * sizeof(glm::vec4) + sizeof(GLfloat);
        }
    };

    Material();

    void createUniformBuffer();

    void uploadAmbientColor();
    void uploadDiffuseColor();
    void uploadSpecularColor();
    void uploadShininess();

    void setAmbientColor(const glm::vec4& ambient);
    void setDiffuseColor(const glm::vec4& diffuse);
    void setSpecularColor(const glm::vec4& specular);
    void setShininess(GLint shininess);

    glm::vec4 getAmbientColor() const;
    glm::vec4 getDiffuseColor() const;
    glm::vec4 getSpecularColor() const;
    GLfloat getShininess() const;

    void uploadBuffer();
    void bindUniformBuffer();
    GLuint getBufferBindingPoint();

    void setMaterial(const MaterialData& material);
    std::string getName();

    ////////////////////////////////////////////////////////////////////////////////
    static std::vector<MaterialData> getBuildInMaterials();

    const static MaterialData MT_Emerald;
    const static MaterialData MT_Jade;
    const static MaterialData MT_Obsidian;
    const static MaterialData MT_Pearl;
    const static MaterialData MT_Ruby;
    const static MaterialData MT_Turquoise;
    const static MaterialData MT_Brass;
    const static MaterialData MT_Bronze;
    const static MaterialData MT_Chrome;
    const static MaterialData MT_Copper;
    const static MaterialData MT_Gold;
    const static MaterialData MT_Silver;
    const static MaterialData MT_BlackPlastic;
    const static MaterialData MT_CyanPlastic;
    const static MaterialData MT_GreenPlastic;
    const static MaterialData MT_RedPlastic;
    const static MaterialData MT_WhitePlastic;
    const static MaterialData MT_YellowPlastic;
    const static MaterialData MT_BlackRubber;
    const static MaterialData MT_CyanRubber;
    const static MaterialData MT_GreenRubber;
    const static MaterialData MT_RedRubber;
    const static MaterialData MT_WhiteRubber;
    const static MaterialData MT_YellowRubber;

protected:
    OpenGLBuffer m_UniformBuffer;
    MaterialData m_MaterialData;
};
