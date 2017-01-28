//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <QtAppHelpers/OpenGLWidgetTestRender.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLWidgetTestRender::OpenGLWidgetTestRender(QWidget *parent)
    : OpenGLWidget(parent),
    m_RenderType(RenderType::NotSetup),
    m_UniformBuffer(nullptr),
    m_VertexBuffer(nullptr),
    m_IndexBuffer(nullptr),
    m_Texture(nullptr),
    m_Shader(nullptr),
    m_MeshObj(nullptr),
    m_MeshLoader(nullptr)
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLWidgetTestRender::~OpenGLWidgetTestRender()
{
    delete m_UniformBuffer;
    delete m_VertexBuffer;
    delete m_IndexBuffer;
    delete m_Texture;
    delete m_Shader;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderTriangle()
{
    assert(isValid());

    ////////////////////////////////////////////////////////////////////////////////
    // setup triangle
    GLfloat triangle[] ={
        /* position */ 0.0,  0.8, 0.0, /* color */ 1.0, 1.0, 0.0,
        /* position */ -0.8, -0.8, 0.0, /* color */ 0.0, 0.0, 1.0,
        /* position */ 0.8, -0.8, 0.0, /* color */ 1.0, 0.0, 0.0
    };

    m_VertexBuffer = new OpenGLBuffer;
    m_VertexBuffer->createBuffer(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);


    // shaders and VAO
    m_Shader = ShaderProgram::getSimpleVertexColorShader();


    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glBindVertexArray(m_VAO));
    m_VertexBuffer->bind();
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0));
    glCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
        (GLvoid*)(3 * sizeof(GLfloat))));
    glCall(glEnableVertexAttribArray(0));
    glCall(glEnableVertexAttribArray(1));
    glCall(glBindVertexArray(0));

    ////////////////////////////////////////////////////////////////////////////////
    m_RenderType = RenderType::Triangle;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderTexture(QString texFile)
{
    assert(isValid());

    ////////////////////////////////////////////////////////////////////////////////
    // setup square and texture
    GLfloat triangle[] ={
        /* position */ 0.0,  0.8, 0.0, /* texcoord */ 0.5, 1.0,
        /* position */ -0.8, -0.8, 0.0, /* texcoord */ 0.0, 0.0,
        /* position */ 0.8, -0.8, 0.0, /* texcoord */ 1.0, 0.0
    };

    m_VertexBuffer = new OpenGLBuffer;
    m_VertexBuffer->createBuffer(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);


    // shaders and VAO
    m_Shader = ShaderProgram::getSimpleTextureShader();


    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glBindVertexArray(m_VAO));
    m_VertexBuffer->bind();
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0));
    glCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
        (GLvoid*)(3 * sizeof(GLfloat))));
    glCall(glEnableVertexAttribArray(0));
    glCall(glEnableVertexAttribArray(1));
    glCall(glBindVertexArray(0));


    m_Texture = new OpenGLTexture(GL_TEXTURE_2D);
    QImage texImg = QImage(texFile).convertToFormat(QImage::Format_RGBA8888);
    m_Texture->uploadData(GL_TEXTURE_2D,
                          GL_RGBA, texImg.width(), texImg.height(),
                          GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());

    m_Texture->setBestParameters();
    m_Texture->generateMipMap();

    ////////////////////////////////////////////////////////////////////////////////
    m_RenderType = RenderType::Texture;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderSkybox(QString texFolder)
{
    ////////////////////////////////////////////////////////////////////////////////
    // textures
    QStringList texFaces =
    {
        "/posx.", "/negx.",
        "/posy.", "/negy.",
        "/posz.", "/negz."
    };

    QString posXFilePath = texFolder + "/posx.jpg";
    QString ext = "jpg";

    if(!QFile::exists(posXFilePath))
    {
        ext = "png";
        posXFilePath = texFolder + "/posx.png";
        Q_ASSERT(QFile::exists(posXFilePath));
    }

    m_Texture = new OpenGLTexture(GL_TEXTURE_CUBE_MAP);

    for(GLuint i = 0; i < 6; ++i)
    {
        QString texFilePath = texFolder + texFaces[i] + ext;
        QImage texImg = QImage(texFilePath).convertToFormat(QImage::Format_RGBA8888);

        m_Texture->uploadData(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                              GL_RGBA, texImg.width(), texImg.height(),
                              GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());
    }

    m_Texture->setBestParameters();
    m_Texture->generateMipMap();

    ////////////////////////////////////////////////////////////////////////////////
    // render data
    m_SkyBoxRender = new SkyBoxRender(m_Camera);
    m_SkyBoxRender->addTexture(m_Texture);
    m_SkyBoxRender->setRenderTextureIndex(1);

    ////////////////////////////////////////////////////////////////////////////////
    m_RenderType = RenderType::SkyBox;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderFloor(QString texFile)
{
    ////////////////////////////////////////////////////////////////////////////////
   // textures
    m_Texture = new OpenGLTexture(GL_TEXTURE_2D);

    QImage texImg = QImage(texFile).convertToFormat(QImage::Format_RGBA8888);

    m_Texture->uploadData(GL_TEXTURE_2D,
                          GL_RGBA, texImg.width(), texImg.height(),
                          GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());

    m_Texture->setBestParameters();
    m_Texture->generateMipMap();


    ////////////////////////////////////////////////////////////////////////////////
    // camera
    m_Camera->setDefaultCamera(glm::vec3(2, 1, 0), glm::vec3(0, 0, 0),
                               glm::vec3(0, 1, 0));

    ////////////////////////////////////////////////////////////////////////////////
    // light
    m_Light = new PointLight;
    m_Light->setLightPosition(glm::vec4(0, 20, 0, 1.0));
    m_Light->uploadBuffer();

    ////////////////////////////////////////////////////////////////////////////////
    // render data
    m_FloorRender = new FloorRender(m_Camera, m_Light);
    m_FloorRender->addTexture(m_Texture);
    m_FloorRender->setRenderTextureIndex(1);


    ////////////////////////////////////////////////////////////////////////////////
    m_RenderType = RenderType::Floor;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderMesh(QString meshFile)
{
    assert(isValid());

    ////////////////////////////////////////////////////////////////////////////////
    m_MeshLoader = new MeshLoader(meshFile.toStdString());
    m_MeshObj = new MeshObject;
    m_MeshObj->setVertices(m_MeshLoader->getVertices());
    m_MeshObj->setVertexNormal(m_MeshLoader->getVertexNormal());
    m_MeshObj->uploadDataToGPU();

    // shaders and VAO
    m_Shader = ShaderProgram::getPhongShader();

    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glBindVertexArray(m_VAO));
    m_MeshObj->bindAllBuffers();
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    glCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
        (GLvoid*)(m_MeshObj->getVNormalOffset())));
    glCall(glEnableVertexAttribArray(0));
    glCall(glEnableVertexAttribArray(1));
    glCall(glBindVertexArray(0));

    ////////////////////////////////////////////////////////////////////////////////
    m_Light = new PointLight;
    m_Light->setLightPosition(glm::vec4(1, 10000, 1, 1.0));
    m_Light->uploadBuffer();

    m_Material = new Material;
    m_Material->setMaterial(Material::MT_Emerald);
    m_Material->uploadBuffer();

    m_UniformBuffer = new OpenGLBuffer;
    m_UniformBuffer->createBuffer(GL_UNIFORM_BUFFER, 4 * sizeof(glm::mat4), nullptr,
                                  GL_DYNAMIC_DRAW);
    glm::mat4 modelMatrix= glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));
    glm::mat4 normalMatrix= glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    m_UniformBuffer->uploadData(glm::value_ptr(modelMatrix),
                                0, sizeof(glm::mat4)); // model matrix
    m_UniformBuffer->uploadData(glm::value_ptr(normalMatrix),
                                sizeof(glm::mat4), sizeof(glm::mat4)); // normal matrix

    ////////////////////////////////////////////////////////////////////////////////
    m_RenderType = RenderType::TriMesh;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initializeGL()
{
    OpenGLWidget::initializeGL();

    //initTestRenderTriangle();
    //initTestRenderTexture(
    //    QString("D:/Programming/QtApps/RealTimeFluidRendering/Textures/Floor/blue_marble.png"));
    initTestRenderFloor(
        QString("D:/Programming/QtApps/RealTimeFluidRendering/Textures/Floor/blue_marble.png"));
    /*initTestRenderSkybox(
        QString("D:/Programming/QtApps/RealTimeFluidRendering/Textures/Sky/sky1"));*/
        /*initTestRenderMesh(
            QString("D:/GoogleDrive/DigitalAssets/Models/Animal/Bear 1/model_mesh.obj"));*/
            /*initTestRenderMesh(
                QString("D:/GoogleDrive/DigitalAssets/Models/Car/Volkswagen Touareg 2/model/Touareg.obj"));*/
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::resizeGL(int w, int h)
{
    OpenGLWidget::resizeGL(w, h);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::paintGL()
{
    startFrameTimer();
    OpenGLWidget::paintGL();

    ////////////////////////////////////////////////////////////////////////////////
    switch(m_RenderType)
    {
        case RenderType::Triangle:
            renderTriangle();
            break;

        case RenderType::Texture:
            renderTexture();
            break;

        case RenderType::SkyBox:
            renderSkyBox();
            break;

        case RenderType::Floor:
            renderFloor();
            break;

        case RenderType::TriMesh:
            renderMesh();
            break;

        default:
            ;
    }
    ////////////////////////////////////////////////////////////////////////////////

    endFrameTimer();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderTriangle()
{
    m_Shader->bind();
    glCall(glBindVertexArray(m_VAO));
    glCall(glDrawArrays(GL_TRIANGLES, 0, 3));
    m_Shader->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderTexture()
{
    assert(m_Texture != nullptr);

    m_Shader->bind();
    m_Texture->bind();

    m_Shader->setUniformValue<GLint>("texSampler", 0);
    glCall(glBindVertexArray(m_VAO));
    glCall(glDrawArrays(GL_TRIANGLES, 0, 3));
    m_Texture->release();
    m_Shader->release();

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderSkyBox()
{
    assert(m_Texture != nullptr);

    m_SkyBoxRender->render();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderFloor()
{
    assert(m_Texture != nullptr);

    m_FloorRender->render();

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderMesh()
{
    assert(m_MeshObj != nullptr);

    static std::vector<Material::MaterialData> materials =
        Material::getBuildInMaterials();
    static int count = 0;
    static int currentMaterialIndex = 0;

    ++count;
    if(count > 300)
    {
        count = 0;

        m_Material->setMaterial(materials[currentMaterialIndex]);
        m_Material->uploadBuffer();
        currentMaterialIndex = (currentMaterialIndex + 1) % materials.size();
        qDebug() << "Material: " << QString::fromStdString(m_Material->getName());
    }

    m_Shader->bind();

    m_UniformBuffer->uploadData(glm::value_ptr(m_Camera->getViewMatrix()),
                                2 * sizeof(glm::mat4), sizeof(glm::mat4));
    m_UniformBuffer->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()),
                                3 * sizeof(glm::mat4), sizeof(glm::mat4));
    m_UniformBuffer->bindBufferBase();
    glCall(glUniformBlockBinding(m_Shader->programID,
           m_Shader->getUniformBlockIndex("Matrices"), m_UniformBuffer->getBindingPoint()));

    m_Light->bindUniformBuffer();
    m_Shader->bindUniformBlock(m_Shader->getUniformBlockIndex("PointLight"),
                               m_Light->getBufferBindingPoint());

    m_Material->bindUniformBuffer();
    m_Shader->bindUniformBlock(m_Shader->getUniformBlockIndex("Material"),
                               m_Material->getBufferBindingPoint());

    m_Shader->setUniformValue<glm::vec3>("camPosition", m_Camera->m_CameraPosition);
    m_Shader->setUniformValue<GLint>("hasTexture", 0);

    glCall(glBindVertexArray(m_VAO));
    m_MeshObj->draw();
    m_Shader->release();

}