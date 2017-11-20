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

#pragma once

#include <OpenGLHelpers/OpenGLMacros.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OpenGLTexture : public OpenGLCallable
{
public:
    OpenGLTexture() : m_bTextureCreated(false), m_BindedTexUnit(0), m_TexureTarget(GL_TEXTURE_2D) {}
    OpenGLTexture(GLenum textureTarget) : m_bTextureCreated(false), m_BindedTexUnit(0), m_TexureTarget(GL_TEXTURE_2D) { createTexture(textureTarget); }

    bool isCreated() { return m_bTextureCreated; }
    void createTexture(GLenum textureTarget);
    void generateMipMap();
    void uploadData(GLenum texTarget, GLint internalFormat, GLsizei width, GLsizei height, GLenum dataFormat, GLenum dataType, const GLvoid* data);

    void setTextureParameter(GLenum filterMode, GLenum value);
    void setAnisotropicFilter(bool enable);
    void setBorderColor(glm::vec4 borderColor);
    void setBestParametersWithMipMap();
    void setBestParametersNoMipMap();
    void setSimplestTexture();

    GLuint getTextureID() const { return m_TextureID; }
    void   bind(GLuint texUnit = 0);
    void   release();

#ifdef __Banana_Qt__
    static void        loadTextures(std::vector<std::shared_ptr<OpenGLTexture> >& textures, QString textureFolder, bool insertNullTex = true, bool bGenMipMap = true);
    static QStringList getTextureFolders(QString texType, QString texRootFolder = QString("Textures"));
    static QStringList getTextureFiles(QString texType, QString texRootFolder = QString("Textures"));
#endif

private:

    ////////////////////////////////////////////////////////////////////////////////
    GLenum m_TexureTarget;
    GLuint m_TextureID;
    GLuint m_BindedTexUnit;
    bool   m_bTextureCreated;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana