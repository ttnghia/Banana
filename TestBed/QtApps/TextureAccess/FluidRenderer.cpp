//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//
// Created on: 2/20/2015
//     Author: Nghia Truong
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include "FluidRenderer.h"

#include <cassert>

FluidRenderer::FluidRenderer(QWidget* parent) :
    QOpenGLWidget(parent),
    iboScreenQuad(QOpenGLBuffer::IndexBuffer),
    method(0)
{
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(0);


    numIterations = 500;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FluidRenderer::dump()
{
//    VSProfileLib::CollectQueryResults();
//    printf("%s\n", VSProfileLib::DumpLevels().c_str());
//    fflush(stdout);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FluidRenderer::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_D)
    {
        dump();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QSize FluidRenderer::sizeHint() const
{
    return QSize(1200, 1000);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QSize FluidRenderer::minimumSizeHint() const
{
    return QSize(50, 50);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int FluidRenderer::getMethod()
{
    return method;
}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FluidRenderer::setMethod(int _method)
{
    method = _method;

//    if(method == 0)
//    {
//        qDebug() << "Method: old";
//    }
//    else
//    {
//        qDebug() << "Method: new";
//    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FluidRenderer::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable (GL_DEPTH_TEST);
    glClearColor(0.9f, 0.9f, 0.9f, 1.0);
    checkOpenGLVersion();

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FluidRenderer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FluidRenderer::paintGL()
{
    static bool sceneInit = false;

    if(!sceneInit)
    {
        initScene();
        sceneInit = true;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glFinish();
    FPSTimer.tick();

    if(method == 0)
    {
        OldMethod();
    }
    else
    {
        NewMethod();
    }


    ////////////////////////////////////////////////////////////////////////////////
    glFinish();
    FPSTimer.tock();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FluidRenderer::checkOpenGLVersion()
{
    QString verStr = QString((const char*)glGetString(GL_VERSION));
    int major = verStr.left(verStr.indexOf(".")).toInt();
    int minor = verStr.mid(verStr.indexOf(".") + 1, 1).toInt();

    if(!(major >= 4 && minor >= 1))
    {
        QMessageBox::critical(this, "Error",
                              QString("Your OpenGL version is %1.%2, which does not satisfy this program requirement (OpenGL >= 4.1)")
                              .arg(major).arg(minor));
        close();
    }

//    qDebug() << major << minor;
//    qDebug() << verStr;
    //    TRUE_OR_DIE(major >= 4 && minor >= 1, "OpenGL version must >= 4.1");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FluidRenderer::initScene()
{
    initOldMethodProgram();
    initNewMethodProgram();
    initScreenQuadMemory();
    initOldMethodVAO();
    initNewMethodVAO();
    initOfflineRenderingObjects();

}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool FluidRenderer::initOldMethodProgram()
{
    GLint location;
    programOldMethod = new QOpenGLShaderProgram;
    QOpenGLShaderProgram* program = programOldMethod;
    bool success;

    success = program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                               ":/shaders/old.vs.glsl");
    assert(success);

    success = program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                               ":/shaders/old.fs.glsl");
    assert(success);

    success = program->link();
    assert(success);

    location = program->attributeLocation("v_coord");
    assert(location >= 0);
    attrVertexOldMethod = location;

    location = program->attributeLocation("v_texcoord");
    assert(location >= 0);
    attrTexCoordOldMethod = location;

    return true;
}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool FluidRenderer::initNewMethodProgram()
{
    GLint location;
    programNewMethodFirst = new QOpenGLShaderProgram;
    QOpenGLShaderProgram* program = programNewMethodFirst;
    bool success;

    success = program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                               ":/shaders/new-first.vs.glsl");
    assert(success);

    success = program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                               ":/shaders/new-first.fs.glsl");
    assert(success);

    success = program->link();
    assert(success);

    location = program->attributeLocation("v_coord");
    assert(location >= 0);
    attrVertexNewMethodFirst = location;

    location = program->attributeLocation("v_texcoord");
    assert(location >= 0);
    attrTexCoordNewMethodFirst = location;

    ////////////////////////////////////////////////////////////////////////////////
    programNewMethodSecond = new QOpenGLShaderProgram;
    program = programNewMethodSecond;

    success = program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                               ":/shaders/new-second.vs.glsl");
    assert(success);

    success = program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                               ":/shaders/new-second.fs.glsl");
    assert(success);

    success = program->link();
    assert(success);

    location = program->attributeLocation("v_coord");
    assert(location >= 0);
    attrVertexNewMethodSecond = location;

    location = program->attributeLocation("v_texcoord");
    assert(location >= 0);
    attrTexCoordNewMethodSecond = location;

    ////////////////////////////////////////////////////////////////////////////////
    return true;
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FluidRenderer::initScreenQuadMemory()
{
    GLfloat verts[] = {  -1, -1, 0,
                         1, -1, 0,
                         1, 1, 0,
                         -1, 1, 0
                      };


    GLfloat texs[] = {  0, 0,
                        1, 0,
                        1, 1,
                        0, 1
                     };


    GLushort indices[] = {0,  1,  2,
                          0, 2, 3
                         };

    ////////////////////////////////////////////////////////////////////////////////
    // init memory for background cube
    vboScreenQuad.create();
    vboScreenQuad.bind();
    vboScreenQuad.allocate((12 + 8) * sizeof(GLfloat));
    vboScreenQuad.write(0, verts, 12 * sizeof(GLfloat));
    vboScreenQuad.write(12 * sizeof(GLfloat), texs, 8 * sizeof(GLfloat));

    vboScreenQuad.release();

    // indices
    iboScreenQuad.create();
    iboScreenQuad.bind();
    iboScreenQuad.allocate(indices, 6 * sizeof(GLushort));
    iboScreenQuad.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FluidRenderer::initOldMethodVAO()
{
    QOpenGLShaderProgram* program = programOldMethod;

    vaoOldMethod.create();
    vaoOldMethod.bind();

    vboScreenQuad.bind();
    program->enableAttributeArray(attrVertexOldMethod);
    program->setAttributeBuffer(attrVertexOldMethod, GL_FLOAT, 0, 3);

    program->enableAttributeArray(attrTexCoordOldMethod);
    program->setAttributeBuffer(attrTexCoordOldMethod, GL_FLOAT, 12 * sizeof(GLfloat), 2);

    iboScreenQuad.bind();

    // release vao before vbo and ibo
    vaoOldMethod.release();
    vboScreenQuad.release();
    iboScreenQuad.release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FluidRenderer::initNewMethodVAO()
{
    QOpenGLShaderProgram* program = programNewMethodFirst;

    vaoNewMethodFirst.create();
    vaoNewMethodFirst.bind();

    vboScreenQuad.bind();
    program->enableAttributeArray(attrVertexNewMethodFirst);
    program->setAttributeBuffer(attrVertexNewMethodFirst, GL_FLOAT, 0, 3);

    program->enableAttributeArray(attrTexCoordNewMethodFirst);
    program->setAttributeBuffer(attrTexCoordNewMethodFirst, GL_FLOAT, 12 * sizeof(GLfloat),
                                2);

    iboScreenQuad.bind();

    // release vao before vbo and ibo
    vaoNewMethodFirst.release();
    vboScreenQuad.release();
    iboScreenQuad.release();

    ////////////////////////////////////////////////////////////////////////////////
    program = programNewMethodSecond;
    vaoNewMethodSecond.create();
    vaoNewMethodSecond.bind();

    vboScreenQuad.bind();
    program->enableAttributeArray(attrVertexNewMethodSecond);
    program->setAttributeBuffer(attrVertexNewMethodSecond, GL_FLOAT, 0, 3);

    program->enableAttributeArray(attrTexCoordNewMethodSecond);
    program->setAttributeBuffer(attrTexCoordNewMethodSecond, GL_FLOAT, 12 * sizeof(GLfloat),
                                2);

    iboScreenQuad.bind();

    // release vao before vbo and ibo
    vaoNewMethodSecond.release();
    vboScreenQuad.release();
    iboScreenQuad.release();

}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FluidRenderer::initOfflineRenderingObjects()
{
    ////////////////////////////////////////////////////////////////////////////////
    // textures
    glGenTextures(1, &texDepthOutput);
    glBindTexture(GL_TEXTURE_2D, texDepthOutput);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 RENDER_TEXTURE_SIZE, RENDER_TEXTURE_SIZE,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);


    glGenTextures(1, &texDepthInput);
    glBindTexture(GL_TEXTURE_2D, texDepthInput);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 RENDER_TEXTURE_SIZE, RENDER_TEXTURE_SIZE,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);


    // textures float data
    glGenTextures(1, &tex4Element0);
    glBindTexture(GL_TEXTURE_2D, tex4Element0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
                 RENDER_TEXTURE_SIZE, RENDER_TEXTURE_SIZE,
                 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);


    glGenTextures(1, &tex4Element1);
    glBindTexture(GL_TEXTURE_2D, tex4Element1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
                 RENDER_TEXTURE_SIZE, RENDER_TEXTURE_SIZE,
                 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);


    glGenTextures(1, &tex4Element2);
    glBindTexture(GL_TEXTURE_2D, tex4Element2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
                 RENDER_TEXTURE_SIZE, RENDER_TEXTURE_SIZE,
                 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);




    ////////////////////////////////////////////////////////////////////////////////
    // frame buffer objects
    glGenFramebuffers(1, &FBO1TexOutput);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO1TexOutput);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                         texDepthOutput, 0);
    assert(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


    glGenFramebuffers(1, &FBOTManyTexOutput);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBOTManyTexOutput);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           tex4Element0, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                           tex4Element1, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                           tex4Element2, 0);
    assert(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FluidRenderer::OldMethod()
{
//    PROFILE("Old Method");


    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO1TexOutput);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glViewport(0, 0, RENDER_TEXTURE_SIZE, RENDER_TEXTURE_SIZE);


    programOldMethod->bind();
    programOldMethod->setUniformValue("u_Tex", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texDepthInput);
    glDrawBuffer(GL_NONE);

    vaoOldMethod.bind();

    for(int i = 0; i < numIterations; ++i)
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    }

    vaoOldMethod.release();
    programOldMethod->release();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


    // output to screen
    glViewport(0, 0, width(), height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    programOldMethod->bind();
    programOldMethod->setUniformValue("u_Tex", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texDepthInput);
    vaoOldMethod.bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    vaoOldMethod.release();
    programOldMethod->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void FluidRenderer::NewMethod()
{
//    PROFILE("New Method");


    GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glViewport(0, 0, RENDER_TEXTURE_SIZE, RENDER_TEXTURE_SIZE);


    for(int i = 0; i < numIterations; ++i)
    {
        ////////////////////////////////////////////////////////////////////////////////
        // first pass
        // only write out to buffers, not any depth value
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBOTManyTexOutput);
//        glClear(GL_COLOR_BUFFER_BIT);
        glDrawBuffer(GL_TRUE);
        glDrawBuffers(3, (GLenum*)buffers);
        glDisable(GL_DEPTH_TEST);

        programNewMethodFirst->bind();
        programNewMethodFirst->setUniformValue("u_Tex", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texDepthInput);

        vaoNewMethodFirst.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        vaoNewMethodFirst.release();

        programNewMethodFirst->release();

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);






        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // second pass
        // only write out depth value, does draw to any buffer

#if 1
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO1TexOutput);

        programNewMethodSecond->bind();
        programNewMethodSecond->setUniformValue("u_Tex1", 0);
        programNewMethodSecond->setUniformValue("u_Tex2", 1);
        programNewMethodSecond->setUniformValue("u_Tex3", 2);

        glDrawBuffer(GL_NONE);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_ALWAYS);
        vaoNewMethodSecond.bind();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex4Element0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex4Element1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, tex4Element2);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

        vaoNewMethodSecond.release();
        programNewMethodSecond->release();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
#endif
    }



    // output to screen
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_ALWAYS);
    glViewport(0, 0, width(), height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    programNewMethodSecond->bind();

    vaoNewMethodSecond.bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    vaoNewMethodSecond.release();
    programNewMethodSecond->release();
}
