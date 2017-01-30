//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/26/2017
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <OpenGLHelpers/OpenGLMacros.h>

#include <vector>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OpenGLTexture : public OpenGLCallable
{
public:
    OpenGLTexture() :
        m_bTextureCreated(false),
        m_BindedTexUnit(0),
        m_TexureTarget(GL_TEXTURE_2D)
    {}

    OpenGLTexture(GLenum textureTarget) :
        m_bTextureCreated(false),
        m_BindedTexUnit(0),
        m_TexureTarget(GL_TEXTURE_2D)
    {
        createTexture(textureTarget);
    }

    ~OpenGLTexture();

    void createTexture(GLenum textureTarget);
    void generateMipMap();
    void uploadData(GLenum texTarget, GLint internalFormat,
                    GLsizei width, GLsizei height,
                    GLenum dataFormat, GLenum dataType, const GLvoid * data);

    void setFilterMode(GLenum filterMode, GLenum value);
    void setAnisotropicFilter(bool enable);
    void setBorderColor(glm::vec4 borderColor);
    void setBestParameters();

    void bind(GLuint texUnit = 0);
    void release();

#ifdef __Banana_Qt__
    static void loadTextures(std::vector<OpenGLTexture*>& textures,
                             QString textureFolder, bool insertNullTex = true);
#endif

private:

    ////////////////////////////////////////////////////////////////////////////////
    GLenum m_TexureTarget;
    GLuint m_TextureID;
    GLuint m_BindedTexUnit;
    bool m_bTextureCreated;
};

