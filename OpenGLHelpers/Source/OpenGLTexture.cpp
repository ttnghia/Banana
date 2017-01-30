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

#include <OpenGLHelpers/OpenGLTexture.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLTexture::~OpenGLTexture()
{
    if(m_bTextureCreated)
        glCall(glDeleteTextures(1, &m_TextureID));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLTexture::OpenGLTexture::createTexture(GLenum textureTarget)
{
    if(m_bTextureCreated)
        glCall(glDeleteTextures(1, &m_TextureID));

    glCall(glGenTextures(1, &m_TextureID));
    m_bTextureCreated = true;
    m_TexureTarget = textureTarget;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLTexture::OpenGLTexture::generateMipMap()
{
    assert(m_bTextureCreated);

    bind();
    glCall(glGenerateMipmap(m_TexureTarget));
    release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLTexture::OpenGLTexture::uploadData(GLenum texTarget, GLint internalFormat,
                                              GLsizei width, GLsizei height,
                                              GLenum dataFormat, GLenum dataType,
                                              const GLvoid * data)
{
    assert(m_bTextureCreated);

    bind();
    glCall(glTexImage2D(texTarget, 0, internalFormat, width, height, 0,
           dataFormat, dataType, data));
    release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLTexture::setFilterMode(GLenum filterMode, GLenum value)
{
    assert(m_bTextureCreated);

    bind();
    glCall(glTexParameteri(m_TexureTarget, filterMode, value));
    release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLTexture::OpenGLTexture::setAnisotropicFilter(bool enable)
{
    assert(m_bTextureCreated);

    bind();

    GLfloat fLargest = 1.0;
    if(enable)
    {
        glCall(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest));
    }

    glCall(glTexParameterf(m_TexureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest));

    release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLTexture::setBorderColor(glm::vec4 borderColor)
{
    assert(m_bTextureCreated);

    bind();
    glCall(glTexParameterfv(m_TexureTarget, GL_TEXTURE_BORDER_COLOR,
           glm::value_ptr(borderColor)));
    release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLTexture::setBestParameters()
{
    assert(m_bTextureCreated);

    bind();
    glCall(glTexParameteri(m_TexureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT));
    glCall(glTexParameteri(m_TexureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT));
    glCall(glTexParameteri(m_TexureTarget, GL_TEXTURE_WRAP_R, GL_REPEAT));

    glCall(glTexParameteri(m_TexureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    glCall(glTexParameteri(m_TexureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLfloat fLargest;
    glCall(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest));
    glCall(glTexParameterf(m_TexureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest));

    if(m_TexureTarget == GL_TEXTURE_CUBE_MAP)
    {
        glCall(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
    }
    release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLTexture::OpenGLTexture::bind(GLuint texUnit /*= 0*/)
{
    assert(m_bTextureCreated);

    m_BindedTexUnit = GL_TEXTURE0 + texUnit;
    glCall(glActiveTexture(m_BindedTexUnit));
    glCall(glBindTexture(m_TexureTarget, m_TextureID));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLTexture::OpenGLTexture::release()
{
    glCall(glActiveTexture(m_BindedTexUnit));
    glCall(glBindTexture(m_TexureTarget, 0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __Banana_Qt__
#include <QDir>
void OpenGLTexture::loadTextures(std::vector<OpenGLTexture*>& textures,
                                 QString textureFolder, bool insertNullTex /*= true*/)
{
    // clear current textures
    for(OpenGLTexture* tex : textures)
    {
        delete tex;
    }

    textures.resize(0);
    if(insertNullTex)
    {
        textures.push_back(nullptr);
    }

    QDir dataDir(textureFolder);
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    QStringList allTexFiles = dataDir.entryList();

    foreach(QString texFile, allTexFiles)
    {
        QString texFilePath = textureFolder + "/" + texFile;
        OpenGLTexture* tex = new OpenGLTexture(GL_TEXTURE_2D);
        QImage texImg = QImage(texFilePath).convertToFormat(QImage::Format_RGBA8888);

        tex->uploadData(GL_TEXTURE_2D,
                        GL_RGBA, texImg.width(), texImg.height(),
                        GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());

        tex->setBestParameters();
        tex->generateMipMap();
        textures.push_back(tex);
    }
}
#endif