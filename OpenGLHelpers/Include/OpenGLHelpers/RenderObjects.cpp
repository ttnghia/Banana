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

#include <OpenGLHelpers/RenderObjects.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SkyBox render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

    foreach(QString texFolder, allTexFolders)
    {
        QString texFolderPath = textureTopFolder + "/" + texFolder;

        QString posXFilePath = texFolderPath + "/posx.jpg";
        QString ext = "jpg";

        if(!QFile::exists(posXFilePath))
        {
            ext = "png";
            posXFilePath = texFolderPath + "/posx.png";
            Q_ASSERT(QFile::exists(posXFilePath));
        }

        OpenGLTexture* skyboxTex = new OpenGLTexture(GL_TEXTURE_CUBE_MAP);

        for(GLuint i = 0; i < 6; ++i)
        {
            QString texFilePath = texFolderPath + texFaces[i] + ext;
            QImage texImg = QImage(texFilePath).convertToFormat(QImage::Format_RGBA8888);

            skyboxTex->uploadData(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                  GL_RGBA, texImg.width(), texImg.height(),
                                  GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());
        }

        skyboxTex->setBestParameters();
        skyboxTex->generateMipMap();
        addTexture(skyboxTex);
    }
}
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::addTexture(OpenGLTexture* texture)
{
    texture->setFilterMode(GL_TEXTURE_WRAP_S, GL_REPEAT);
    texture->setFilterMode(GL_TEXTURE_WRAP_T, GL_REPEAT);
    texture->setFilterMode(GL_TEXTURE_WRAP_R, GL_REPEAT);

    m_Textures.push_back(texture);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::setRenderTextureIndex(int texIndex)
{
    assert(static_cast<size_t>(texIndex) < m_Textures.size());

    m_CurrentTexture = m_Textures[texIndex];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t SkyBoxRender::getNumTextures()
{
    return m_Textures.size();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::render()
{
    assert(m_Camera != nullptr && m_UBufferCamData != nullptr);
    if(m_CurrentTexture == nullptr)
    {
        return;
    }

    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()),
                                 0, sizeof(glm::mat4));
    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()),
                                 sizeof(glm::mat4), sizeof(glm::mat4));
    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->m_CameraPosition),
                                 3 * sizeof(glm::mat4), sizeof(glm::vec3));

    m_Shader->bind();
    m_CurrentTexture->bind();


    m_UBufferModelMatrix->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBModelMatrix,
                               m_UBufferModelMatrix->getBindingPoint());

    m_UBufferCamData->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBCamData,
                               m_UBufferCamData->getBindingPoint());

    m_Shader->setUniformValue(m_UTexSampler, 0);


    glCall(glBindVertexArray(m_VAO));
    m_CubeObj->draw();
    m_CurrentTexture->release();
    m_Shader->release();
    glCall(glBindVertexArray(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
    m_CubeObj->bindAllBuffers();
    glCall(glVertexAttribPointer(m_AtrVPosition, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    glCall(glEnableVertexAttribArray(m_AtrVPosition));
    glCall(glBindVertexArray(0));

    ////////////////////////////////////////////////////////////////////////////////
    // uniform buffer
    m_UBufferModelMatrix = new OpenGLBuffer;
    m_UBufferModelMatrix->createBuffer(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4),
                                       nullptr, GL_STATIC_DRAW);

    glm::mat4 modelMatrix= glm::scale(glm::mat4(1.0f), glm::vec3(500.0f));
    m_UBufferModelMatrix->uploadData(glm::value_ptr(modelMatrix),
                                     0, sizeof(glm::mat4));

    if(m_UBufferCamData == nullptr)
    {
        m_UBufferCamData = new OpenGLBuffer;
        m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER,
                                       3 * sizeof(glm::mat4) + sizeof(glm::vec4),
                                       nullptr, GL_DYNAMIC_DRAW);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // default null texture
    m_Textures.push_back(nullptr);
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Light render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLightRender::setRenderSize(GLfloat renderSize)
{
    m_RenderSize = renderSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLightRender::render()
{
    assert(m_Camera != nullptr && m_UBufferCamData != nullptr);

    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()),
                                 0, sizeof(glm::mat4));
    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()),
                                 sizeof(glm::mat4), sizeof(glm::mat4));

    m_Shader->bind();

    m_UBufferCamData->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBCamData,
                               m_UBufferCamData->getBindingPoint());

    m_Light->bindUniformBuffer();
    m_Shader->bindUniformBlock(m_UBLight,
                               m_Light->getBufferBindingPoint());

    glCall(glBindVertexArray(m_VAO));
    glCall(glPointSize(m_RenderSize));
    glCall(glEnable(GL_VERTEX_PROGRAM_POINT_SIZE));
    glCall(glDrawArrays(GL_POINTS, 0, 1));
    glCall(glBindVertexArray(0));
    m_Shader->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
        "    mat4 shadowMatrix;\n"
        "    vec4 camPosition;\n"
        "};\n"
        "\n"
        "layout(std140) uniform PointLight\n"
        "{\n"
        "    vec4 ambient;\n"
        "    vec4 diffuse;\n"
        "    vec4 specular;\n"
        "    vec4 position;\n"
        "} light;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 worldCoord = vec4(vec3(light.position), 1.0);\n"
        "    gl_Position = projectionMatrix * viewMatrix * worldCoord;\n"
        "}";

    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "layout(std140) uniform PointLight\n"
        "{\n"
        "    vec4 ambient;\n"
        "    vec4 diffuse;\n"
        "    vec4 specular;\n"
        "    vec4 position;\n"
        "} light;\n"
        "\n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    outColor = light.diffuse;\n"
        "}\n";

    m_Shader = new ShaderProgram(std::string("LightRenderShader"));
    m_Shader->addVertexShaderFromSource(vertexShader);
    m_Shader->addFragmentShaderFromSource(fragmentShader);
    m_Shader->link();

    glCall(glGenVertexArrays(1, &m_VAO));

    m_UBCamData = m_Shader->getUniformBlockIndex("CameraData");
    m_UBLight = m_Shader->getUniformBlockIndex("PointLight");


    ////////////////////////////////////////////////////////////////////////////////
    // uniform buffer
    if(m_UBufferCamData == nullptr)
    {
        m_UBufferCamData = new OpenGLBuffer;
        m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER,
                                       3 * sizeof(glm::mat4) + sizeof(glm::vec4),
                                       nullptr, GL_DYNAMIC_DRAW);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Mesh render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

MeshObject * MeshRender::getMeshObj()
{
    return m_MeshObj;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Material * MeshRender::getMaterial()
{
    return m_Material;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLTexture * MeshRender::getCurrentTexture()
{
    return m_CurrentTexture;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t MeshRender::getNumTextures()
{
    return m_Textures.size();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::addTexture(OpenGLTexture * texture, GLenum texWrapMode /*= GL_REPEAT*/)
{
    texture->setFilterMode(GL_TEXTURE_WRAP_S, texWrapMode);
    texture->setFilterMode(GL_TEXTURE_WRAP_T, texWrapMode);
    texture->setFilterMode(GL_TEXTURE_WRAP_R, texWrapMode);

    m_Textures.push_back(texture);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::setRenderTextureIndex(int texIndex)
{
    assert(static_cast<size_t>(texIndex) < m_Textures.size());

    m_CurrentTexture = m_Textures[texIndex];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::transform(const glm::vec3 & translation, const glm::vec3 & scale)
{
    glm::mat4 modelMatrix = glm::scale(glm::translate(glm::mat4(1.0), translation), scale);
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

    m_UBufferModelMatrix->uploadData(glm::value_ptr(modelMatrix),
                                     0, sizeof(glm::mat4)); // model matrix
    m_UBufferModelMatrix->uploadData(glm::value_ptr(normalMatrix),
                                     sizeof(glm::mat4), sizeof(glm::mat4)); // normal matrix
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::render()
{
    assert(m_MeshObj != nullptr && m_Camera != nullptr && m_UBufferCamData != nullptr);

    if(m_SelfUpdateCamera)
    {
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()),
                                     0, sizeof(glm::mat4));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()),
                                     sizeof(glm::mat4), sizeof(glm::mat4));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->m_CameraPosition),
                                     3 * sizeof(glm::mat4), sizeof(glm::vec3));
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

    m_UBufferModelMatrix->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBModelMatrix,
                               m_UBufferModelMatrix->getBindingPoint());

    m_UBufferCamData->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBCamData,
                               m_UBufferCamData->getBindingPoint());

    m_Light->bindUniformBuffer();
    m_Shader->bindUniformBlock(m_UBLight,
                               m_Light->getBufferBindingPoint());

    m_Material->bindUniformBuffer();
    m_Shader->bindUniformBlock(m_UBMaterial,
                               m_Material->getBufferBindingPoint());

    glCall(glBindVertexArray(m_VAO));
    m_MeshObj->draw();

    if(m_CurrentTexture != nullptr)
    {
        m_CurrentTexture->release();
    }
    m_Shader->release();
    glCall(glBindVertexArray(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::initRenderData()
{
    assert(m_MeshObj != nullptr);

    m_Shader = ShaderProgram::getPhongShader();

    m_AtrVPosition = m_Shader->getAtributeLocation("v_Position");
    m_AtrVNormal   = m_Shader->getAtributeLocation("v_Normal");
    m_AtrVTexCoord = m_Shader->getAtributeLocation("v_TexCoord");

    m_UHasTexture = m_Shader->getUniformLocation("hasTexture");
    m_UTexSampler = m_Shader->getUniformLocation("u_TexSampler");

    m_UBModelMatrix = m_Shader->getUniformBlockIndex("ModelMatrix");
    m_UBCamData     = m_Shader->getUniformBlockIndex("CameraData");
    m_UBLight       = m_Shader->getUniformBlockIndex("PointLight");
    m_UBMaterial    = m_Shader->getUniformBlockIndex("Material");

    ////////////////////////////////////////////////////////////////////////////////
    m_MeshObj->uploadDataToGPU();

    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glBindVertexArray(m_VAO));
    m_MeshObj->bindAllBuffers();
    glCall(glEnableVertexAttribArray(m_AtrVPosition));
    glCall(glVertexAttribPointer(m_AtrVPosition, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));

    if(m_MeshObj->hasVertexNormal())
    {
        glCall(glEnableVertexAttribArray(m_AtrVNormal));
        glCall(glVertexAttribPointer(m_AtrVNormal, 3, GL_FLOAT, GL_FALSE, 0,
            (GLvoid*)(m_MeshObj->getVNormalOffset())));
    }
    if(m_MeshObj->hasVertexTexCoord())
    {
        glCall(glEnableVertexAttribArray(m_AtrVTexCoord));
        glCall(glVertexAttribPointer(m_AtrVTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
            (GLvoid*)(m_MeshObj->getVTexCoordOffset())));
    }

    glCall(glBindVertexArray(0));

    ////////////////////////////////////////////////////////////////////////////////
    // uniform buffer
    if(m_UBufferCamData == nullptr)
    {
        m_UBufferCamData = new OpenGLBuffer;
        m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER,
                                       3 * sizeof(glm::mat4) + sizeof(glm::vec4),
                                       nullptr, GL_DYNAMIC_DRAW);
    }

    m_UBufferModelMatrix = new OpenGLBuffer;
    m_UBufferModelMatrix->createBuffer(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr,
                                       GL_STATIC_DRAW);
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


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Plane render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PlaneRender::setAllowNonTextureRender(bool allowNonTex)
{
    m_AllowedNonTexRender = allowNonTex;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PlaneRender::clearTextures()
{
    assert(m_MeshRender != nullptr);
    m_MeshRender->clearTextures();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PlaneRender::addTexture(OpenGLTexture* texture, GLenum texWrapMode /*= GL_REPEAT*/)
{
    assert(m_MeshRender != nullptr);
    m_MeshRender->addTexture(texture, texWrapMode);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PlaneRender::setRenderTextureIndex(int texIndex)
{
    assert(m_MeshRender != nullptr);
    m_MeshRender->setRenderTextureIndex(texIndex);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t PlaneRender::getNumTextures()
{
    assert(m_MeshRender != nullptr);
    return m_MeshRender->getNumTextures();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PlaneRender::transform(const glm::vec3& translation, const glm::vec3& scale)
{
    assert(m_MeshRender != nullptr);
    m_MeshRender->transform(translation, scale);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PlaneRender::scaleTexCoord(int scaleX, int scaleY)
{
    assert(m_MeshRender != nullptr);
    GridObject* gridObj = static_cast<GridObject*>(m_MeshRender->getMeshObj());

    assert(gridObj != nullptr);
    gridObj->scaleTexCoord(scaleX, scaleY);
    gridObj->uploadDataToGPU();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PlaneRender::render()
{
    assert(m_MeshRender != nullptr);

    if(m_MeshRender->getCurrentTexture() != nullptr ||
        (m_MeshRender->getCurrentTexture() == nullptr && m_AllowedNonTexRender))
        m_MeshRender->render();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PlaneRender::initRenderData()
{
    assert(m_MeshRender != nullptr);
    // nothing to do
}



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WireFrameBoxRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WireFrameBoxRender::transform(const glm::vec3 & translation, const glm::vec3 & scale)
{
    glm::mat4 modelMatrix = glm::scale(glm::translate(glm::mat4(1.0), translation), scale);
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

    m_UBufferModelMatrix->uploadData(glm::value_ptr(modelMatrix),
                                     0, sizeof(glm::mat4)); // model matrix
    m_UBufferModelMatrix->uploadData(glm::value_ptr(normalMatrix),
                                     sizeof(glm::mat4), sizeof(glm::mat4)); // normal matrix}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WireFrameBoxRender::render()
{
    m_Shader->bind();

    m_UBufferModelMatrix->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBModelMatrix, m_UBufferModelMatrix->getBindingPoint());

    if(m_SelfUpdateCamera)
    {
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()),
                                     0, sizeof(glm::mat4));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()),
                                     sizeof(glm::mat4), sizeof(glm::mat4));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->m_CameraPosition),
                                     3 * sizeof(glm::mat4), sizeof(glm::vec3));
    }
    m_UBufferCamData->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBCamData, m_UBufferCamData->getBindingPoint());

    m_Shader->setUniformValue(m_UColor, m_BoxColor);

    glCall(glBindVertexArray(m_VAO));
    m_WireFrameBoxObj->draw();
    m_Shader->release();
    glCall(glBindVertexArray(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
    m_WireFrameBoxObj->bindAllBuffers();
    glCall(glVertexAttribPointer(m_AtrVPosition, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    glCall(glEnableVertexAttribArray(m_AtrVPosition));
    glCall(glBindVertexArray(0));

    ////////////////////////////////////////////////////////////////////////////////
    // uniform buffer
    if(m_UBufferCamData == nullptr)
    {
        m_UBufferCamData = new OpenGLBuffer;
        m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER,
                                       3 * sizeof(glm::mat4) + sizeof(glm::vec4),
                                       nullptr, GL_DYNAMIC_DRAW);
    }

    m_UBufferModelMatrix = new OpenGLBuffer;
    m_UBufferModelMatrix->createBuffer(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr,
                                       GL_STATIC_DRAW);
    transform(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
}
