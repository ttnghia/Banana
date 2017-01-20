//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// TestGLSLWidget.cpp
//
// Created on: 8/10/2016
//     Author: Nghia Truong
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include "TestGLSLWidget.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
TestGLSLWidget::TestGLSLWidget(QWidget* parent) :
    OpenGLWidget(parent),
    initializedTestScene(false),
    testProgram(nullptr)
{
    defaultSize = QSize(1500, 1500);

    camera.setDefaultCamera(glm::vec3 (10.0f,  0.0f, 3.0f),
                            glm::vec3 (10.0f, 0.0f, 0.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TestGLSLWidget::initTestScene()
{
    bool success;
    testProgram = new QOpenGLShaderProgram;

    success = testProgram ->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                                    ":/shaders/test.vs.glsl");
    assert(success);

    success = testProgram ->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                                    ":/shaders/test.fs.glsl");
    assert(success);

    success = testProgram ->link();
    assert(success);

    glUseProgram(testProgram ->programId());

//    const char* attribute_coord = ;
//    const char* attribute_color = ;
    GLint attribute_vertex = glGetAttribLocation(testProgram->programId(), "v_coord");
    GLint attribute_color = glGetAttribLocation(testProgram->programId(), "v_color");

    if(attribute_vertex == -1 || attribute_color == -1)
    {
        qDebug() << "Could not bind attribute(s)";
        return;
    }

    GLfloat triangle_attributes[] =
    {
        10.0,  0.8, 0.0,   1.0, 0.0, 0.0,
        9.2, -0.8, 0.0,   0.0, 0.0, 1.0,
        10.8, -0.8, 0.0,   0.0, 1.0, 0.0,
    };
    glGenBuffers(1, &vbo_triangle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_attributes), triangle_attributes,
                 GL_STATIC_DRAW);


    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(attribute_vertex);
    glEnableVertexAttribArray(attribute_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
//    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(
        attribute_vertex,   // attribute
        3,                   // number of elements per vertex, here (x,y,z)
        GL_FLOAT,            // the type of each element
        GL_FALSE,            // take our values as-is
        6 * sizeof(GLfloat), // next coord3d appears every 6 floats
        0                    // offset of the first element
    );
    glVertexAttribPointer(
        attribute_color,      // attribute
        3,                      // number of elements per vertex, here (r,g,b)
        GL_FLOAT,               // the type of each element
        GL_FALSE,               // take our values as-is
        6 * sizeof(GLfloat),    // next color appears every 6 floats
        (GLvoid*) (3 * sizeof(GLfloat))  // offset of first element
    );


    initializedTestScene = true;
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TestGLSLWidget::initializeGL()
{
    OpenGLWidget::initializeGL();
    checkGLVersion();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TestGLSLWidget::renderTestScene()
{
    if(!initializedTestScene)
    {
        initTestScene();
    }

    assert(testProgram != nullptr);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(testProgram->programId());
    glBindVertexArray(vao);

    int location = glGetUniformLocation(testProgram->programId(), "viewProjectionMatrix");

//    qDebug() << QString::fromStdString(glm::to_string(viewProjectionMatrix));

//    qDebug() << location;
    glUniformMatrix4fv(location, 1 /*only setting 1 matrix*/, false /*transpose?*/,
                       glm::value_ptr(camera.getViewProjectionMatrix()));



    // draw points 0-3 from the currently bound VAO with current in-use shader

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glUseProgram(0);
}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TestGLSLWidget::paintGL()
{
    //    glClearColor(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX,
    //                 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


//    const glm::mat4& mat_rot = arcball.get_rotation_matrix();

//    const glm::mat4& mat_trans = glm::translate(glm::mat4(1.f), camera.cameraFocus);
//    const glm::mat4& mat_trans_inv = glm::translate(glm::mat4(1.f), -camera.cameraFocus);

//    viewProjectionMatrix = projectionMatrix * camera.getViewMatrix() *
//                           (mat_trans * mat_rot * mat_trans_inv);

    renderTestScene();
}
