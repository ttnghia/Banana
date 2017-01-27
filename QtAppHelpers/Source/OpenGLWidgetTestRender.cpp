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
    // cube object
    m_CubeObj = new CubeObject;
    m_CubeObj->uploadDataToGPU();

    ////////////////////////////////////////////////////////////////////////////////
    // shaders and VAO
    m_Shader = ShaderProgram::getSkyBoxShader();

    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glBindVertexArray(m_VAO));
    m_CubeObj->bindAllBuffers();
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    glCall(glEnableVertexAttribArray(0));
    glCall(glBindVertexArray(0));

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
    m_RenderType = RenderType::SkyBox;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::initTestRenderMesh(QString meshFile)
{
    assert(isValid());

    ////////////////////////////////////////////////////////////////////////////////
    m_MeshLoader = new MeshLoader(meshFile.toStdString());
    m_MeshObj = new MeshObject;
    m_MeshObj->setVertices(m_MeshLoader->get_vertices());
    m_MeshObj->uploadDataToGPU();

    // shaders and VAO
    m_Shader = ShaderProgram::getSimpleUniformColorShader();

    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glBindVertexArray(m_VAO));
    m_MeshObj->bindAllBuffers();
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    glCall(glEnableVertexAttribArray(0));
    glCall(glBindVertexArray(0));

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
    //initTestRenderSkybox(
    //    QString("D:/Programming/QtApps/RealTimeFluidRendering/Textures/Sky/sky1"));
    initTestRenderMesh(
        QString("D:/GoogleDrive/DigitalAssets/Models/Animal/Bear 1/model_mesh.obj"));
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
    m_FPSCounter.countFrame();

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

    m_Shader->bind();
    m_Texture->bind();

    m_Shader->setUniformValue<GLint>("texSampler", 0);
    m_Shader->setUniformValue<glm::vec3>("camPosition", m_Camera.m_CameraPosition);
    m_Shader->setUniformValue<glm::mat4>("viewMatrix", m_Camera.getViewMatrix());
    m_Shader->setUniformValue<glm::mat4>("projectionMatrix", m_Camera.getProjectionMatrix());

    glCall(glBindVertexArray(m_VAO));
    //m_CubeObj->setCullFaceMode(GL_NONE);
    m_CubeObj->draw();
    m_Texture->release();
    m_Shader->release();

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::renderMesh()
{
    assert(m_MeshObj != nullptr);

    m_Shader->bind();

    m_Shader->setUniformValue<glm::vec3>("objColor", glm::vec3(0.5, 0.1, 0.9));

    //m_Shader->setUniformValue<glm::vec3>("camPosition", m_Camera.m_CameraPosition);
    //m_Shader->setUniformValue<glm::mat4>("viewMatrix", m_Camera.getViewMatrix());
    //m_Shader->setUniformValue<glm::mat4>("projectionMatrix", m_Camera.getProjectionMatrix());

    glCall(glBindVertexArray(m_VAO));
    m_MeshObj->draw();
    m_Shader->release();

}