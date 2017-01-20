//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/19/2017
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

#pragma once

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __APPLE__
#include <OpenGL.h>
#include <QOpenGLFunctions_4_1_Core>
typedef  QOpenGLFunctions_4_1_Core OpenGLFunctions;
#else
#include <QOpenGLFunctions_4_5_Core>
typedef  QOpenGLFunctions_4_5_Core OpenGLFunctions;
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __APPLE__
#define __BNN_RunMainWindow __BNN_RunMainWindowMac
#else
#define __BNN_RunMainWindow __BNN_RunMainWindowWin
#endif


#define __BNN_RunMainWindowWin(MainWindowClass, argc, argv) \
{ \
    QSurfaceFormat format; \
    format.setDepthBufferSize(24); \
    format.setStencilBufferSize(8); \
    format.setVersion(4, 5); \
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer); \
    format.setProfile(QSurfaceFormat::CoreProfile); \
    format.setSamples(4); \
    QSurfaceFormat::setDefaultFormat(format); \
    QApplication a(argc, argv); \
    MainWindowClass w; \
    w.show(); \
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, \
                                      w.size(), \
                                      qApp->desktop()->availableGeometry())); \
    return a.exec(); \
}

#define __BNN_RunMainWindowMac(MainWindowClass, argc, argv) \
{ \
    QSurfaceFormat format; \
    format.setDepthBufferSize(24); \
    format.setStencilBufferSize(8); \
    format.setVersion(4, 1); \
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer); \
    format.setProfile(QSurfaceFormat::CoreProfile); \
    format.setSamples(4); \
    QSurfaceFormat::setDefaultFormat(format); \
    QApplication a(argc, argv); \
    MainWindowClass w; \
    w.show(); \
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, \
                                      w.size(), \
                                      qApp->desktop()->availableGeometry())); \
    return a.exec(); \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// => enums

enum class SpecialKey
{
    NoKey,
    ShiftKey,
    CtrlKey,
    AltKey
};

enum class MouseButton
{
    NoButton,
    LeftButton,
    MiddleButton,
    RightButton
};

enum class MeshFileType
{
    OBJFile,
    PLYFile,
    UnsupportedFileType
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// => structs
struct Frustum
{
    float fov;
    float neard;
    float fard;


    Frustum(float _fov = 45.0f, float _near = 0.1f, float _far = 1000.0f)
    {
        fov = _fov;
        neard = _near;
        fard = _far;
    }

};

//struct RenderObject
//{
//    GLuint vao;
//    GLuint vbo;
//    unsigned int num_triangles;

//    bool has_vertex_normal;
//    bool has_vertex_color;
//};

struct Resolution2D
{
    Resolution2D(int _x, int _y)
    {
        x = _x;
        y = _y;
    }
    int x;
    int y;
};

struct Resolution3D
{
    Resolution3D(int _x, int _y, int _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    int x;
    int y;
    int z;
};
