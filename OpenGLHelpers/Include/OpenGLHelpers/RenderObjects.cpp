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

#include <OpenGLHelpers/RenderObjects.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SkyBox render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __Banana_Qt__
#include <QDir>

SkyBoxRender::SkyBoxRender(Camera* camera, QString texureTopFolder) :
    m_Camera(camera)
{
    initRenderData();
    loadTextures(texureTopFolder);
}

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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::initRenderData()
{
    m_Shader = ShaderProgram::getSkyBoxShader();

    m_AtrVPosition = m_Shader->getAtributeLocation("v_position");
    m_UTexSampler = m_Shader->getUniformLocation("texSampler");
    m_UCamPosition = m_Shader->getUniformLocation("camPosition");
    m_UMatrices = m_Shader->getUniformBlockIndex("Matrices");

    ////////////////////////////////////////////////////////////////////////////////
    // cube object
    m_CubeObj = new CubeObject;
    m_CubeObj->uploadDataToGPU();

    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glBindVertexArray(m_VAO));
    m_CubeObj->bindAllBuffers();
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    glCall(glEnableVertexAttribArray(0));
    glCall(glBindVertexArray(0));

    ////////////////////////////////////////////////////////////////////////////////
    // uniform buffer
    m_UBufferMatrices = new OpenGLBuffer;
    m_UBufferMatrices->createBuffer(GL_UNIFORM_BUFFER, 5 * sizeof(glm::mat4),
                                    nullptr, GL_STATIC_DRAW);
    glm::mat4 modelMatrix= glm::scale(glm::mat4(1.0f), glm::vec3(500.0f));
    m_UBufferMatrices->uploadData(glm::value_ptr(modelMatrix),
                                  0, sizeof(glm::mat4));

    ////////////////////////////////////////////////////////////////////////////////
    // texture
    m_SkyBoxTextures.push_back(nullptr);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::clearTextures()
{
    for(OpenGLTexture* tex : m_SkyBoxTextures)
    {
        delete tex;
    }

    m_SkyBoxTextures.resize(0);
    m_CurrentSkyBoxTex = nullptr;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::addTexture(OpenGLTexture* texture)
{
    texture->setFilterMode(GL_TEXTURE_WRAP_S, GL_REPEAT);
    texture->setFilterMode(GL_TEXTURE_WRAP_T, GL_REPEAT);
    texture->setFilterMode(GL_TEXTURE_WRAP_R, GL_REPEAT);

    m_SkyBoxTextures.push_back(texture);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::setRenderTextureIndex(int texIndex)
{
    assert(static_cast<size_t>(texIndex) < m_SkyBoxTextures.size());

    m_CurrentSkyBoxTex = m_SkyBoxTextures[texIndex];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t SkyBoxRender::getNumTextures()
{
    return m_SkyBoxTextures.size();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::render()
{
    assert(m_Camera != nullptr && m_UBufferMatrices != nullptr);
    if(m_CurrentSkyBoxTex == nullptr)
    {
        return;
    }

    m_UBufferMatrices->uploadData(glm::value_ptr(m_Camera->getViewMatrix()),
                                  2 * sizeof(glm::mat4), sizeof(glm::mat4));
    m_UBufferMatrices->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()),
                                  3 * sizeof(glm::mat4), sizeof(glm::mat4));

    m_Shader->bind();
    m_CurrentSkyBoxTex->bind();

    m_UBufferMatrices->bindBufferBase();
    m_Shader->bindUniformBlock(m_UMatrices,
                               m_UBufferMatrices->getBindingPoint());

    m_Shader->setUniformValue(m_UCamPosition, m_Camera->m_CameraPosition);
    m_Shader->setUniformValue(m_UTexSampler, 0);


    glCall(glBindVertexArray(m_VAO));
    m_CubeObj->draw();
    m_CurrentSkyBoxTex->release();
    m_Shader->release();
    glCall(glBindVertexArray(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Floor render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __Banana_Qt__
FloorRender::FloorRender(Camera* camera, Light* light, QString texureFolder) :
    m_Camera(camera), m_Light(light)
{
    initRenderData();
    loadTextures(texureFolder);
}

void FloorRender::loadTextures(QString textureFolder)
{
    clearTextures();

    QDir dataDir(textureFolder);
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    QStringList allTexFiles = dataDir.entryList();

    foreach(QString texFile, allTexFiles)
    {
        QString texFilePath = textureFolder + "/" + texFile;
        OpenGLTexture* floorTex = new OpenGLTexture(GL_TEXTURE_2D);
        QImage texImg = QImage(texFilePath).convertToFormat(QImage::Format_RGBA8888);

        floorTex->uploadData(GL_TEXTURE_2D,
                             GL_RGBA, texImg.width(), texImg.height(),
                             GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());

        floorTex->setBestParameters();
        floorTex->generateMipMap();
        addTexture(floorTex);
    }
}
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FloorRender::clearTextures()
{
    for(OpenGLTexture* tex : m_FloorTextures)
    {
        delete tex;
    }

    m_FloorTextures.resize(0);
    m_CurrentFloorTex = nullptr;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FloorRender::addTexture(OpenGLTexture* texture)
{
    texture->setFilterMode(GL_TEXTURE_WRAP_S, GL_REPEAT);
    texture->setFilterMode(GL_TEXTURE_WRAP_T, GL_REPEAT);
    texture->setFilterMode(GL_TEXTURE_WRAP_R, GL_REPEAT);

    m_FloorTextures.push_back(texture);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FloorRender::setRenderTextureIndex(int texIndex)
{
    assert(static_cast<size_t>(texIndex) < m_FloorTextures.size());

    m_CurrentFloorTex = m_FloorTextures[texIndex];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t FloorRender::getNumTextures()
{
    return m_FloorTextures.size();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FloorRender::transform(const glm::vec3& translation, const glm::vec3& scale)
{
    m_ModelMatrix = glm::scale(glm::translate(glm::mat4(1.0), translation), scale);
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(glm::mat3(m_ModelMatrix)));

    m_UBufferMatrices->uploadData(glm::value_ptr(m_ModelMatrix),
                                  0, sizeof(glm::mat4)); // model matrix
    m_UBufferMatrices->uploadData(glm::value_ptr(normalMatrix),
                                  sizeof(glm::mat4), sizeof(glm::mat4)); // normal matrix
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FloorRender::scaleTexCoord(int scaleX, int scaleY)
{
    m_GridObj->scaleTexCoord(scaleX, scaleY);
    m_GridObj->uploadDataToGPU();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FloorRender::render()
{
    assert(m_Camera != nullptr && m_UBufferMatrices != nullptr);
    if(m_CurrentFloorTex == nullptr)
    {
        return;
    }

    m_UBufferMatrices->uploadData(glm::value_ptr(m_Camera->getViewMatrix()),
                                  2 * sizeof(glm::mat4), sizeof(glm::mat4));
    m_UBufferMatrices->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()),
                                  3 * sizeof(glm::mat4), sizeof(glm::mat4));

    m_Shader->bind();
    m_CurrentFloorTex->bind();

    m_UBufferMatrices->bindBufferBase();
    m_Shader->bindUniformBlock(m_UMatrices,
                               m_UBufferMatrices->getBindingPoint());

    m_Light->bindUniformBuffer();
    m_Shader->bindUniformBlock(m_ULight,
                               m_Light->getBufferBindingPoint());

    m_Material->bindUniformBuffer();
    m_Shader->bindUniformBlock(m_UMaterial,
                               m_Material->getBufferBindingPoint());

    m_Shader->setUniformValue(m_UCamPosition, m_Camera->m_CameraPosition);
    m_Shader->setUniformValue(m_UHasTexture, 1);
    m_Shader->setUniformValue(m_UTexSampler, 0);


    glCall(glBindVertexArray(m_VAO));
    m_GridObj->draw();
    m_CurrentFloorTex->release();
    m_Shader->release();
    glCall(glBindVertexArray(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FloorRender::initRenderData()
{
    m_Shader = ShaderProgram::getPhongShader();

    m_AtrVPosition = m_Shader->getAtributeLocation("v_position");
    m_AtrVNormal = m_Shader->getAtributeLocation("v_normal");
    m_AtrVTexCoord = m_Shader->getAtributeLocation("v_texcoord");

    m_UHasTexture = m_Shader->getUniformLocation("hasTexture");
    m_UTexSampler = m_Shader->getUniformLocation("texSampler");
    m_UCamPosition = m_Shader->getUniformLocation("camPosition");
    m_UMatrices = m_Shader->getUniformBlockIndex("Matrices");
    m_ULight = m_Shader->getUniformBlockIndex("PointLight");
    m_UMaterial = m_Shader->getUniformBlockIndex("Material");

    ////////////////////////////////////////////////////////////////////////////////
    // cube object
    m_GridObj = new GridObject(1, 1);
    m_GridObj->uploadDataToGPU();

    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glBindVertexArray(m_VAO));
    m_GridObj->bindAllBuffers();
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    glCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
        (GLvoid*)(m_GridObj->getVNormalOffset())));
    glCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0,
        (GLvoid*)(m_GridObj->getVTexCoordOffset())));
    glCall(glEnableVertexAttribArray(0));
    glCall(glEnableVertexAttribArray(1));
    glCall(glEnableVertexAttribArray(2));
    glCall(glBindVertexArray(0));

    ////////////////////////////////////////////////////////////////////////////////
    // uniform buffer
    m_UBufferMatrices = new OpenGLBuffer;
    m_UBufferMatrices->createBuffer(GL_UNIFORM_BUFFER, 5 * sizeof(glm::mat4),
                                    nullptr, GL_STATIC_DRAW);
    transform(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
    scaleTexCoord(10, 10);

    ////////////////////////////////////////////////////////////////////////////////
    // texture
    m_FloorTextures.push_back(nullptr);

    ////////////////////////////////////////////////////////////////////////////////
    // material
    m_Material = new Material;
    m_Material->setAmbientColor(glm::vec4(1.0));
    m_Material->setDiffuseColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
    m_Material->setSpecularColor(glm::vec4(1.0));
    m_Material->setShininess(250.0);
    m_Material->uploadBuffer();
}

