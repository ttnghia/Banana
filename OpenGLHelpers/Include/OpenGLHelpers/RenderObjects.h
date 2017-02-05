//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/28/2017
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <OpenGLHelpers/OpenGLMacros.h>
#include <OpenGLHelpers/Camera.h>
#include <OpenGLHelpers/ShaderProgram.h>
#include <OpenGLHelpers/OpenGLBuffer.h>
#include <OpenGLHelpers/OpenGLTexture.h>
#include <OpenGLHelpers/CubeObject.h>
#include <OpenGLHelpers/GridObject.h>
#include <OpenGLHelpers/WireFrameBoxObject.h>

#include <OpenGLHelpers/Light.h>
#include <OpenGLHelpers/Material.h>

#include <vector>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// abstract base class of object render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RenderObject : public OpenGLCallable
{
protected:
    RenderObject(Camera* camera, OpenGLBuffer* bufferCamData = nullptr) :
        m_Camera(camera), m_UBufferCamData(bufferCamData)
    {
        m_SelfUpdateCamera = (m_UBufferCamData == nullptr);
    }

    virtual void initRenderData() = 0;
    virtual void render() = 0;

    GLuint         m_UBModelMatrix;
    GLuint         m_UBCamData;
    GLuint         m_VAO;

    bool           m_SelfUpdateCamera;
    Camera*        m_Camera;
    OpenGLBuffer*  m_UBufferModelMatrix;
    OpenGLBuffer*  m_UBufferCamData;
    ShaderProgram* m_Shader;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SkyBox render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class SkyBoxRender : public RenderObject
{
public:
#ifdef __Banana_Qt__
    SkyBoxRender(Camera* camera, QString texureTopFolder, OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_CurrentTexture(nullptr)
    {
        initRenderData();
        loadTextures(texureTopFolder);
    }

    void loadTextures(QString textureTopFolder);
#endif

    SkyBoxRender(Camera* camera, OpenGLBuffer* bufferCamData = nullptr) : RenderObject(camera, bufferCamData), m_CurrentTexture(nullptr)
    {
        initRenderData();
    }

    SkyBoxRender(Camera* camera, std::vector<OpenGLTexture*> textures, OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_Textures(textures), m_CurrentTexture(nullptr)
    {
        initRenderData();
    }

    OpenGLTexture* getCurrentTexture();
    size_t getNumTextures();
    void clearTextures();
    void addTexture(OpenGLTexture* texture);
    void setRenderTextureIndex(int texIndex);

    virtual void render() override;

protected:
    virtual void initRenderData() override;

    GLuint                      m_AtrVPosition;
    GLuint                      m_UTexSampler;
    CubeObject*                 m_CubeObj;
    OpenGLTexture*              m_CurrentTexture;
    std::vector<OpenGLTexture*> m_Textures;

};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Light render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PointLightRender : public RenderObject
{
public:
    PointLightRender(Camera* camera, PointLight* light, OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_Light(light), m_RenderSize(20.0)
    {
        initRenderData();
    }

    void setRenderSize(GLfloat renderSize);
    virtual void render() override;

private:
    virtual void initRenderData() override;

    GLfloat        m_RenderSize;
    GLuint         m_UBLight;
    PointLight*    m_Light;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Mesh render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MeshRender : public RenderObject
{
public:
#ifdef __Banana_Qt__
    MeshRender(MeshObject* meshObj, Camera* camera, PointLight* light, QString textureFolder,
               Material* material = nullptr, OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_MeshObj(meshObj), m_Light(light), m_Material(material), m_CurrentTexture(nullptr)
    {
        initRenderData();
        OpenGLTexture::loadTextures(m_Textures, textureFolder);
    }

    void loadTexture(QString textureFolder);
#endif

    MeshRender(MeshObject* meshObj, Camera* camera, PointLight* light, Material* material = nullptr, OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_MeshObj(meshObj), m_Light(light), m_Material(material), m_CurrentTexture(nullptr)
    {
        initRenderData();
    }

    MeshObject*    getMeshObj();
    Material*      getMaterial();
    OpenGLTexture* getCurrentTexture();
    size_t         getNumTextures();

    void clearTextures(bool insertNullTex = true);
    void addTexture(OpenGLTexture* texture, GLenum texWrapMode = GL_REPEAT);
    void setRenderTextureIndex(int texIndex);
    void transform(const glm::vec3& translation, const glm::vec3& scale);
    void setupVAO();

    virtual void render() override;

private:
    virtual void initRenderData() override;

    GLuint                      m_AtrVPosition;
    GLuint                      m_AtrVNormal;
    GLuint                      m_AtrVTexCoord;
    GLuint                      m_UBLight;
    GLuint                      m_UBMaterial;
    GLuint                      m_UHasTexture;
    GLuint                      m_UTexSampler;
    MeshObject*                 m_MeshObj;
    PointLight*                 m_Light;
    Material*                   m_Material;
    std::vector<OpenGLTexture*> m_Textures;
    OpenGLTexture*              m_CurrentTexture;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Plane render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PlaneRender : public RenderObject
{
public:
#ifdef __Banana_Qt__
    PlaneRender(Camera* camera, PointLight* light, QString texureFolder, OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData),
        m_MeshRender(new MeshRender(new GridObject, camera, light, texureFolder, nullptr, bufferCamData)),
        m_AllowedNonTexRender(true)
    {
        initRenderData();
    }

    void loadTextures(QString textureFolder);
#endif

    PlaneRender(Camera* camera, PointLight* light, OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData),
        m_MeshRender(new MeshRender(new GridObject, camera, light, nullptr, bufferCamData)),
        m_AllowedNonTexRender(true)
    {
        initRenderData();
    }

    void setAllowNonTextureRender(bool allowNonTex);
    void clearTextures();
    void addTexture(OpenGLTexture* texture, GLenum texWrapMode = GL_REPEAT);
    void setRenderTextureIndex(int texIndex);
    size_t getNumTextures();

    void transform(const glm::vec3& translation, const glm::vec3& scale);
    void scaleTexCoord(int scaleX, int scaleY);

    virtual void render() override;

private:
    virtual void initRenderData() override;

    bool        m_AllowedNonTexRender;
    MeshRender* m_MeshRender;
    GridObject* m_GridObj;

};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WirteFrameBoxRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class WireFrameBoxRender : public RenderObject
{
public:
    WireFrameBoxRender(Camera* camera, OpenGLBuffer* bufferCamData = nullptr) : RenderObject(camera, bufferCamData), m_BoxColor(0.5, 0.5, 0)
    {
        initRenderData();
    }

    void transform(const glm::vec3& translation, const glm::vec3& scale);
    void setBox(const glm::vec3& boxMin, const glm::vec3& boxMax);
    virtual void render() override;

private:
    virtual void initRenderData() override;

    glm::vec3           m_BoxColor;
    GLuint              m_AtrVPosition;
    GLuint              m_UColor;
    WireFrameBoxObject* m_WireFrameBoxObj;

};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// OffScreenRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OffScreenRender : public RenderObject
{
public:
    OffScreenRender(int width, int height, int numColorBuffers = 1, bool hasStencilBuffer = false,
                    GLenum formatDepthStencilBuffer = GL_DEPTH_COMPONENT, GLenum formatColorBuffers = GL_RGBA,
                    Camera* camera = nullptr, OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_BufferWidth(width), m_BufferHeight(height), m_hasStencilBuffer(hasStencilBuffer),
        m_NumColorBuffers(numColorBuffers), m_FormatDepthStencilBuff(formatDepthStencilBuffer), m_FormatColorBuff(formatColorBuffers)
    {
        initRenderData();
    }

    void resize(int width, int height);
    void beginRender();
    void endRender(GLuint defaultFBO = 0);

    OpenGLTexture* getDepthStencilBuffer();
    OpenGLTexture* getColorBuffer(int colorBufferID = 0);

    virtual void render() override // do nothing
    {}

private:
    virtual void   initRenderData() override;

    int                         m_BufferWidth;
    int                         m_BufferHeight;
    int                         m_NumColorBuffers;
    bool                        m_hasStencilBuffer;
    GLenum                      m_FormatDepthStencilBuff;
    GLenum                      m_FormatColorBuff;
    GLuint                      m_FrameBufferID;
    OpenGLTexture*              m_DepthStencilBuffer;
    std::vector<OpenGLTexture*> m_ColorBuffers;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// DepthBufferRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DepthBufferRender : public RenderObject
{
public:
    DepthBufferRender(Camera* camera, OpenGLBuffer* bufferCamData = nullptr, int width = 1024, int height = 1024) :
        RenderObject(camera, bufferCamData),
        m_BufferWidth(width), m_BufferHeight(height)
    {
        initRenderData();
    }

    void resize(int width, int height);
    void beginRender();
    void endRender();

    OpenGLTexture* getFloatDepthBuffer();
    OpenGLTexture* getTestDepthBuffer();

    virtual void render() override // do nothing
    {}

private:
    virtual void initRenderData() override;
    void generateFrameBuffer();

    int            m_BufferWidth;
    int            m_BufferHeight;
    glm::vec4      m_ClearColor;
    GLuint         m_AtrVPosition;
    GLuint         m_FrameBuffer;
    OpenGLTexture* m_DepthBuffer;
    OpenGLTexture* m_DepthTestBuffer;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ScreenQuadTextureRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ScreenQuadTextureRender : public RenderObject
{
public:
    ScreenQuadTextureRender() : RenderObject(nullptr, nullptr), m_Texture(nullptr), m_TexelSizeValue(1), m_ValueScale(1.0)
    {
        initRenderData();
    }

    ScreenQuadTextureRender(OpenGLTexture* texture, int texelSize = 1) :
        RenderObject(nullptr, nullptr), m_Texture(texture), m_TexelSizeValue(texelSize), m_ValueScale(1.0)
    {
        initRenderData();
    }

    void setValueScale(float scale);
    void setTexture(OpenGLTexture* texture, int texelSize = 1);
    virtual void render() override;
private:
    virtual void initRenderData() override;

    float          m_ValueScale;
    int            m_TexelSizeValue;
    GLuint         m_AtrVPosition;
    GLuint         m_UTexelSize;
    GLuint         m_UValueScale;
    GLuint         m_UTexSampler;
    OpenGLTexture* m_Texture;
};