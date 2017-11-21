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
#include <OpenGLHelpers/Camera.h>
#include <OpenGLHelpers/ShaderProgram.h>
#include <OpenGLHelpers/OpenGLBuffer.h>
#include <OpenGLHelpers/OpenGLTexture.h>
#include <OpenGLHelpers/MeshObjects/CubeObject.h>
#include <OpenGLHelpers/MeshObjects/GridObject.h>
#include <OpenGLHelpers/MeshObjects/WireFrameBoxObject.h>

#include <OpenGLHelpers/Lights.h>
#include <OpenGLHelpers/Material.h>

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
    const SharedPtr<ShaderProgram>& getShader() const;

protected:
    RenderObject(const SharedPtr<Camera>& camera, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        m_Camera(camera), m_UBufferCamData(bufferCamData)
    {
        m_SelfUpdateCamera = (m_UBufferCamData == nullptr);
    }

    virtual void initRenderData() = 0;
    virtual void render()         = 0;

    GLuint m_UBModelMatrix;
    GLuint m_UBCamData;
    GLuint m_VAO;

    bool                     m_SelfUpdateCamera;
    SharedPtr<Camera>        m_Camera;
    SharedPtr<OpenGLBuffer>  m_UBufferModelMatrix;
    SharedPtr<OpenGLBuffer>  m_UBufferCamData;
    SharedPtr<ShaderProgram> m_Shader;
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
    SkyBoxRender(const SharedPtr<Camera>& camera, QString texureTopFolder, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_CurrentTexture(nullptr)
    {
        initRenderData();
        loadTextures(texureTopFolder);
    }

    void loadTextures(QString textureTopFolder);
#endif

    SkyBoxRender(const SharedPtr<Camera>& camera, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_CurrentTexture(nullptr)
    {
        initRenderData();
    }

    SkyBoxRender(const SharedPtr<Camera>& camera, const Vector<SharedPtr<OpenGLTexture> >& textures, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_Textures(textures), m_CurrentTexture(nullptr)
    {
        initRenderData();
    }

    const SharedPtr<OpenGLTexture>& getCurrentTexture();
    size_t                          getNumTextures();
    void                            clearTextures();
    void                            addTexture(const SharedPtr<OpenGLTexture>& texture);
    void                            setRenderTextureIndex(int texIndex);
    void                            scale(float scaleX, float scaleY, float scaleZ);

    virtual void render() override;

protected:
    virtual void initRenderData() override;

    GLuint                            m_AtrVPosition;
    GLuint                            m_UTexSampler;
    UniquePtr<CubeObject>             m_CubeObj;
    SharedPtr<OpenGLTexture>          m_CurrentTexture;
    Vector<SharedPtr<OpenGLTexture> > m_Textures;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Light render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PointLightRender : public RenderObject
{
public:
    PointLightRender(const SharedPtr<Camera>& camera, const SharedPtr<PointLights>& lights, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_Lights(lights), m_RenderSize(20.0)
    {
        initRenderData();
    }

    void         setRenderSize(GLfloat renderSize);
    virtual void render() override;

private:
    virtual void initRenderData() override;

    GLfloat                       m_RenderSize;
    GLuint                        m_UBLight;
    const SharedPtr<PointLights>& m_Lights;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WirteFrameBoxRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class WireFrameBoxRender : public RenderObject
{
public:
    WireFrameBoxRender(const SharedPtr<Camera>& camera, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_BoxColor(0.5, 0.5, 0)
    {
        initRenderData();
    }

    void         setColor(const Vec3f& color);
    void         transform(const Vec3f& translation, const Vec3f& scale);
    void         setBox(const Vec3f& boxMin, const Vec3f& boxMax);
    virtual void render() override;

private:
    virtual void initRenderData() override;

    Vec3f                         m_BoxColor;
    GLuint                        m_AtrVPosition;
    GLuint                        m_UColor;
    UniquePtr<WireFrameBoxObject> m_WireFrameBoxObj;
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

    SharedPtr<OpenGLTexture>& getColorBuffer(int colorBufferID = 0);
    void                      swapColorBuffer(SharedPtr<OpenGLTexture>& colorBuffer, int bufferID = 0);
    void                      fastSwapColorBuffer(SharedPtr<OpenGLTexture>& colorBuffer, int bufferID = 0);

    virtual void render() override {} // do nothing

protected:
    virtual void initRenderData() override;

    int                               m_BufferWidth;
    int                               m_BufferHeight;
    int                               m_NumColorBuffers;
    GLenum                            m_FormatColorBuff;
    GLuint                            m_FrameBufferID;
    GLuint                            m_RenderBufferID;
    Vector<SharedPtr<OpenGLTexture> > m_ColorBuffers;
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
        OffScreenRender(width, height, 1, GL_R32F), m_ClearLinearDepthValue(-1.0e6), m_DefaultClearColor(Vec3f(0.8, 0.8, 0.8)) {}

    virtual void beginRender() override;
    virtual void endRender(GLuint defaultFBO /* = 0 */) override;

    virtual void setNumColorBuffers(int numColorBuffers) override;

    void                      setDefaultClearColor(const Vec3f& clearColor);
    void                      setClearDepthValue(GLfloat clearValue);
    SharedPtr<OpenGLTexture>& getDepthBuffer();

private:
    virtual void initRenderData() override;

    GLfloat m_ClearLinearDepthValue;
    Vec3f   m_DefaultClearColor;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ScreenQuadTextureRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ScreenQuadTextureRender : public RenderObject
{
public:
    ScreenQuadTextureRender() : RenderObject(nullptr, nullptr), m_Texture(nullptr), m_TexelSizeValue(1), m_ValueScale(1.0) { initRenderData(); }
    ScreenQuadTextureRender(SharedPtr<OpenGLTexture>& texture, int texelSize = 1) :
        RenderObject(nullptr, nullptr), m_Texture(texture), m_TexelSizeValue(texelSize), m_ValueScale(1.0) { initRenderData(); }

    void         setValueScale(float scale);
    void         setTexture(const SharedPtr<OpenGLTexture>& texture, int texelSize = 1);
    virtual void render() override;

private:
    virtual void initRenderData() override;

    float                    m_ValueScale;
    int                      m_TexelSizeValue;
    GLuint                   m_UTexelSize;
    GLuint                   m_UValueScale;
    GLuint                   m_UTexSampler;
    SharedPtr<OpenGLTexture> m_Texture;
};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// CheckerboardBackgroundRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class CheckerboardBackgroundRender : public RenderObject
{
public:
    CheckerboardBackgroundRender() : RenderObject(nullptr, nullptr) { initRenderData(); }
    CheckerboardBackgroundRender(const Vec3f& color1, const Vec3f& color2) : RenderObject(nullptr, nullptr) { initRenderData(); setColors(color1, color2); }
    CheckerboardBackgroundRender(const Vec4f& color1, const Vec4f& color2) : RenderObject(nullptr, nullptr) { initRenderData(); setColors(color1, color2); }

    void         setTexScales(Vec2f texScales) { m_TexScales = texScales; }
    void         setScreenSize(int width, int height) { m_ScreenWidth = width; m_ScreenHeight = height; }
    void         setColor1(const Vec3f& color1) { m_Color1 = Vec4f(color1, 1.0); }
    void         setColor1(const Vec4f& color1) { m_Color1 = color1; }
    void         setColor2(const Vec3f& color2) { m_Color2 = Vec4f(color2, 1.0); }
    void         setColor2(const Vec4f& color2) { m_Color2 = color2; }
    void         setColors(const Vec3f& color1, const Vec3f& color2) { setColor1(color1); setColor2(color2); }
    void         setColors(const Vec4f& color1, const Vec4f& color2) { setColor1(color1); setColor2(color2); }
    virtual void render() override;

private:
    virtual void initRenderData() override;

    Vec2f m_TexScales    = Vec2f(0.01);
    GLint m_ScreenWidth  = 1;
    GLint m_ScreenHeight = 1;
    Vec4f m_Color1       = Vec4f(1);
    Vec4f m_Color2       = Vec4f(0, 0, 0, 1);

    GLuint m_UTexScales;
    GLuint m_UScreenWidth;
    GLuint m_UScreenHeight;
    GLuint m_UColor1;
    GLuint m_UColor2;
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
    MeshRender(const SharedPtr<MeshObject>& meshObj, const SharedPtr<Camera>& camera, const SharedPtr<PointLights>& light,
               QString textureFolder,
               const SharedPtr<Material>& material = nullptr, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_MeshObj(meshObj), m_Lights(light), m_Material(material), m_CurrentTexture(nullptr),
        m_ShadowBufferWidth(1024), m_ShadowBufferHeight(1024), m_Exposure(1.0f)
    {
        initRenderData();
        OpenGLTexture::loadTextures(m_Textures, textureFolder);
    }

    void loadTextures(QString textureFolder);
#endif

    MeshRender(const SharedPtr<MeshObject>& meshObj, const SharedPtr<Camera>& camera, const SharedPtr<PointLights>& light,
               const SharedPtr<Material>& material = nullptr, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_MeshObj(meshObj), m_Lights(light), m_Material(material), m_CurrentTexture(nullptr),
        m_ShadowBufferWidth(1024), m_ShadowBufferHeight(1024), m_Exposure(1.0f)
    {
        initRenderData();
    }

    const SharedPtr<MeshObject>&    getMeshObj();
    const SharedPtr<Material>&      getMaterial();
    const SharedPtr<OpenGLTexture>& getCurrentTexture();
    size_t                          getNumTextures();

    void clearTextures(bool insertNullTex = true);
    void addTexture(const SharedPtr<OpenGLTexture>& texture, GLenum texWrapMode = GL_REPEAT);
    void setRenderTextureIndex(int texIndex);
    void setExternalShadowMaps(const Vector<SharedPtr<OpenGLTexture> >& shadowMaps);
    void resizeShadowMap(int width, int height);
    void setExposure(float exposure);
    void transform(const Vec3f& translation, const Vec3f& scale);
    void setupVAO();

    SharedPtr<OpenGLTexture>&         getLightShadowMap(int lightID = 0);
    Vector<SharedPtr<OpenGLTexture> > getAllLightShadowMaps();
    SharedPtr<OpenGLTexture>&         getCameraShadowMap();

    virtual void render() override;

    virtual void initDepthBufferData(const Vec3f& defaultClearColor);
    virtual void renderToLightDepthBuffer(int scrWidth, int scrHeight, GLuint defaultFBO = 0);
    virtual void renderToCameraDepthBuffer(int scrWidth, int scrHeight, GLuint defaultFBO = 0);

protected:
    virtual void initRenderData() override;

    GLuint                            m_AtrVPosition;
    GLuint                            m_AtrVNormal;
    GLuint                            m_AtrVTexCoord;
    GLuint                            m_UBLight;
    GLuint                            m_UBLightMatrices;
    GLuint                            m_LDSULightID;
    GLuint                            m_UBMaterial;
    GLuint                            m_UHasTexture;
    GLuint                            m_UHasShadow;
    GLuint                            m_UTexSampler;
    GLuint                            m_UShadowMap[MAX_NUM_LIGHTS];
    GLuint                            m_UExposure;
    SharedPtr<MeshObject>             m_MeshObj;
    SharedPtr<PointLights>            m_Lights;
    SharedPtr<Material>               m_Material;
    Vector<SharedPtr<OpenGLTexture> > m_Textures;
    SharedPtr<OpenGLTexture>          m_CurrentTexture;
    Vector<SharedPtr<OpenGLTexture> > m_ExternalShadowMaps;

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

    SharedPtr<ShaderProgram>              m_LightDepthShader;
    SharedPtr<ShaderProgram>              m_CameraDepthShader;
    Vector<UniquePtr<DepthBufferRender> > m_LightDepthBufferRenders;
    UniquePtr<DepthBufferRender>          m_CameraDepthBufferRender;
    Mat4x4f                               m_LightView[MAX_NUM_LIGHTS];
    Mat4x4f                               m_LightProjection[MAX_NUM_LIGHTS];
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
    PlaneRender(const SharedPtr<Camera>& camera, const SharedPtr<PointLights>& light, QString textureFolder,
                const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        MeshRender(std::make_shared<GridObject>(), camera, light, textureFolder, nullptr, bufferCamData),
        m_AllowedNonTexRender(true) {}
#endif

    PlaneRender(const SharedPtr<Camera>& camera, const SharedPtr<PointLights>& light, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
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