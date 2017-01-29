//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <OpenGLHelpers/OpenGLMacros.h>
#include <OpenGLHelpers/Camera.h>
#include <OpenGLHelpers/ShaderProgram.h>
#include <OpenGLHelpers/OpenGLBuffer.h>
#include <OpenGLHelpers/OpenGLTexture.h>
#include <OpenGLHelpers/CubeObject.h>
#include <OpenGLHelpers/GridObject.h>
#include <OpenGLHelpers/Light.h>
#include <OpenGLHelpers/Material.h>

#include <vector>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// abstract base class of object render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RenderObject : public OpenGLCallable
{
protected:
    RenderObject(Camera* camera, OpenGLBuffer* bufferCamData = nullptr) :
        m_Camera(camera), m_UBufferCamData(bufferCamData)
    {}

    virtual void initRenderData() = 0;
    virtual void render() = 0;

    GLuint         m_UModelMatrix;
    GLuint         m_UCamData;
    GLuint         m_VAO;

    Camera*        m_Camera;
    OpenGLBuffer*  m_UBufferModelMatrix;
    OpenGLBuffer*  m_UBufferCamData;
    ShaderProgram* m_Shader;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SkyBox render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class SkyBoxRender : public RenderObject
{
public:
#ifdef __Banana_Qt__
    SkyBoxRender(Camera* camera, QString texureTopFolder,
                 OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData)
    {
        initRenderData();
        loadTextures(texureTopFolder);
    }

    void loadTextures(QString textureTopFolder);
#endif

    SkyBoxRender(Camera* camera, OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData)
    {
        initRenderData();
    }

    SkyBoxRender(Camera* camera, std::vector<OpenGLTexture*> skyboxTextures,
                 OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData),
        m_SkyBoxTextures(skyboxTextures)
    {
        initRenderData();
    }

    void clearTextures();
    void addTexture(OpenGLTexture* texture);
    void setRenderTextureIndex(int texIndex);
    size_t getNumTextures();

    virtual void render() override;

protected:
    virtual void initRenderData() override;

    GLuint          m_AtrVPosition;
    GLuint         m_UTexSampler;

    CubeObject*                 m_CubeObj;
    OpenGLTexture*              m_CurrentSkyBoxTex;
    std::vector<OpenGLTexture*> m_SkyBoxTextures;

};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Floor render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class FloorRender : public RenderObject
{
public:
#ifdef __Banana_Qt__
    FloorRender(Camera* camera, Light* light, QString texureFolder,
                OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_Light(light)
    {
        initRenderData();
        loadTextures(texureFolder);
    }
    void loadTextures(QString textureFolder);
#endif

    FloorRender(Camera* camera, Light* light, OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_Light(light)
    {
        initRenderData();
    }

    FloorRender(Camera* camera, Light* light, std::vector<OpenGLTexture*> floorTextures,
                OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_Light(light),
        m_FloorTextures(floorTextures)
    {
        initRenderData();
    }

    void clearTextures();
    void addTexture(OpenGLTexture* texture);
    void setRenderTextureIndex(int texIndex);
    size_t getNumTextures();

    void transform(const glm::vec3& translation, const glm::vec3& scale);
    void scaleTexCoord(int scaleX, int scaleY);

    virtual void render() override;

private:
    virtual void initRenderData() override;

    GLuint          m_AtrVPosition;
    GLuint          m_AtrVNormal;
    GLuint          m_AtrVTexCoord;
    GLuint         m_UHasTexture;
    GLuint         m_UTexSampler;
    GLuint         m_ULight;
    GLuint         m_UMaterial;

    GridObject*    m_GridObj;
    Light*         m_Light;
    Material*      m_Material;

    OpenGLTexture*              m_CurrentFloorTex;
    std::vector<OpenGLTexture*> m_FloorTextures;

};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Light render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PointLightRender : public RenderObject
{
public:
    PointLightRender(Camera* camera, PointLight* light,
                     OpenGLBuffer* bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData),
        m_Light(light), m_RenderSize(20.0)
    {
        initRenderData();
    }

    void setRenderSize(GLfloat renderSize);
    virtual void render() override;

private:
    virtual void initRenderData() override;

    GLfloat        m_RenderSize;
    GLuint         m_ULight;

    PointLight*    m_Light;
};