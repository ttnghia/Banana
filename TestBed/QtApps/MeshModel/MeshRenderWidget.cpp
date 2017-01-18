//------------------------------------------------------------------------------------------
// MeshRenderWidget.cpp
//
// Created on: 9/5/2016
//     Author: Nghia Truong
//
//------------------------------------------------------------------------------------------
#include "MeshRenderWidget.h"

//------------------------------------------------------------------------------------------
MeshRenderWidget::MeshRenderWidget(QWidget* parent): OpenGLWidget(parent),
    mesh_ready(false),
    meshModel(nullptr)
{
    defaultSize = QSize(1500, 1500);
}

//------------------------------------------------------------------------------------------
// This example displays one of the following shapes
typedef enum { SHAPE_TEAPOT = 1, SHAPE_TORUS, SHAPE_CONE } Shape;
#define NUM_SHAPES 3
Shape g_CurrentShape = SHAPE_TORUS;
// Shapes scale
float g_Zoom = 1.0f;
// Shape orientation (stored as a quaternion)
float g_Rotation[] = { 0.0f, 0.0f, 0.0f, 1.0f };
// Auto rotate
int g_AutoRotate = 0;
int g_RotateTime = 0;
float g_RotateStart[] = { 0.0f, 0.0f, 0.0f, 1.0f };
// Shapes material
float g_MatAmbient[] = { 0.5f, 0.0f, 0.0f, 1.0f };
float g_MatDiffuse[] = { 1.0f, 1.0f, 0.0f, 1.0f };
// Light parameter
float g_LightMultiplier = 1.0f;
float g_LightDirection[] = { -0.57735f, -0.57735f, -0.57735f };

//  Callback function called when the 'AutoRotate' variable value of the tweak bar has changed
void TW_CALL SetAutoRotateCB(const void* value, void* clientData)
{
    (void)clientData; // unused

    g_AutoRotate = *(const int*)value;  // copy value to g_AutoRotate

    if( g_AutoRotate != 0 )
    {
        // init rotation
        g_RotateTime = 0;
        g_RotateStart[0] = g_Rotation[0];
        g_RotateStart[1] = g_Rotation[1];
        g_RotateStart[2] = g_Rotation[2];
        g_RotateStart[3] = g_Rotation[3];

        // make Rotation variable read-only
        TwDefine(" TweakBar/ObjRotation readonly ");
    }
    else
        // make Rotation variable read-write
    {
        TwDefine(" TweakBar/ObjRotation readwrite ");
    }
}


//  Callback function called by the tweak bar to get the 'AutoRotate' value
void TW_CALL GetAutoRotateCB(void* value, void* clientData)
{
    (void)clientData; // unused
    *(int*)value = g_AutoRotate;  // copy g_AutoRotate to value
}


void MeshRenderWidget::initializeGL()
{
    OpenGLWidget::initializeGL();
    checkGLVersion();

    antTweakBar = TwNewBar("NameOfMyTweakBar");

    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLUT and OpenGL.' "); // Message added to the help bar.
    TwDefine(" TweakBar size='200 400' color='96 216 224' "); // change default tweak bar size and color

    // Add 'g_Zoom' to 'bar': this is a modifable (RW) variable of type TW_TYPE_FLOAT. Its key shortcuts are [z] and [Z].
    TwAddVarRW(antTweakBar, "Zoom", TW_TYPE_FLOAT, &g_Zoom,
               " min=0.01 max=2.5 step=0.01 keyIncr=z keyDecr=Z help='Scale the object (1=original size).' ");

    // Add 'g_Rotation' to 'bar': this is a variable of type TW_TYPE_QUAT4F which defines the object's orientation
    TwAddVarRW(antTweakBar, "ObjRotation", TW_TYPE_QUAT4F, &g_Rotation,
               " label='Object rotation' opened=true help='Change the object orientation.' ");

    // Add callback to toggle auto-rotate mode (callback functions are defined above).
    TwAddVarCB(antTweakBar, "AutoRotate", TW_TYPE_BOOL32, SetAutoRotateCB, GetAutoRotateCB,
               NULL,
               " label='Auto-rotate' key=space help='Toggle auto-rotate mode.' ");

    // Add 'g_LightMultiplier' to 'bar': this is a variable of type TW_TYPE_FLOAT. Its key shortcuts are [+] and [-].
    TwAddVarRW(antTweakBar, "Multiplier", TW_TYPE_FLOAT, &g_LightMultiplier,
               " label='Light booster' min=0.1 max=4 step=0.02 keyIncr='+' keyDecr='-' help='Increase/decrease the light power.' ");

    // Add 'g_LightDirection' to 'bar': this is a variable of type TW_TYPE_DIR3F which defines the light direction
    TwAddVarRW(antTweakBar, "LightDir", TW_TYPE_DIR3F, &g_LightDirection,
               " label='Light direction' opened=true help='Change the light direction.' ");

    // Add 'g_MatAmbient' to 'bar': this is a variable of type TW_TYPE_COLOR3F (3 floats color, alpha is ignored)
    // and is inserted into a group named 'Material'.
    TwAddVarRW(antTweakBar, "Ambient", TW_TYPE_COLOR3F, &g_MatAmbient, " group='Material' ");

    // Add 'g_MatDiffuse' to 'bar': this is a variable of type TW_TYPE_COLOR3F (3 floats color, alpha is ignored)
    // and is inserted into group 'Material'.
    TwAddVarRW(antTweakBar, "Diffuse", TW_TYPE_COLOR3F, &g_MatDiffuse, " group='Material' ");

    // Add the enum variable 'g_CurrentShape' to 'bar'
    // (before adding an enum variable, its enum type must be declared to AntTweakBar as follow)
    {
        // ShapeEV associates Shape enum values with labels that will be displayed instead of enum values
        TwEnumVal shapeEV[NUM_SHAPES] = { {SHAPE_TEAPOT, "Teapot"}, {SHAPE_TORUS, "Torus"}, {SHAPE_CONE, "Cone"} };
        // Create a type for the enum shapeEV
        TwType shapeType = TwDefineEnum("ShapeType", shapeEV, NUM_SHAPES);
        // add 'g_CurrentShape' to 'bar': this is a variable of type ShapeType. Its key shortcuts are [<] and [>].
        TwAddVarRW(antTweakBar, "Shape", shapeType, &g_CurrentShape,
                   " keyIncr='<' keyDecr='>' help='Change object shape.' ");
    }
}

//------------------------------------------------------------------------------------------
void MeshRenderWidget::paintGL()
{
    if(!mesh_ready)
    {
        loadMesh();
    }

    assert(mesh_ready);

    //    glClearColor(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX,
    //                 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    renderMesh();
    renderAntTweakBar();
}

//------------------------------------------------------------------------------------------
void MeshRenderWidget::loadMesh()
{
    meshModel = new MeshModel;
    bool success =
#ifdef __APPLE__
        meshModel->load_mesh("/Volumes/Working/Programming/Mango/Assets/PLY/bunny.ply");
#else
        meshModel->load_mesh("D:/Programming/Mango/Assets/PLY/bunny.ply");
#endif
//            meshModel->load_mesh("/Volumes/Working/Programming/Mango/Assets/OBJ/cornell_box.obj");
//            meshModel->load_mesh("/Volumes/Working/Programming/Mango/Assets/OBJ/cube.obj");
//        meshModel->load_mesh("/Users/nghia/al.obj");


    if(!success)
    {
        mesh_ready = false;
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    shaderProgram = new QOpenGLShaderProgram;
    success = shaderProgram ->addShaderFromSourceFile(QOpenGLShader::Vertex,
//                                                      ":/shaders/phong-shading.vs.glsl");
                                                      ":/shaders/flat-shading.vs.glsl");
    assert(success);

    success = shaderProgram ->addShaderFromSourceFile(QOpenGLShader::Fragment,
//                                                      ":/shaders/phong-shading.fs.glsl");
                                                      ":/shaders/flat-shading.fs.glsl");
    assert(success);

    success = shaderProgram ->link();
    assert(success);

    glUseProgram(shaderProgram ->programId());

    GLint attribute_vertex = glGetAttribLocation(shaderProgram->programId(), "v_coord");
//    GLint attribute_normal = glGetAttribLocation(shaderProgram->programId(), "v_normal");
    GLint attribute_color = glGetAttribLocation(shaderProgram->programId(), "v_color");

//    if (attribute_vertex == -1 || attribute_normal == -1 || attribute_color == -1)
    if (attribute_vertex == -1 || attribute_color == -1)
    {
        qDebug() << "Could not bind attribute(s)";
        return;
    }

    // data: triangles * 3(vertex/tri) * 9(each vertex has coord3d + normal3d + color3d)
    GLsizeiptr bufferSize = (GLsizeiptr)sizeof(float) * meshModel->num_triangles * 3 * 9;
    GLsizeiptr blockDataSize = (GLsizeiptr)sizeof(float) * meshModel->num_triangles * 3 * 3;



    qDebug() << bufferSize << blockDataSize;
    qDebug() << meshModel->get_vertices().size()*sizeof(float);
    qDebug() << meshModel->num_triangles;

    glGenBuffers(1, &vbo_mesh);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, blockDataSize,
                    meshModel->get_vertices().data());
    glBufferSubData(GL_ARRAY_BUFFER, blockDataSize, blockDataSize,
                    meshModel->get_vcolors().data());
    glBufferSubData(GL_ARRAY_BUFFER, 2 * blockDataSize, blockDataSize,
                    meshModel->get_vnormals().data());


    glGenVertexArrays(1, &vao_mesh);
    glBindVertexArray(vao_mesh);
    glEnableVertexAttribArray(attribute_vertex);
    glEnableVertexAttribArray(attribute_color);
//    glEnableVertexAttribArray(attribute_normal);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh);

    glVertexAttribPointer(
        attribute_vertex,   // attribute
        3,                   // number of elements per vertex, here (x,y,z)
        GL_FLOAT,            // the type of each element
        GL_FALSE,            // take our values as-is
        3 * sizeof(GLfloat), // next coord3d appears every 5 floats
        0                    // offset of the first element
    );

    glVertexAttribPointer(
        attribute_color,   // attribute
        3,                   // number of elements per vertex, here (x,y,z)
        GL_FLOAT,            // the type of each element
        GL_FALSE,            // take our values as-is
        3 * sizeof(GLfloat), // next coord3d appears every 6 floats
        (void*)blockDataSize                    // offset of the first element
    );

//    glVertexAttribPointer(
//        attribute_normal,   // attribute
//        3,                   // number of elements per vertex, here (x,y,z)
//        GL_FLOAT,            // the type of each element
//        GL_FALSE,            // take our values as-is
//        3 * sizeof(GLfloat), // next coord3d appears every 6 floats
//        (void*)(2 * blockDataSize)                  // offset of the first element
//    );

    ////////////////////////////////////////////////////////////////////////////////
    // => setup camera
    meshModel->set_default_camera(camera);

    ////////////////////////////////////////////////////////////////////////////////
    mesh_ready = true;

}

//------------------------------------------------------------------------------------------
void MeshRenderWidget::renderMesh()
{
    assert(shaderProgram != nullptr);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram->programId());
    glBindVertexArray(vao_mesh);

//    int location_mm = glGetUniformLocation(shaderProgram->programId(), "modelMatrix");
    int location_mvp = glGetUniformLocation(shaderProgram->programId(),
                                            "viewProjectionMatrix");
//    int location_nm = glGetUniformLocation(shaderProgram->programId(), "normalMatrix");

//    qDebug() << QString::fromStdString(glm::to_string(viewProjectionMatrix));

//    qDebug() << location;
//    glUniformMatrix4fv(location_mm, 1 /*only setting 1 matrix*/, false /*transpose?*/,
//                       glm::value_ptr(glm::mat4(1.0)));
    glUniformMatrix4fv(location_mvp, 1 /*only setting 1 matrix*/, false /*transpose?*/,
                       glm::value_ptr(camera.getViewProjectionMatrix()));
//    glUniformMatrix4fv(location_nm, 1 /*only setting 1 matrix*/, false /*transpose?*/,
//                       glm::value_ptr(glm::mat4(1.0)));



    // draw points 0-3 from the currently bound VAO with current in-use shader
//qDebug() << "num tri: " << meshModel->num_triangles;

    glDrawArrays(GL_TRIANGLES, 0, meshModel->num_triangles * 3);
    glUseProgram(0);
}
