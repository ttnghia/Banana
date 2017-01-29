//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SkyBox render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class SkyBoxRender : public OpenGLCallable
{
public:
#ifdef __Banana_Qt__
    SkyBoxRender(Camera* camera, QString texureTopFolder,
                 OpenGLBuffer* bufferMatrices = nullptr);

    void loadTextures(QString textureTopFolder);
#endif

    SkyBoxRender(Camera* camera, OpenGLBuffer* bufferMatrices = nullptr) :
        m_Camera(camera), m_UBufferMatrices(bufferMatrices)
    {
        initRenderData();
    }

    SkyBoxRender(Camera* camera, std::vector<OpenGLTexture*> skyboxTextures,
                 OpenGLBuffer* bufferMatrices = nullptr) :
        m_Camera(camera), m_SkyBoxTextures(skyboxTextures), m_UBufferMatrices(bufferMatrices)
    {
        initRenderData();
    }

    void clearTextures();
    void addTexture(OpenGLTexture* texture);
    void setRenderTextureIndex(int texIndex);
    size_t getNumTextures();

    void render();

private:
    void initRenderData();

    GLint          m_AtrVPosition;
    GLuint         m_UCamPosition;
    GLuint         m_UTexSampler;
    GLuint         m_UMatrices;
    GLuint         m_VAO;
    CubeObject*    m_CubeObj;
    Camera*        m_Camera;
    OpenGLBuffer*  m_UBufferMatrices;
    ShaderProgram* m_Shader;

    OpenGLTexture*              m_CurrentSkyBoxTex;
    std::vector<OpenGLTexture*> m_SkyBoxTextures;

};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Floor render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class FloorRender : public OpenGLCallable
{
public:
#ifdef __Banana_Qt__
    FloorRender(Camera* camera, Light* light, QString texureFolder,
                OpenGLBuffer* bufferMatrices = nullptr);
    void loadTextures(QString textureFolder);
#endif

    FloorRender(Camera* camera, Light* light, OpenGLBuffer* bufferMatrices = nullptr) :
        m_Camera(camera), m_Light(light), m_UBufferMatrices(bufferMatrices)
    {
        initRenderData();
    }

    FloorRender(Camera* camera, Light* light, std::vector<OpenGLTexture*> floorTextures,
                OpenGLBuffer* bufferMatrices = nullptr) :
        m_Camera(camera), m_FloorTextures(floorTextures), m_Light(light),
        m_UBufferMatrices(bufferMatrices)
    {
        initRenderData();
    }

    void clearTextures();
    void addTexture(OpenGLTexture* texture);
    void setRenderTextureIndex(int texIndex);
    size_t getNumTextures();

    void transform(const glm::vec3& translation, const glm::vec3& scale);
    void scaleTexCoord(int scaleX, int scaleY);

    void render();

private:
    void initRenderData();

    GLint          m_AtrVPosition;
    GLint          m_AtrVNormal;
    GLint          m_AtrVTexCoord;
    GLuint         m_UCamPosition;
    GLuint         m_UHasTexture;
    GLuint         m_UTexSampler;
    GLuint         m_UMatrices;
    GLuint         m_ULight;
    GLuint         m_UMaterial;
    GLuint         m_VAO;
    GridObject*    m_GridObj;
    Camera*        m_Camera;
    OpenGLBuffer*  m_UBufferMatrices;
    ShaderProgram* m_Shader;
    Light*         m_Light;
    Material*      m_Material;
    glm::mat4      m_ModelMatrix;

    OpenGLTexture*              m_CurrentFloorTex;
    std::vector<OpenGLTexture*> m_FloorTextures;

};


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Light render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PointLightRender : public OpenGLCallable
{
public:
    PointLightRender(Camera* camera, PointLight* light,
                     OpenGLBuffer* bufferMatrices = nullptr) :
        m_Camera(camera), m_Light(light), m_RenderSize(20.0),
        m_UBufferMatrices(bufferMatrices)
    {
        initRenderData();
    }

    void setRenderSize(GLfloat renderSize);
    void render();

private:
    void initRenderData();

    GLuint         m_UMatrices;
    GLuint         m_ULight;
    GLuint         m_VAO;
    Camera*        m_Camera;
    OpenGLBuffer*  m_UBufferMatrices;
    ShaderProgram* m_Shader;
    PointLight*    m_Light;
    GLfloat        m_RenderSize;
};