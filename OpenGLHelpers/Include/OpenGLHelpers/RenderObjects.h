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
#include <OpenGLHelpers/MeshObjects/CubeObject.h>
#include <OpenGLHelpers/MeshObjects/GridObject.h>
#include <OpenGLHelpers/MeshObjects/WireFrameBoxObject.h>

#include <OpenGLHelpers/Lights.h>
#include <OpenGLHelpers/Material.h>

#include <vector>
#include <memory>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// abstract base class of object render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RenderObject : public OpenGLCallable
{
public:
    const std::shared_ptr<ShaderProgram>& getShader() const;

protected:
    RenderObject(const std::shared_ptr<Camera>& camera, const std::shared_ptr<OpenGLBuffer>& bufferCamData = nullptr) :
        m_Camera(camera), m_UBufferCamData(bufferCamData)
    {
        m_SelfUpdateCamera = (m_UBufferCamData == nullptr);
    }

    virtual void initRenderData() = 0;
    virtual void render()         = 0;


    GLuint m_UBModelMatrix;
    GLuint m_UBCamData;
    GLuint m_VAO;

    bool                           m_SelfUpdateCamera;
    std::shared_ptr<Camera>        m_Camera;
    std::shared_ptr<OpenGLBuffer>  m_UBufferModelMatrix;
    std::shared_ptr<OpenGLBuffer>  m_UBufferCamData;
    std::shared_ptr<ShaderProgram> m_Shader;
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
    SkyBoxRender(const std::shared_ptr<Camera>& camera, QString texureTopFolder, const std::shared_ptr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_CurrentTexture(nullptr)
    {
        initRenderData();
        loadTextures(texureTopFolder);
    }

    void loadTextures(QString textureTopFolder);
#endif

    SkyBoxRender(const std::shared_ptr<Camera>& camera, const std::shared_ptr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_CurrentTexture(nullptr)
    {
        initRenderData();
    }

    SkyBoxRender(const std::shared_ptr<Camera>& camera, const std::vector<std::shared_ptr<OpenGLTexture> >& textures, const std::shared_ptr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_Textures(textures), m_CurrentTexture(nullptr)
    {
        initRenderData();
    }

    const std::shared_ptr<OpenGLTexture>& getCurrentTexture();
    size_t                                getNumTextures();
    void                                  clearTextures();
    void                                  addTexture(const std::shared_ptr<OpenGLTexture>& texture);
    void                                  setRenderTextureIndex(int texIndex);
    void                                  scale(float scaleX, float scaleY, float scaleZ);

    virtual void render() override;

protected:
    virtual void initRenderData() override;

    GLuint                                       m_AtrVPosition;
    GLuint                                       m_UTexSampler;
    std::unique_ptr<CubeObject>                  m_CubeObj;
    std::shared_ptr<OpenGLTexture>               m_CurrentTexture;
    std::vector<std::shared_ptr<OpenGLTexture> > m_Textures;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Light render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PointLightRender : public RenderObject
{
public:
    PointLightRender(const std::shared_ptr<Camera>& camera, const std::shared_ptr<PointLights>& lights, const std::shared_ptr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_Lights(lights), m_RenderSize(20.0)
    {
        initRenderData();
    }

    void         setRenderSize(GLfloat renderSize);
    virtual void render() override;

private:
    virtual void initRenderData() override;

    GLfloat                             m_RenderSize;
    GLuint                              m_UBLight;
    const std::shared_ptr<PointLights>& m_Lights;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WirteFrameBoxRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class WireFrameBoxRender : public RenderObject
{
public:
    WireFrameBoxRender(const std::shared_ptr<Camera>& camera, const std::shared_ptr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_BoxColor(0.5, 0.5, 0)
    {
        initRenderData();
    }

    void         setColor(const glm::vec3& color);
    void         transform(const glm::vec3& translation, const glm::vec3& scale);
    void         setBox(const glm::vec3& boxMin, const glm::vec3& boxMax);
    virtual void render() override;

private:
    virtual void initRenderData() override;

    glm::vec3                           m_BoxColor;
    GLuint                              m_AtrVPosition;
    GLuint                              m_UColor;
    std::unique_ptr<WireFrameBoxObject> m_WireFrameBoxObj;
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
    void         setColorBufferParameter(GLenum paramName, GLenum paramValue);

    std::shared_ptr<OpenGLTexture>& getColorBuffer(int colorBufferID = 0);
    void                            swapColorBuffer(std::shared_ptr<OpenGLTexture>& colorBuffer, int bufferID = 0);
    void                            fastSwapColorBuffer(std::shared_ptr<OpenGLTexture>& colorBuffer, int bufferID = 0);

    virtual void render() override {} // do nothing

protected:
    virtual void initRenderData() override;

    int                                          m_BufferWidth;
    int                                          m_BufferHeight;
    int                                          m_NumColorBuffers;
    GLenum                                       m_FormatColorBuff;
    GLuint                                       m_FrameBufferID;
    GLuint                                       m_RenderBufferID;
    std::vector<std::shared_ptr<OpenGLTexture> > m_ColorBuffers;
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
        OffScreenRender(width, height, 1, GL_R32F), m_ClearLinearDepthValue(-1.0e6), m_DefaultClearColor(glm::vec4(0.8, 0.8, 0.8, 1.0)) {}

    virtual void beginRender() override;
    virtual void endRender(GLuint defaultFBO /* = 0 */) override;

    virtual void setNumColorBuffers(int numColorBuffers) override;

    void                            setDefaultClearColor(const glm::vec4& clearColor);
    void                            setClearDepthValue(GLfloat clearValue);
    std::shared_ptr<OpenGLTexture>& getDepthBuffer();

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

    ScreenQuadTextureRender(std::shared_ptr<OpenGLTexture>& texture, int texelSize = 1) :
        RenderObject(nullptr, nullptr), m_Texture(texture), m_TexelSizeValue(texelSize), m_ValueScale(1.0)
    {
        initRenderData();
    }

    void         setValueScale(float scale);
    void         setTexture(const std::shared_ptr<OpenGLTexture>& texture, int texelSize = 1);
    virtual void render() override;

private:
    virtual void initRenderData() override;

    float                          m_ValueScale;
    int                            m_TexelSizeValue;
    GLuint                         m_UTexelSize;
    GLuint                         m_UValueScale;
    GLuint                         m_UTexSampler;
    std::shared_ptr<OpenGLTexture> m_Texture;
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
    MeshRender(const std::shared_ptr<MeshObject>& meshObj, const std::shared_ptr<Camera>& camera, const std::shared_ptr<PointLights>& light,
               QString textureFolder,
               const std::shared_ptr<Material>& material = nullptr, const std::shared_ptr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_MeshObj(meshObj), m_Lights(light), m_Material(material), m_CurrentTexture(nullptr),
        m_ShadowBufferWidth(1024), m_ShadowBufferHeight(1024), m_Exposure(1.0f)
    {
        initRenderData();
        OpenGLTexture::loadTextures(m_Textures, textureFolder);
    }

    void loadTextures(QString textureFolder);
#endif

    MeshRender(const std::shared_ptr<MeshObject>& meshObj, const std::shared_ptr<Camera>& camera, const std::shared_ptr<PointLights>& light,
               const std::shared_ptr<Material>& material = nullptr, const std::shared_ptr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_MeshObj(meshObj), m_Lights(light), m_Material(material), m_CurrentTexture(nullptr),
        m_ShadowBufferWidth(1024), m_ShadowBufferHeight(1024), m_Exposure(1.0f)
    {
        initRenderData();
    }

    const std::shared_ptr<MeshObject>&    getMeshObj();
    const std::shared_ptr<Material>&      getMaterial();
    const std::shared_ptr<OpenGLTexture>& getCurrentTexture();
    size_t                                getNumTextures();

    void clearTextures(bool insertNullTex = true);
    void addTexture(const std::shared_ptr<OpenGLTexture>& texture, GLenum texWrapMode = GL_REPEAT);
    void setRenderTextureIndex(int texIndex);
    void setExternalShadowMaps(const std::vector<std::shared_ptr<OpenGLTexture> >& shadowMaps);
    void resizeShadowMap(int width, int height);
    void setExposure(float exposure);
    void transform(const glm::vec3& translation, const glm::vec3& scale);
    void setupVAO();

    std::shared_ptr<OpenGLTexture>&              getLightShadowMap(int lightID = 0);
    std::vector<std::shared_ptr<OpenGLTexture> > getAllLightShadowMaps();
    std::shared_ptr<OpenGLTexture>&              getCameraShadowMap();

    virtual void render() override;

    virtual void initDepthBufferData(const glm::vec4& defaultClearColor);
    virtual void renderToLightDepthBuffer(int scrWidth, int scrHeight, GLuint defaultFBO = 0);
    virtual void renderToCameraDepthBuffer(int scrWidth, int scrHeight, GLuint defaultFBO = 0);

protected:
    virtual void initRenderData() override;

    GLuint                                       m_AtrVPosition;
    GLuint                                       m_AtrVNormal;
    GLuint                                       m_AtrVTexCoord;
    GLuint                                       m_UBLight;
    GLuint                                       m_UBLightMatrices;
    GLuint                                       m_LDSULightID;
    GLuint                                       m_UBMaterial;
    GLuint                                       m_UHasTexture;
    GLuint                                       m_UHasShadow;
    GLuint                                       m_UTexSampler;
    GLuint                                       m_UShadowMap[MAX_NUM_LIGHTS];
    GLuint                                       m_UExposure;
    std::shared_ptr<MeshObject>                  m_MeshObj;
    std::shared_ptr<PointLights>                 m_Lights;
    std::shared_ptr<Material>                    m_Material;
    std::vector<std::shared_ptr<OpenGLTexture> > m_Textures;
    std::shared_ptr<OpenGLTexture>               m_CurrentTexture;
    std::vector<std::shared_ptr<OpenGLTexture> > m_ExternalShadowMaps;

    bool    m_DepthBufferInitialized = false;
    GLint   m_ShadowBufferWidth;
    GLint   m_ShadowBufferHeight;
    GLfloat m_Exposure;
    GLuint  m_LDSAtrVPosition;
    GLuint  m_LDSUBLightMatrices;
    GLuint  m_LDSUBModelMatrix;
    GLuint  m_LDSVAO;
    GLuint  m_CDSAtrVPosition;
    GLuint  m_CDSUBModelMatrix;
    GLuint  m_CDSUBCameraData;
    GLuint  m_CDSVAO;

    std::shared_ptr<ShaderProgram>                   m_LightDepthShader;
    std::shared_ptr<ShaderProgram>                   m_CameraDepthShader;
    std::vector<std::unique_ptr<DepthBufferRender> > m_LightDepthBufferRenders;
    std::unique_ptr<DepthBufferRender>               m_CameraDepthBufferRender;
    glm::mat4                                        m_LightView[MAX_NUM_LIGHTS];
    glm::mat4                                        m_LightProjection[MAX_NUM_LIGHTS];
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
    PlaneRender(const std::shared_ptr<Camera>& camera, const std::shared_ptr<PointLights>& light, QString textureFolder,
                const std::shared_ptr<OpenGLBuffer>& bufferCamData = nullptr) :
        MeshRender(std::make_shared<GridObject>(), camera, light, textureFolder, nullptr, bufferCamData),
        m_AllowedNonTexRender(true) {}
#endif

    PlaneRender(const std::shared_ptr<Camera>& camera, const std::shared_ptr<PointLights>& light, const std::shared_ptr<OpenGLBuffer>& bufferCamData = nullptr) :
        MeshRender(std::make_shared<GridObject>(), camera, light, nullptr, bufferCamData),
        m_AllowedNonTexRender(true) {}

    void setAllowNonTextureRender(bool allowNonTex);
    void scaleTexCoord(int scaleX, int scaleY);

    virtual void render() override;

protected:
    bool m_AllowedNonTexRender;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana