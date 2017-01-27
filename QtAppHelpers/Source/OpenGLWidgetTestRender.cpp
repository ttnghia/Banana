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
    m_Shader(nullptr)
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
void OpenGLWidgetTestRender::setRenderTriangle()
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


    const GLchar* vertexShader =
        "#version 410 core\n"
        "layout(location = 0) in vec3 v_position;\n"
        "layout(location = 1) in vec3 v_color;\n"
        "out vec3 f_color;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(v_position, 1.0);\n"
        "    f_color = v_color;\n"
        "}";

    const GLchar* fragmentShader =
        "#version 410 core\n"
        "in vec3 f_color;\n"
        "out vec4 outColor;\n"
        "void main()\n"
        "{ outColor = vec4(f_color, 1.0f); }\n";

    m_Shader = new ShaderProgram;
    m_Shader->addVertexShader(vertexShader);
    m_Shader->addFragmentShader(fragmentShader);
    m_Shader->link();

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
void OpenGLWidgetTestRender::setRenderTexture(QString texFile)
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


    const GLchar* vertexShader =
        "#version 410 core\n"
        "layout(location = 0) in vec3 v_position;\n"
        "layout(location = 1) in vec2 v_texcoord;\n"
        "out vec2 f_texcoord;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(v_position, 1.0);\n"
        "    f_texcoord = v_texcoord;\n"
        "}";

    const GLchar* fragmentShader =
        "#version 410 core\n"
        "uniform sampler2D texSampler;\n"
        "in vec2 f_texcoord;\n"
        "out vec4 outColor;\n"
        "void main()\n"
        "{\n"
        "    outColor = texture(texSampler, f_texcoord);\n"
        "}\n";

    m_Shader = new ShaderProgram;
    m_Shader->addVertexShader(vertexShader);
    m_Shader->addFragmentShader(fragmentShader);
    m_Shader->link();

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
void OpenGLWidgetTestRender::initializeGL()
{
    OpenGLWidget::initializeGL();

    setRenderTriangle();
    //setRenderTexture(QString("D:/Programming/QtApps/RealTimeFluidRendering/Textures/Floor/blue_marble.png"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::resizeGL(int w, int h)
{
    OpenGLWidget::resizeGL(w, h);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidgetTestRender::paintGL()
{
    OpenGLWidget::paintGL();
    m_FPSCounter.countFrame();
    startFrameTimer();

    ////////////////////////////////////////////////////////////////////////////////
    if(m_RenderType == RenderType::Triangle)
        renderTriangle();
    else if(m_RenderType == RenderType::Texture)
        renderTexture();
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
