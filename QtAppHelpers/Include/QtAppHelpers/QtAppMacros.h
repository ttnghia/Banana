//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __APPLE__
#include <OpenGL.h>
#   include <QOpenGLFunctions_4_1_Core>
typedef  QOpenGLFunctions_4_1_Core   OpenGLFunctions;
#else
#   include <QOpenGLFunctions_4_1_Core>
typedef  QOpenGLFunctions_4_1_Core   OpenGLFunctions;
#endif

#define __Banana_Qt__

#include <QDebug>
#include <QMessageBox>
#ifdef __BNN_ERROR
#   undef __BNN_ERROR
#endif
#define __BNN_ERROR(err)                                       \
    {                                                          \
        QMessageBox::critical(nullptr, QString("Error"), err); \
    }

#ifdef __BNN_INFO
#   undef __BNN_INFO
#endif
#define __BNN_INFO(info)                                          \
    {                                                             \
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
#define __BNNQt_RunMainWindow             __BNNQt_RunMainWindowMac
#define __BNNQt_RunMainWindowSplashScreen __BNNQt_RunMainWindowMacSplashScreen
#else
#define __BNNQt_RunMainWindow             __BNNQt_RunMainWindowWin
#define __BNNQt_RunMainWindowSplashScreen __BNNQt_RunMainWindowWinSplashScreen
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNNQt_RunMainWindowWin(MainWindowClass, argc, argv, vsync)                                                          \
    {                                                                                                                         \
        QSurfaceFormat format;                                                                                                \
        format.setDepthBufferSize(24);                                                                                        \
        format.setStencilBufferSize(8);                                                                                       \
        format.setVersion(4, 5);                                                                                              \
        format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);                                                                 \
        format.setProfile(QSurfaceFormat::CoreProfile);                                                                       \
        format.setSwapInterval(vsync ? 1 : 0);                                                                                \
        QSurfaceFormat::setDefaultFormat(format);                                                                             \
        QApplication a(argc, argv);                                                                                           \
        a.setAttribute(Qt::AA_UseDesktopOpenGL);                                                                              \
        MainWindowClass w;                                                                                                    \
        w.show();                                                                                                             \
        w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w.size(), qApp->desktop()->availableGeometry())); \
        return a.exec();                                                                                                      \
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#include <Banana/Utils/NumberHelpers.h>
#include <QtAppHelpers/CustomSplashScreen.h>
#include <QDir>
#define __BNNQt_RunMainWindowWinSplashScreen(MainWindowClass, argc, argv, vsync, FSPath)                                      \
    {                                                                                                                         \
        QSurfaceFormat format;                                                                                                \
        format.setDepthBufferSize(24);                                                                                        \
        format.setStencilBufferSize(8);                                                                                       \
        format.setVersion(4, 5);                                                                                              \
        format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);                                                                 \
        format.setProfile(QSurfaceFormat::CoreProfile);                                                                       \
        format.setSwapInterval(vsync ? 1 : 0);                                                                                \
        QSurfaceFormat::setDefaultFormat(format);                                                                             \
        QApplication a(argc, argv);                                                                                           \
        QDir imgDir(FSPath);                                                                                                  \
        imgDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);                                                                 \
        QStringList imgFiles = imgDir.entryList();                                                                            \
        QPixmap pixmap(FSPath + "/" + imgFiles[NumberHelpers::generateRandomInt(0, imgFiles.size() - 1)]);                    \
        CustomSplashScreen splash(pixmap);                                                                                    \
        splash.show();                                                                                                        \
        splash.showStatusMessage(QObject::tr("Initializing..."), QColor(50, 0, 255));                                         \
        MainWindowClass w;                                                                                                    \
        w.show();                                                                                                             \
        w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w.size(), qApp->desktop()->availableGeometry())); \
        splash.hide();                                                                                                        \
        QTimer::singleShot(2500, &splash, SLOT(close()));                                                                     \
        return a.exec();                                                                                                      \
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNNQt_RunMainWindowMac(MainWindowClass, argc, argv, vsync)                                                          \
    {                                                                                                                         \
        QSurfaceFormat format;                                                                                                \
        format.setDepthBufferSize(24);                                                                                        \
        format.setStencilBufferSize(8);                                                                                       \
        format.setVersion(4, 1);                                                                                              \
        format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);                                                                 \
        format.setProfile(QSurfaceFormat::CoreProfile);                                                                       \
        format.setSwapInterval(vsync ? 1 : 0);                                                                                \
        QSurfaceFormat::setDefaultFormat(format);                                                                             \
        QApplication a(argc, argv);                                                                                           \
        MainWindowClass w;                                                                                                    \
        w.show();                                                                                                             \
        w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w.size(), qApp->desktop()->availableGeometry())); \
        return a.exec();                                                                                                      \
    }
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNNQt_RunMainWindowMacSplashScreen(MainWindowClass, argc, argv, vsync, FSPath)                                      \
    {                                                                                                                         \
        QSurfaceFormat format;                                                                                                \
        format.setDepthBufferSize(24);                                                                                        \
        format.setStencilBufferSize(8);                                                                                       \
        format.setVersion(4, 1);                                                                                              \
        format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);                                                                 \
        format.setProfile(QSurfaceFormat::CoreProfile);                                                                       \
        format.setSwapInterval(vsync ? 1 : 0);                                                                                \
        QSurfaceFormat::setDefaultFormat(format);                                                                             \
        QApplication a(argc, argv);                                                                                           \
        QDir imgDir(FSPath);                                                                                                  \
        imgDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);                                                                 \
        QStringList imgFiles = imgDir.entryList();                                                                            \
        QPixmap pixmap(FSPath + "/" + imgFiles[NumberHelpers::generateRandomInt(0, imgFiles.size() - 1)]);                    \
        CustomSplashScreen splash(pixmap);                                                                                    \
        splash.show();                                                                                                        \
        splash.showStatusMessage(QObject::tr("Initializing..."), QColor(50, 0, 255));                                         \
        MainWindowClass w;                                                                                                    \
        w.show();                                                                                                             \
        w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w.size(), qApp->desktop()->availableGeometry())); \
        splash.hide();                                                                                                        \
        QTimer::singleShot(2500, &splash, SLOT(close()));                                                                     \
        return a.exec();                                                                                                      \
    }

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __BNNQt_PrintLine                                         \
    {                                                             \
        qDebug() << "Line:" << __LINE__ << ", file:" << __FILE__; \
    }

#define __BNNQt_PrintExp(x)                                                            \
    {                                                                                  \
        qDebug() << "Printing at line: " << __LINE__ << ", file: " << __FILE__ << ":"; \
        qDebug() << "    " << #x << ": " << x;                                         \
    }

#define __BNNQt_PrintLocation                                       \
    {                                                               \
        qDebug() << "Function: " << __func__;                       \
        qDebug() << "Line: " << __LINE__ << ", file: " << __FILE__; \
    }

#define __BNNQt_Die(errMsg)                                                        \
    {                                                                              \
        qDebug() << "Error occured at line:" << __LINE__ << ", file:" << __FILE__; \
        qDebug() << "Error message:" << errMsg;                                    \
        exit(EXIT_FAILURE);                                                        \
    }

#define __BNNQt_AssertMsg(condition, errMsg)                                                 \
    {                                                                                        \
        if(!(condition))                                                                     \
        {                                                                                    \
            qDebug() << "Fatal error occured at line:" << __LINE__ << ", file:" << __FILE__; \
            qDebug() << "Error message:" << errMsg;                                          \
            exit(EXIT_FAILURE);                                                              \
        }                                                                                    \
    }

#define __BNNQt_Check(condition, errStr)                     \
    {                                                        \
        if(!(condition))                                     \
        {                                                    \
            QMessageBox::information(this, "Error", errStr); \
            return false;                                    \
        }                                                    \
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
        fov   = _fov;
        neard = _near;
        fard  = _far;
    }
};
