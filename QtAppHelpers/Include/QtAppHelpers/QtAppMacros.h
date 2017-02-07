//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/21/2017
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __APPLE__
#include <OpenGL.h>
#   include <QOpenGLFunctions_4_1_Core>
typedef  QOpenGLFunctions_4_1_Core OpenGLFunctions;
#else
#   include <QOpenGLFunctions_4_5_Core>
typedef  QOpenGLFunctions_4_5_Core OpenGLFunctions;
#endif

#define __Banana_Qt__

#include <QDebug>
#include <QMessageBox>
#ifdef __BNN_Err
#   undef __BNN_Err
#endif
#define __BNN_Err(err) \
 { \
     QMessageBox::critical(nullptr, QString("Error"), err); \
 }

#ifdef __BNN_Info
#   undef __BNN_Info
#endif
#define __BNN_Info(info) \
 { \
     QMessageBox::information(nullptr, QString("Info"), info); \
 }


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

#if defined(_WIN32) || defined(_WIN64)
#   ifndef NOMINMAX
#      define NOMINMAX
#   endif
#   define __func__ __FUNCTION__
#endif 
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __APPLE__
#define __BNNQt_RunMainWindow __BNNQt_RunMainWindowMac
#define __BNNQt_RunMainWindowSplashScreen __BNNQt_RunMainWindowMacSplashScreen
#else
#define __BNNQt_RunMainWindow __BNNQt_RunMainWindowWin
#define __BNNQt_RunMainWindowSplashScreen __BNNQt_RunMainWindowWinSplashScreen
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNNQt_RunMainWindowWin(MainWindowClass, argc, argv, vsync) \
{ \
    QSurfaceFormat format; \
    format.setDepthBufferSize(24); \
    format.setStencilBufferSize(8); \
    format.setVersion(4, 5); \
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer); \
    format.setProfile(QSurfaceFormat::CoreProfile); \
    format.setSamples(4); \
    format.setSwapInterval(vsync ? 1 : 0); \
    QSurfaceFormat::setDefaultFormat(format); \
    QApplication a(argc, argv); \
    MainWindowClass w; \
    w.show(); \
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, \
                                      w.size(), \
                                      qApp->desktop()->availableGeometry())); \
    return a.exec(); \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <BananaAppCore/NumberHelpers.h>
#include <QtAppHelpers/CustomSplashScreen.h>
#include <QDir>
#define __BNNQt_RunMainWindowWinSplashScreen(MainWindowClass, argc, argv, vsync, FSPath) \
{ \
    QSurfaceFormat format; \
    format.setDepthBufferSize(24); \
    format.setStencilBufferSize(8); \
    format.setVersion(4, 5); \
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer); \
    format.setProfile(QSurfaceFormat::CoreProfile); \
    format.setSamples(4); \
    format.setSwapInterval(vsync ? 1 : 0); \
    QSurfaceFormat::setDefaultFormat(format); \
    QApplication a(argc, argv); \
    QDir imgDir(FSPath);\
    imgDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);\
    QStringList imgFiles = imgDir.entryList();\
    QPixmap pixmap(FSPath + "/" + imgFiles[NumberHelpers::generate_random_int(0, imgFiles.size() - 1)]); \
    CustomSplashScreen splash(pixmap); \
    splash.show(); \
    splash.showStatusMessage(QObject::tr("Initializing..."), QColor(50, 0, 255));\
    MainWindowClass w; \
    w.show(); \
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, \
              w.size(), \
              qApp->desktop()->availableGeometry())); \
    splash.finish(&w);\
    return a.exec(); \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNNQt_RunMainWindowMac(MainWindowClass, argc, argv, vsync) \
{ \
    QSurfaceFormat format; \
    format.setDepthBufferSize(24); \
    format.setStencilBufferSize(8); \
    format.setVersion(4, 1); \
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer); \
    format.setProfile(QSurfaceFormat::CoreProfile); \
    format.setSamples(4); \
    format.setSwapInterval(vsync ? 1 : 0); \
    QSurfaceFormat::setDefaultFormat(format); \
    QApplication a(argc, argv); \
    MainWindowClass w; \
    w.show(); \
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, \
                                      w.size(), \
                                      qApp->desktop()->availableGeometry())); \
    return a.exec(); \
}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNNQt_RunMainWindowMacSplashScreen(MainWindowClass, argc, argv, vsync, FSPath) \
{ \
    QSurfaceFormat format; \
    format.setDepthBufferSize(24); \
    format.setStencilBufferSize(8); \
    format.setVersion(4, 1); \
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer); \
    format.setProfile(QSurfaceFormat::CoreProfile); \
    format.setSamples(4); \
    format.setSwapInterval(vsync ? 1 : 0); \
    QSurfaceFormat::setDefaultFormat(format); \
    QApplication a(argc, argv); \
    QDir imgDir(FSPath); \
    imgDir.setFilter(QDir::NoDotAndDotDot | QDir::Files); \
    QStringList imgFiles = imgDir.entryList(); \
    QPixmap pixmap(FSPath + "/" + imgFiles[NumberHelpers::generate_random_int(0, imgFiles.size() - 1)]); \
    CustomSplashScreen splash(pixmap); \
    splash.show(); \
    splash.showStatusMessage(QObject::tr("Initializing..."), QColor(50, 0, 255)); \
    MainWindowClass w; \
    w.show(); \
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, \
                                      w.size(), \
                                      qApp->desktop()->availableGeometry())); \
    splash.finish(&w);\
    return a.exec(); \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNNQt_PrintLine \
{ \
    qDebug()<< "Line:" << __LINE__ << ", file:" << __FILE__; \
}

#define __BNNQt_PrintExp(x) \
{ \
    qDebug() << "Printing at line: " << __LINE__ << ", file: " << __FILE__ << ":"; \
    qDebug() << "    " << #x << ": " << x; \
}

#define __BNNQt_PrintLocation \
{ \
    qDebug() << "Function: " << __func__; \
    qDebug() << "Line: " << __LINE__ << ", file: " << __FILE__; \
}

#define __BNNQt_Die(errMsg) \
{ \
    qDebug()<< "Error occured at line:" << __LINE__ << ", file:" << __FILE__; \
    qDebug()<< "Error message:" << errMsg; \
    exit(EXIT_FAILURE); \
}

#define __BNNQt_AssertMsg(condition, errMsg) \
{ \
    if(!(condition)) \
    { \
        qDebug()<< "Fatal error occured at line:" << __LINE__ << ", file:" << __FILE__; \
        qDebug()<< "Error message:" << errMsg; \
        exit(EXIT_FAILURE); \
    } \
}

#define __BNNQt_Check(condition, errStr) \
{ \
    if(!(condition)) \
    { \
        QMessageBox::information(this, "Error", errStr); \
        return false; \
    } \
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
