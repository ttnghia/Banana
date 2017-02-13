//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____                              
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _ 
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/ 
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
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

#include <OpenGLHelpers/Lights.h>
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
    PointLightRender(Camera* camera, PointLights* lights, OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_Lights(lights), m_RenderSize(20.0)
    {
        initRenderData();
    }

    void setRenderSize(GLfloat renderSize);
    virtual void render() override;

private:
    virtual void initRenderData() override;

    GLfloat      m_RenderSize;
    GLuint       m_UBLight;
    PointLights* m_Lights;
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
    OffScreenRender(int width, int height, int numColorBuffers = 1, GLenum formatColorBuffers = GL_RGBA) :
        RenderObject(nullptr, nullptr), m_BufferWidth(width), m_BufferHeight(height), m_NumColorBuffers(numColorBuffers), m_FormatColorBuff(formatColorBuffers)
    {
        initRenderData();
    }

    virtual void resize(int width, int height);
    virtual void beginRender();
    virtual void endRender(GLuint defaultFBO = 0);

    virtual void setNumColorBuffers(int numColorBuffers);
    void setColorBufferParameter(GLenum paramName, GLenum paramValue);

    OpenGLTexture* getColorBuffer(int colorBufferID = 0);
    void swapColorBuffer(OpenGLTexture*& colorBuffer, int bufferID = 0);

    virtual void render() override // do nothing
    {}

protected:
    virtual void initRenderData() override;

    int                         m_BufferWidth;
    int                         m_BufferHeight;
    int                         m_NumColorBuffers;
    GLenum                      m_FormatColorBuff;
    GLuint                      m_FrameBufferID;
    GLuint                      m_RenderBufferID;
    std::vector<OpenGLTexture*> m_ColorBuffers;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// DepthBufferRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DepthBufferRender : public OffScreenRender
{
public:
    DepthBufferRender(int width = 1024, int height = 1024) :
        OffScreenRender(width, height, 1, GL_R32F), m_ClearLinearDepthValue(-1.0e6), m_DefaultClearColor(glm::vec4(0.8, 0.8, 0.8, 1.0))
    {}

    virtual void beginRender() override;
    virtual void endRender(GLuint defaultFBO /* = 0 */) override;

    virtual void setNumColorBuffers(int numColorBuffers) override;

    void setDefaultClearColor(const glm::vec4& clearColor);
    void setClearDepthValue(GLfloat clearValue);
    OpenGLTexture* getDepthBuffer();

private:
    virtual void initRenderData() override;

    GLfloat   m_ClearLinearDepthValue;
    glm::vec4 m_DefaultClearColor;
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


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Mesh render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MeshRender : public RenderObject
{
public:
#ifdef __Banana_Qt__
    MeshRender(MeshObject* meshObj, Camera* camera, PointLights* light, QString textureFolder,
               Material* material = nullptr, OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_MeshObj(meshObj), m_Lights(light), m_Material(material), m_CurrentTexture(nullptr),
        m_ShadowBufferWidth(1024), m_ShadowBufferHeight(1024)
    {
        initRenderData();
        OpenGLTexture::loadTextures(m_Textures, textureFolder);
    }

    void loadTextures(QString textureFolder);
#endif

    MeshRender(MeshObject* meshObj, Camera* camera, PointLights* light, Material* material = nullptr, OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_MeshObj(meshObj), m_Lights(light), m_Material(material), m_CurrentTexture(nullptr),
        m_ShadowBufferWidth(1024), m_ShadowBufferHeight(1024)
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
    void setExternalShadowMaps(std::vector<OpenGLTexture*> shadowMaps);
    void resizeShadowMap(int width, int height);
    void transform(const glm::vec3& translation, const glm::vec3& scale);
    void setupVAO();

    OpenGLTexture* getShadowMap(int lightID = 0);
    std::vector<OpenGLTexture*> getAllShadowMaps();

    virtual void render() override;

    void initShadowMapRenderData(const glm::vec4& clearColor, bool bLinearDepthBuffer = false);
    void renderToDepthBuffer(int scrWidth, int scrHeight, GLuint defaultFBO = 0);

protected:
    virtual void initRenderData() override;

    GLuint                          m_AtrVPosition;
    GLuint                          m_AtrVNormal;
    GLuint                          m_AtrVTexCoord;
    GLuint                          m_UBLight;
    GLuint                          m_UBLightMatrices;
    GLuint                          m_DSULightID;
    GLuint                          m_UBMaterial;
    GLuint                          m_UHasTexture;
    GLuint                          m_UHasShadow;
    GLuint                          m_UTexSampler;
    GLuint                          m_UShadowMap[MAX_NUM_LIGHTS];
    MeshObject*                     m_MeshObj;
    PointLights*                    m_Lights;
    Material*                       m_Material;
    std::vector<OpenGLTexture*>     m_Textures;
    OpenGLTexture*                  m_CurrentTexture;
    std::vector<OpenGLTexture*>     m_ExternalShadowMaps;
    GLint                           m_ShadowBufferWidth;
    GLint                           m_ShadowBufferHeight;
    GLuint                          m_DSAtrVPosition;
    GLuint                          m_DSUBLightMatrices;
    GLuint                          m_DSUBModelMatrix;
    GLuint                          m_DSVAO;
    ShaderProgram*                  m_DepthShader;
    std::vector<DepthBufferRender*> m_DepthBufferRenders;
    glm::mat4                       m_LightView[MAX_NUM_LIGHTS];
    glm::mat4                       m_LightProjection[MAX_NUM_LIGHTS];

};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Plane render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PlaneRender : public MeshRender
{
public:
#ifdef __Banana_Qt__
    PlaneRender(Camera* camera, PointLights* light, QString texureFolder, OpenGLBuffer* bufferCamData = nullptr) :
        MeshRender(new GridObject, camera, light, texureFolder, nullptr, bufferCamData),
        m_AllowedNonTexRender(true)
    {}

#endif

    PlaneRender(Camera* camera, PointLights* light, OpenGLBuffer* bufferCamData = nullptr) :
        MeshRender(new GridObject, camera, light, nullptr, bufferCamData),
        m_AllowedNonTexRender(true)
    {}

    void setAllowNonTextureRender(bool allowNonTex);
    void scaleTexCoord(int scaleX, int scaleY);

    virtual void render() override;

private:
    bool        m_AllowedNonTexRender;
    GridObject* m_GridObj;

};
