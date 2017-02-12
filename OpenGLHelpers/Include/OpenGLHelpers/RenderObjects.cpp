﻿//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

#include <OpenGLHelpers/RenderObjects.h>

#include <future>
#include <vector>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SkyBox render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __Banana_Qt__
#include <QDir>
void SkyBoxRender::loadTextures(QString textureTopFolder)
{
    QStringList texFaces =
    {
        "/posx.", "/negx.",
        "/posy.", "/negy.",
        "/posz.", "/negz."
    };

    clearTextures();
    QDir dataDir(textureTopFolder);
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    QStringList allTexFolders = dataDir.entryList();

    std::vector<QImage> textureImages;
    textureImages.resize(allTexFolders.count() * 6);
    std::vector<int> loadSuccess;
    loadSuccess.assign(allTexFolders.count(), 0);
    std::vector<std::future<void> > futureObjs;

    for(int i = 0; i < allTexFolders.count(); ++i)
    {
        QString texFolderPath = textureTopFolder + "/" + allTexFolders[i];
        futureObjs.emplace_back(std::async(std::launch::async, [&, texFolderPath, i]()
        {
            ////////////////////////////////////////////////////////////////////////////////
            // find the extension of texture imageg
            QString posXFilePath = texFolderPath + "/posx.jpg";
            QString ext = "jpg";

            if(!QFile::exists(posXFilePath))
            {
                ext = "png";
                posXFilePath = texFolderPath + "/posx.png";
                if(!QFile::exists(posXFilePath))
                {
                    return; // only support .png and .jpg
                }
            }

            ////////////////////////////////////////////////////////////////////////////////
            // check if all 6 faces exist
            bool check = true;
            for(GLuint j = 0; j < 6; ++j)
            {
                QString texFilePath = texFolderPath + texFaces[j] + ext;
                if(!QFile::exists(texFilePath))
                {
                    check = false;
                    break;
                }
            }

            if(!check)
            {
                return;
            }

            ////////////////////////////////////////////////////////////////////////////////
            // load the textures
            loadSuccess[i] = 1;

            for(GLuint j = 0; j < 6; ++j)
            {
                QString texFilePath = texFolderPath + texFaces[j] + ext;
                textureImages[i * 6 + j] = QImage(texFilePath).convertToFormat(QImage::Format_RGBA8888);
            }
        }));
    }

    for(std::future<void>& f : futureObjs)
    {
        if(f.valid())
            f.wait();
    }

    for(int i = 0; i < allTexFolders.count(); ++i)
    {
        if(loadSuccess[i] == 0)
            continue;

        OpenGLTexture* skyboxTex = new OpenGLTexture(GL_TEXTURE_CUBE_MAP);
        for(GLuint j = 0; j < 6; ++j)
        {
            const QImage& texImg = textureImages[i * 6 + j];
            skyboxTex->uploadData(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, GL_RGBA, texImg.width(), texImg.height(), GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());
        }

        skyboxTex->setBestParametersNoMipMap();
        addTexture(skyboxTex);
    }
}
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::clearTextures()
{
    for(OpenGLTexture* tex : m_Textures)
    {
        delete tex;
    }

    m_Textures.resize(0);
    m_Textures.push_back(nullptr);
    m_CurrentTexture = nullptr;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::addTexture(OpenGLTexture* texture)
{
    texture->setFilterMode(GL_TEXTURE_WRAP_S, GL_REPEAT);
    texture->setFilterMode(GL_TEXTURE_WRAP_T, GL_REPEAT);
    texture->setFilterMode(GL_TEXTURE_WRAP_R, GL_REPEAT);

    m_Textures.push_back(texture);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::setRenderTextureIndex(int texIndex)
{
    if(texIndex < 0)
        return;

    assert(static_cast<size_t>(texIndex) < m_Textures.size());

    m_CurrentTexture = m_Textures[texIndex];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLTexture* SkyBoxRender::getCurrentTexture()
{
    return m_CurrentTexture;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t SkyBoxRender::getNumTextures()
{
    return m_Textures.size();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::render()
{
    assert(m_Camera != nullptr && m_UBufferCamData != nullptr);
    if(m_CurrentTexture == nullptr)
    {
        return;
    }

    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()), 0, sizeof(glm::mat4));
    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()), sizeof(glm::mat4), sizeof(glm::mat4));
    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->m_CameraPosition), 5 * sizeof(glm::mat4), sizeof(glm::vec3));

    m_Shader->bind();
    m_CurrentTexture->bind();


    m_UBufferModelMatrix->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBModelMatrix, m_UBufferModelMatrix->getBindingPoint());

    m_UBufferCamData->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBCamData, m_UBufferCamData->getBindingPoint());

    m_Shader->setUniformValue(m_UTexSampler, 0);


    glCall(glBindVertexArray(m_VAO));
    m_CubeObj->draw();
    m_CurrentTexture->release();
    m_Shader->release();
    glCall(glBindVertexArray(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::initRenderData()
{
    m_Shader = ShaderProgram::getSkyBoxShader();

    m_AtrVPosition = m_Shader->getAtributeLocation("v_Position");
    m_UTexSampler = m_Shader->getUniformLocation("u_TexSampler");

    m_UBModelMatrix = m_Shader->getUniformBlockIndex("ModelMatrix");
    m_UBCamData = m_Shader->getUniformBlockIndex("CameraData");

    ////////////////////////////////////////////////////////////////////////////////
    // cube object
    m_CubeObj = new CubeObject;
    m_CubeObj->uploadDataToGPU();

    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glBindVertexArray(m_VAO));
    m_CubeObj->getVertexBuffer()->bind();
    m_CubeObj->getIndexBuffer()->bind();
    glCall(glVertexAttribPointer(m_AtrVPosition, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    glCall(glEnableVertexAttribArray(m_AtrVPosition));
    glCall(glBindVertexArray(0));

    ////////////////////////////////////////////////////////////////////////////////
    // uniform buffer
    m_UBufferModelMatrix = new OpenGLBuffer;
    m_UBufferModelMatrix->createBuffer(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

    glm::mat4 modelMatrix= glm::scale(glm::mat4(1.0f), glm::vec3(500.0f));
    m_UBufferModelMatrix->uploadData(glm::value_ptr(modelMatrix), 0, sizeof(glm::mat4));

    if(m_UBufferCamData == nullptr)
    {
        m_UBufferCamData = new OpenGLBuffer;
        m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER, 5 * sizeof(glm::mat4) + sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // default null texture
    m_Textures.push_back(nullptr);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Light render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLightRender::setRenderSize(GLfloat renderSize)
{
    m_RenderSize = renderSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLightRender::render()
{
    assert(m_Camera != nullptr && m_UBufferCamData != nullptr);

    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()), 0, sizeof(glm::mat4));
    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()), sizeof(glm::mat4), sizeof(glm::mat4));

    m_Shader->bind();

    m_UBufferCamData->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBCamData, m_UBufferCamData->getBindingPoint());

    m_Lights->bindUniformBuffer();
    m_Shader->bindUniformBlock(m_UBLight, m_Lights->getBufferBindingPoint());

    glCall(glBindVertexArray(m_VAO));
    glCall(glPointSize(m_RenderSize));
    glCall(glEnable(GL_VERTEX_PROGRAM_POINT_SIZE));
    glCall(glDrawArrays(GL_POINTS, 0, m_Lights->getNumLights()));
    glCall(glBindVertexArray(0));
    m_Shader->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLightRender::initRenderData()
{
    const GLchar* vertexShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "layout(std140) uniform CameraData\n"
        "{\n"
        "    mat4 viewMatrix;\n"
        "    mat4 projectionMatrix;\n"
        "    mat4 invViewMatrix;\n"
        "    mat4 invProjectionMatrix;\n"
        "    mat4 shadowMatrix;\n"
        "    vec4 camPosition;\n"
        "};\n"
        "\n"
        "#define NUM_TOTAL_LIGHTS 8\n"
        "struct PointLight\n"
        "{\n"
        "    vec4 ambient; \n"
        "    vec4 diffuse; \n"
        "    vec4 specular; \n"
        "    vec4 position; \n"
        "};\n"

        "layout(std140) uniform Lights\n"
        "{\n"
        "    PointLight lights[NUM_TOTAL_LIGHTS]; \n"
        "    int u_NumLights; \n"
        "}; \n"
        "\n"
        "out vec3 f_lightDiffuse;\n"
        "void main()\n"
        "{\n"
        "    vec4 worldCoord = vec4(vec3(lights[gl_VertexID].position), 1.0);\n"
        "    gl_Position = projectionMatrix * viewMatrix * worldCoord;\n"
        "    f_lightDiffuse = vec3(lights[gl_VertexID].diffuse);\n"
        "}";

    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "in vec3 f_lightDiffuse;\n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    outColor = vec4(vec3(1.0 - max(max(f_lightDiffuse.x, f_lightDiffuse.y), f_lightDiffuse.z)) + f_lightDiffuse, 1.0);\n"
        "}\n";

    m_Shader = new ShaderProgram(std::string("LightRenderShader"));
    m_Shader->addVertexShaderFromSource(vertexShader);
    m_Shader->addFragmentShaderFromSource(fragmentShader);
    m_Shader->link();

    glCall(glGenVertexArrays(1, &m_VAO));

    m_UBCamData = m_Shader->getUniformBlockIndex("CameraData");
    m_UBLight = m_Shader->getUniformBlockIndex("Lights");


    ////////////////////////////////////////////////////////////////////////////////
    // uniform buffer
    if(m_UBufferCamData == nullptr)
    {
        m_UBufferCamData = new OpenGLBuffer;
        m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER, 5 * sizeof(glm::mat4) + sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WireFrameBoxRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WireFrameBoxRender::transform(const glm::vec3 & translation, const glm::vec3 & scale)
{
    glm::mat4 modelMatrix = glm::scale(glm::translate(glm::mat4(1.0), translation), scale);
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

    m_UBufferModelMatrix->uploadData(glm::value_ptr(modelMatrix), 0, sizeof(glm::mat4)); // model matrix
    m_UBufferModelMatrix->uploadData(glm::value_ptr(normalMatrix), sizeof(glm::mat4), sizeof(glm::mat4)); // normal matrix}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WireFrameBoxRender::setBox(const glm::vec3& boxMin, const glm::vec3& boxMax)
{
    m_WireFrameBoxObj->setBox(boxMin, boxMax);
    m_WireFrameBoxObj->uploadDataToGPU();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WireFrameBoxRender::render()
{
    m_Shader->bind();

    m_UBufferModelMatrix->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBModelMatrix, m_UBufferModelMatrix->getBindingPoint());

    if(m_SelfUpdateCamera)
    {
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()), 0, sizeof(glm::mat4));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()), sizeof(glm::mat4), sizeof(glm::mat4));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->m_CameraPosition), 5 * sizeof(glm::mat4), sizeof(glm::vec3));
    }
    m_UBufferCamData->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBCamData, m_UBufferCamData->getBindingPoint());

    m_Shader->setUniformValue(m_UColor, m_BoxColor);

    glCall(glBindVertexArray(m_VAO));
    m_WireFrameBoxObj->draw();
    m_Shader->release();
    glCall(glBindVertexArray(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WireFrameBoxRender::initRenderData()
{
    m_Shader = ShaderProgram::getObjUniformColorShader();

    m_AtrVPosition = m_Shader->getAtributeLocation("v_Position");
    m_UColor       = m_Shader->getUniformLocation("f_Color");

    m_UBModelMatrix = m_Shader->getUniformBlockIndex("ModelMatrix");
    m_UBCamData     = m_Shader->getUniformBlockIndex("CameraData");


    m_WireFrameBoxObj = new WireFrameBoxObject;
    m_WireFrameBoxObj->uploadDataToGPU();

    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glBindVertexArray(m_VAO));
    m_WireFrameBoxObj->getVertexBuffer()->bind();
    m_WireFrameBoxObj->getIndexBuffer()->bind();
    glCall(glVertexAttribPointer(m_AtrVPosition, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    glCall(glEnableVertexAttribArray(m_AtrVPosition));
    glCall(glBindVertexArray(0));

    ////////////////////////////////////////////////////////////////////////////////
    // uniform buffer
    if(m_UBufferCamData == nullptr)
    {
        m_UBufferCamData = new OpenGLBuffer;
        m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER, 5 * sizeof(glm::mat4) + sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    }

    m_UBufferModelMatrix = new OpenGLBuffer;
    m_UBufferModelMatrix->createBuffer(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    transform(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// OffScreenRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::resize(int width, int height)
{
    assert(m_DepthStencilBuffer != nullptr);

    m_BufferWidth  = width;
    m_BufferHeight = height;

    m_DepthStencilBuffer->uploadData(GL_TEXTURE_2D, m_FormatDepthStencilBuff, m_BufferWidth, m_BufferHeight,
                                     m_hasStencilBuffer ? GL_DEPTH_STENCIL : GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);

    // color attachment
    for(int i = 0; i < m_NumColorBuffers; ++i)
    {
        assert(m_ColorBuffers[i] != nullptr);
        m_ColorBuffers[i]->uploadData(GL_TEXTURE_2D, m_FormatColorBuff, m_BufferWidth, m_BufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::beginRender()
{
    glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID));
    glCall(glViewport(0, 0, m_BufferWidth, m_BufferHeight));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::endRender(GLuint defaultFBO /*= 0*/)
{
    glCall(glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::setNumColorBuffers(int numColorBuffers)
{
    if(m_NumColorBuffers == numColorBuffers)
    {
        return;
    }
    else
    {
        glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID));

        if(m_NumColorBuffers > numColorBuffers)
        {
            for(int i = numColorBuffers; i < m_NumColorBuffers; ++i)
            {
                glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0));
            }
        }
        else // m_NumColorBuffers < numColorBuffers
        {
            for(int i = m_ColorBuffers.size(); i < numColorBuffers; ++i)
            {
                OpenGLTexture * tex = new OpenGLTexture(GL_TEXTURE_2D);
                tex->setFilterMode(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                tex->setFilterMode(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                tex->uploadData(GL_TEXTURE_2D, m_FormatColorBuff, m_BufferWidth, m_BufferHeight, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

                m_ColorBuffers.push_back(tex);
            }

            for(int i = m_NumColorBuffers; i < numColorBuffers; ++i)
                glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorBuffers[i]->getTextureID(), 0));
        }

        if(numColorBuffers == 0)
        {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }

        // check error
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
#ifdef __Banana_Qt__
            __BNN_Die(QString("OffScreenRender: FrameBuffer is incomplete!"));
#else
            __BNN_Die("%s: FrameBuffer is incomplete!\n", m_Shader->getProgramName());
#endif
        }

        glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        m_NumColorBuffers = numColorBuffers;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::setColorBufferFilterModes(GLenum minFilter, GLenum magFiliter)
{
    for(OpenGLTexture* tex : m_ColorBuffers)
    {
        tex->setFilterMode(GL_TEXTURE_MIN_FILTER, minFilter);
        tex->setFilterMode(GL_TEXTURE_MAG_FILTER, magFiliter);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLTexture* OffScreenRender::getDepthStencilBuffer()
{
    return m_DepthStencilBuffer;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLTexture* OffScreenRender::getColorBuffer(int colorBufferID /*= 0*/)
{
    assert(colorBufferID < m_NumColorBuffers);
    return m_ColorBuffers[colorBufferID];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::swapDepthStencilBuffer(OpenGLTexture*& depthStencil)
{
    OpenGLTexture* tmp   = m_DepthStencilBuffer;
    m_DepthStencilBuffer = depthStencil;
    depthStencil         = tmp;
    tmp                  = nullptr;

    glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID));
    glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, m_hasStencilBuffer ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
           m_DepthStencilBuffer->getTextureID(), 0));
    // check error
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
#ifdef __Banana_Qt__
        __BNN_Die(QString("OffScreenRender: FrameBuffer is incomplete!"));
#else
        __BNN_Die("%s: FrameBuffer is incomplete!\n", m_Shader->getProgramName());
#endif
    }

    glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::swapColorBuffer(OpenGLTexture*& colorBuffer, int bufferID)
{
    OpenGLTexture* tmp       = m_ColorBuffers[bufferID];
    m_ColorBuffers[bufferID] = colorBuffer;
    colorBuffer              = tmp;
    tmp                      = nullptr;

    glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID));
    glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + bufferID, GL_TEXTURE_2D, m_ColorBuffers[bufferID]->getTextureID(), 0));
    // check error
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
#ifdef __Banana_Qt__
        __BNN_Die(QString("OffScreenRender: FrameBuffer is incomplete!"));
#else
        __BNN_Die("%s: FrameBuffer is incomplete!\n", m_Shader->getProgramName());
#endif
    }

    glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::initRenderData()
{
    glCall(glGenFramebuffers(1, &m_FrameBufferID));
    glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID));

    // depth-stencil attachment
    m_DepthStencilBuffer = new OpenGLTexture(GL_TEXTURE_2D);
    m_DepthStencilBuffer->setFilterMode(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    m_DepthStencilBuffer->setFilterMode(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_DepthStencilBuffer->uploadData(GL_TEXTURE_2D, m_FormatDepthStencilBuff, m_BufferWidth, m_BufferHeight,
                                     m_hasStencilBuffer ? GL_DEPTH_STENCIL : GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
    //m_DepthStencilBuffer->setBestParameters();

    glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, m_hasStencilBuffer ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
           m_DepthStencilBuffer->getTextureID(), 0));

    // color attachment
    for(int i = 0; i < m_NumColorBuffers; ++i)
    {
        OpenGLTexture * tex = new OpenGLTexture(GL_TEXTURE_2D);
        tex->setFilterMode(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        tex->setFilterMode(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        tex->uploadData(GL_TEXTURE_2D, m_FormatColorBuff, m_BufferWidth, m_BufferHeight, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, tex->getTextureID(), 0));
        m_ColorBuffers.push_back(tex);
    }

    if(m_NumColorBuffers == 0)
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    // check error
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
#ifdef __Banana_Qt__
        __BNN_Die(QString("OffScreenRender: FrameBuffer is incomplete!"));
#else
        __BNN_Die("%s: FrameBuffer is incomplete!\n", m_Shader->getProgramName());
#endif
    }

    glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// DepthBufferRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void DepthBufferRender::beginRender()
{
    OffScreenRender::beginRender();

    glClearColor(m_ClearLinearDepthValue, m_ClearLinearDepthValue, m_ClearLinearDepthValue, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DepthBufferRender::endRender(GLuint defaultFBO /* = 0 */)
{
    OffScreenRender::endRender(defaultFBO);

    glClearColor(m_DefaultClearColor[0], m_DefaultClearColor[1], m_DefaultClearColor[2], m_DefaultClearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DepthBufferRender::setDefaultClearColor(const glm::vec4& clearColor)
{
    m_DefaultClearColor = clearColor;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DepthBufferRender::setClearLinearDepthValue(GLfloat clearValue)
{
    m_ClearLinearDepthValue = clearValue;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLTexture * DepthBufferRender::getLinearDepthBuffer()
{
    return m_bLinearDepthBuffer ? m_ColorBuffers[0] : nullptr;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLTexture * DepthBufferRender::getDepthBuffer()
{
    return m_DepthStencilBuffer;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ScreenQuadTextureRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ScreenQuadTextureRender::render()
{
    assert(m_Texture != nullptr);

    m_Texture->bind();
    m_Shader->bind();
    m_Shader->setUniformValue(m_UTexSampler, 0);
    m_Shader->setUniformValue(m_UValueScale, m_ValueScale);
    m_Shader->setUniformValue(m_UTexelSize, m_TexelSizeValue);

    glCall(glBindVertexArray(m_VAO));
    glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4););
    glCall(glBindVertexArray(0));
    m_Shader->release();
    m_Texture->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ScreenQuadTextureRender::initRenderData()
{
    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "uniform int u_TexelSize;\n"
        "uniform float u_ValueScale;\n"
        "uniform sampler2D u_TexSampler;\n"
        "\n"
        "in vec2 f_TexCoord;\n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    outColor = vec4(0, 0, 0, 1);\n"
        "\n"
        "    switch(u_TexelSize)\n"
        "    {\n"
        "        case 1:\n"
        "            outColor = vec4(vec3(u_ValueScale * texture(u_TexSampler, f_TexCoord).x), 1.0);\n"
        "            break;\n"
        "        case 2:\n"
        "            outColor.xy = u_ValueScale * texture(u_TexSampler, f_TexCoord).xy;\n"
        "            break;\n"
        "        case 3:\n"
        "            outColor.xyz = u_ValueScale * texture(u_TexSampler, f_TexCoord).xyz;\n"
        "            break;\n"
        "        case 4:\n"
        "            outColor = u_ValueScale * texture(u_TexSampler, f_TexCoord);\n"
        "            break;\n"
        "        default:\n"
        "            break;\n"
        "    }\n"
        "}\n";

    m_Shader = ShaderProgram::getScreenQuadShader(fragmentShader, "ScreenQuadRender");

    glCall(glGenVertexArrays(1, &m_VAO));

    m_UTexSampler = m_Shader->getUniformLocation("u_TexSampler");
    m_UValueScale = m_Shader->getUniformLocation("u_ValueScale");
    m_UTexelSize  = m_Shader->getUniformLocation("u_TexelSize");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ScreenQuadTextureRender::setValueScale(float scale)
{
    m_ValueScale = scale;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ScreenQuadTextureRender::setTexture(OpenGLTexture * texture, int texelSize /*= 1*/)
{
    m_Texture        = texture;
    m_TexelSizeValue = texelSize;
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Mesh render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __Banana_Qt__
void MeshRender::loadTexture(QString textureFolder)
{
    OpenGLTexture::loadTextures(m_Textures, textureFolder);
}
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MeshObject * MeshRender::getMeshObj()
{
    return m_MeshObj;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Material * MeshRender::getMaterial()
{
    return m_Material;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLTexture * MeshRender::getCurrentTexture()
{
    return m_CurrentTexture;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t MeshRender::getNumTextures()
{
    return m_Textures.size();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::clearTextures(bool insertNullTex /*= true*/)
{
    for(OpenGLTexture* tex : m_Textures)
    {
        delete tex;
    }

    m_Textures.resize(0);
    if(insertNullTex)
    {
        m_Textures.push_back(nullptr);
    }

    m_CurrentTexture = nullptr;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::addTexture(OpenGLTexture * texture, GLenum texWrapMode /*= GL_REPEAT*/)
{
    texture->setFilterMode(GL_TEXTURE_WRAP_S, texWrapMode);
    texture->setFilterMode(GL_TEXTURE_WRAP_T, texWrapMode);
    texture->setFilterMode(GL_TEXTURE_WRAP_R, texWrapMode);
    m_Textures.push_back(texture);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::setRenderTextureIndex(int texIndex)
{
    if(texIndex < 0)
        return;

    assert(static_cast<size_t>(texIndex) < m_Textures.size());
    m_CurrentTexture = m_Textures[texIndex];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::setExternalShadowMap(OpenGLTexture* shadowMap)
{
    m_ExternalShadowMap = shadowMap;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::transform(const glm::vec3 & translation, const glm::vec3 & scale)
{
    glm::mat4 modelMatrix = glm::scale(glm::translate(glm::mat4(1.0), translation), scale);
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

    m_UBufferModelMatrix->uploadData(glm::value_ptr(modelMatrix), 0, sizeof(glm::mat4)); // model matrix
    m_UBufferModelMatrix->uploadData(glm::value_ptr(normalMatrix), sizeof(glm::mat4), sizeof(glm::mat4)); // normal matrix
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::render()
{
    assert(m_MeshObj != nullptr && m_Camera != nullptr && m_UBufferCamData != nullptr);
    if(m_MeshObj->isEmpty())
        return;

    if(m_SelfUpdateCamera)
    {
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()), 0, sizeof(glm::mat4));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()), sizeof(glm::mat4), sizeof(glm::mat4));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->m_CameraPosition), 5 * sizeof(glm::mat4), sizeof(glm::vec3));
    }

    m_Shader->bind();
    if(m_CurrentTexture != nullptr)
    {
        m_CurrentTexture->bind();
        m_Shader->setUniformValue(m_UHasTexture, 1);
        m_Shader->setUniformValue(m_UTexSampler, 0);
    }
    else
    {
        m_Shader->setUniformValue(m_UHasTexture, 0);
    }

    if(m_ExternalShadowMap != nullptr)
        m_ExternalShadowMap->bind(1);

    m_UBufferModelMatrix->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBModelMatrix, m_UBufferModelMatrix->getBindingPoint());

    m_UBufferCamData->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBCamData, m_UBufferCamData->getBindingPoint());

    m_Lights->bindUniformBuffer();
    m_Shader->bindUniformBlock(m_UBLight, m_Lights->getBufferBindingPoint());

    m_Material->bindUniformBuffer();
    m_Shader->bindUniformBlock(m_UBMaterial, m_Material->getBufferBindingPoint());

    glCall(glBindVertexArray(m_VAO));
    m_MeshObj->draw();
    glCall(glBindVertexArray(0));

    if(m_CurrentTexture != nullptr)
    {
        m_CurrentTexture->release();
    }
    if(m_ExternalShadowMap != nullptr)
        m_ExternalShadowMap->release();
    m_Shader->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::renderToDepthBuffer(int scrWidth, int scrHeight, GLuint defaultFBO /*= 0*/)
{
    assert(m_MeshObj != nullptr && m_Camera != nullptr && m_UBufferCamData != nullptr);
    assert(m_Lights != nullptr && m_Lights->getNumLights() > 0);
    assert(m_DepthBufferRenders.size() != 0);

    if(m_MeshObj->isEmpty())
        return;

    ////////////////////////////////////////////////////////////////////////////////
    // update the depth render objs in case the number of lights has changed
    for(int i = m_DepthBufferRenders.size(); i < m_Lights->getNumLights(); ++i)
    {
        m_DepthBufferRenders.push_back(new DepthBufferRender(m_ShadowBufferWidth, m_ShadowBufferHeight));
    }

    m_DepthShader->bind();
    m_UBufferModelMatrix->bindBufferBase();
    m_DepthShader->bindUniformBlock(m_DSUBModelMatrix, m_UBufferModelMatrix->getBindingPoint());

    m_UBufferLightMatrix->bindBufferBase();
    m_DepthShader->bindUniformBlock(m_DSUBLightMatrix, m_UBufferLightMatrix->getBindingPoint());

    GLfloat aspect = (GLfloat)m_ShadowBufferWidth / (GLfloat)m_ShadowBufferHeight;
    for(int i = 0; i < m_Lights->getNumLights(); ++i)
    {
        m_LightView[i] = glm::lookAt(glm::make_vec3(glm::value_ptr(m_Lights->getLightPosition(i))), m_Camera->m_CameraFocus, glm::vec3(0.0f, 0.9f, 0.950f));
        m_LightProjection[i] = glm::perspective(45.0f, aspect, 0.1f, 1000.0f);

        m_UBufferLightMatrix->uploadData(glm::value_ptr(m_LightView[i]), 0, sizeof(glm::mat4));
        m_UBufferLightMatrix->uploadData(glm::value_ptr(m_LightProjection[i]), sizeof(glm::mat4), sizeof(glm::mat4));
        m_UBufferLightMatrix->uploadData(glm::value_ptr(m_Lights->getLightPosition(i)), 2 * sizeof(glm::mat4), sizeof(glm::vec3));

        m_DepthBufferRenders[i]->beginRender();


        glCall(glBindVertexArray(m_DSVAO));
        m_MeshObj->draw();
        glCall(glBindVertexArray(0));

        m_DepthBufferRenders[i]->endRender(defaultFBO);
    }


    m_DepthShader->release();
    glViewport(0, 0, scrWidth, scrHeight);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::initShadowMapRenderData(const glm::vec4& clearColor, bool bLinearDepthBuffer/* = false*/)
{
    for(int i = 0; i < m_Lights->getNumLights(); ++i)
    {
        m_DepthBufferRenders.push_back(new DepthBufferRender(m_ShadowBufferWidth, m_ShadowBufferHeight));
        m_DepthBufferRenders[i]->setDefaultClearColor(clearColor);
    }

    m_DepthShader = ShaderProgram::getSimpleDepthShader();

    m_DSAtrVPosition = m_DepthShader->getAtributeLocation("v_Position");
    m_DSUBModelMatrix = m_DepthShader->getUniformBlockIndex("ModelMatrix");
    m_DSUBLightMatrix = m_DepthShader->getUniformBlockIndex("LightMatrix");
    glCall(glGenVertexArrays(1, &m_DSVAO));
    glCall(glBindVertexArray(m_DSVAO));
    m_MeshObj->getVertexBuffer()->bind();
    glCall(glEnableVertexAttribArray(m_DSAtrVPosition));
    glCall(glVertexAttribPointer(m_DSAtrVPosition, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    if(m_MeshObj->hasIndexBuffer())
    {
        m_MeshObj->getIndexBuffer()->bind();
    }
    glCall(glBindVertexArray(0));


    m_UBufferLightMatrix = new OpenGLBuffer;
    m_UBufferLightMatrix->createBuffer(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::resizeShadowMap(int width, int height)
{
    m_ShadowBufferWidth  = width;
    m_ShadowBufferHeight = height;

    for(int i = 0; i < m_Lights->getNumLights(); ++i)
    {
        m_DepthBufferRenders[i]->resize(width, height);
    }
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::setupVAO()
{
    if(m_MeshObj->isEmpty())
        return;

    glCall(glBindVertexArray(m_VAO));
    m_MeshObj->getVertexBuffer()->bind();
    glCall(glEnableVertexAttribArray(m_AtrVPosition));
    glCall(glVertexAttribPointer(m_AtrVPosition, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));

    if(m_MeshObj->hasVertexNormal())
    {
        m_MeshObj->getNormalBuffer()->bind();
        glCall(glEnableVertexAttribArray(m_AtrVNormal));
        glCall(glVertexAttribPointer(m_AtrVNormal, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    }
    if(m_MeshObj->hasVertexTexCoord())
    {
        m_MeshObj->getTexCoordBuffer()->bind();
        glCall(glEnableVertexAttribArray(m_AtrVTexCoord));
        glCall(glVertexAttribPointer(m_AtrVTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    }
    //if(m_MeshObj->hasVertexColor())
    //{
    //    m_MeshObj->getVertexColorBuffer()->bind();
    //    glCall(glEnableVertexAttribArray(m_AtrVTexCoord));
    //    glCall(glVertexAttribPointer(m_AtrVTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    //}

    if(m_MeshObj->hasIndexBuffer())
    {
        m_MeshObj->getIndexBuffer()->bind();
    }
    glCall(glBindVertexArray(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLTexture* MeshRender::getShadowMap(int lightID /*= 0*/)
{
    assert(m_DepthBufferRenders.size() > 0);
    return m_DepthBufferRenders[lightID]->getDepthBuffer();
}

OpenGLTexture* MeshRender::getLinearShadowMap(int lightID /*= 0*/)
{
    return m_DepthBufferRenders[lightID]->getLinearDepthBuffer();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::initRenderData()
{
    assert(m_MeshObj != nullptr);

    m_Shader = ShaderProgram::getPhongShader();

    m_AtrVPosition = m_Shader->getAtributeLocation("v_Position");
    m_AtrVNormal   = m_Shader->getAtributeLocation("v_Normal");
    m_AtrVTexCoord = m_Shader->getAtributeLocation("v_TexCoord");

    m_UHasTexture = m_Shader->getUniformLocation("u_HasTexture");
    m_UTexSampler = m_Shader->getUniformLocation("u_TexSampler");
    //m_UShadowMap  = m_Shader->getUniformLocation("m_UShadowMap");

    m_UBModelMatrix = m_Shader->getUniformBlockIndex("ModelMatrix");
    m_UBCamData     = m_Shader->getUniformBlockIndex("CameraData");
    m_UBLight       = m_Shader->getUniformBlockIndex("Lights");
    m_UBMaterial    = m_Shader->getUniformBlockIndex("Material");

    ////////////////////////////////////////////////////////////////////////////////
    m_MeshObj->uploadDataToGPU();
    glCall(glGenVertexArrays(1, &m_VAO));
    setupVAO();

    ////////////////////////////////////////////////////////////////////////////////
    // uniform buffer
    if(m_UBufferCamData == nullptr)
    {
        m_UBufferCamData = new OpenGLBuffer;
        m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER, 5 * sizeof(glm::mat4) + sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    }

    m_UBufferModelMatrix = new OpenGLBuffer;
    m_UBufferModelMatrix->createBuffer(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    transform(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));

    ////////////////////////////////////////////////////////////////////////////////
    // material
    if(m_Material == nullptr)
    {
        m_Material = new Material;
        /*m_Material->setAmbientColor(glm::vec4(1.0));
        m_Material->setDiffuseColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
        m_Material->setSpecularColor(glm::vec4(1.0));
        m_Material->setShininess(250.0);*/
        m_Material->setMaterial(Material::MT_Emerald);
        m_Material->uploadDataToGPU();
    }

    ////////////////////////////////////////////////////////////////////////////////
    // default null texture
    m_Textures.push_back(nullptr);
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Plane render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PlaneRender::setAllowNonTextureRender(bool allowNonTex)
{
    m_AllowedNonTexRender = allowNonTex;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PlaneRender::scaleTexCoord(int scaleX, int scaleY)
{
    GridObject* gridObj = static_cast<GridObject*>(m_MeshObj);

    assert(gridObj != nullptr);
    gridObj->scaleTexCoord(scaleX, scaleY);
    gridObj->uploadDataToGPU();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PlaneRender::render()
{
    if(m_CurrentTexture != nullptr || (m_CurrentTexture == nullptr && m_AllowedNonTexRender))
        MeshRender::render();
}
