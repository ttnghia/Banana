//------------------------------------------------------------------------------------------
// TestFixedGLWidget.cpp
//
// Created on: 8/10/2016
//     Author: Nghia Truong
//
//------------------------------------------------------------------------------------------
#include "TestFixedGLWidget.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif


//------------------------------------------------------------------------------------------
TestFixedGLWidget::TestFixedGLWidget(QWidget* parent) :
    QOpenGLWidget(parent)
{


    camera.setDefaultCamera(glm::vec3 (0.0f,  0.0f, 40.0f),
                            glm::vec3 (0.0f, 0.0f, 0.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f));
}

//------------------------------------------------------------------------------------------
inline void WireSphere(int obj, float r, int sec, int slice)
{
    glNewList(obj, GL_COMPILE);

    glColor3f(0, 0.5, 0.5);
    GLUquadricObj* sphereObj = gluNewQuadric();
    gluQuadricDrawStyle(sphereObj, GLU_FILL);
    gluSphere(sphereObj, r, sec, slice);

    glColor3f(1, 1, 0);
    GLUquadricObj* sphereObjWire = gluNewQuadric();
    gluQuadricDrawStyle(sphereObjWire, GLU_LINE);
    gluSphere(sphereObjWire, r * 1.01, sec, slice);

    glEndList();
}

//------------------------------------------------------------------------------------------
void TestFixedGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    WireSphere(1, 10, 40, 38);
}

//------------------------------------------------------------------------------------------
void TestFixedGLWidget::resizeGL(int w, int h)
{
    camera.resizeWindow((float)w, (float)h);

    float aspect = (double)w / (double)h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 150.0);
    glMatrixMode(GL_MODELVIEW);
}

//------------------------------------------------------------------------------------------
void TestFixedGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(camera.cameraPosition.x, camera.cameraPosition.y, camera.cameraPosition.z,
              camera.cameraFocus.x, camera.cameraFocus.y, camera.cameraFocus.z,
              camera.cameraUpDirection.x, camera.cameraUpDirection.y, camera.cameraUpDirection.z);

//    glMultMatrixf(arcball.get_rotation_data());

//    qDebug() << QString::fromStdString(glm::to_string(arcball.get_rotation_matrix()));


    // draw axis
    glBegin(GL_LINES);
    glColor4f(1.0, 0.0, 0.0, 1.0);
    glVertex3f( 0.0,  0.0,  0.0);
    glVertex3f(10.0,  0.0,  0.0);
    glColor4f(0.0, 1.0, 0.0, 1.0);
    glVertex3f( 0.0,  0.0,  0.0);
    glVertex3f( 0.0, 10.0,  0.0);
    glColor4f(0.0, 0.0, 1.0, 1.0);
    glVertex3f( 0.0,  0.0,  0.0);
    glVertex3f( 0.0,  0.0, 10.0);
    glEnd();

    // draw sphere
    glColor4f(0.2, 0.8, 0.1, 1.0);

    glCallList(1);

    glFlush();
}
