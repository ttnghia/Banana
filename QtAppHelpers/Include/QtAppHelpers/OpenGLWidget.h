//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/AvgTimer.h>
#include <QtAppHelpers/FPSCounter.h>
#include <QtAppHelpers/AntTweakBarWrapper.h>

#include <OpenGLHelpers/Camera.h>
#include <OpenGLHelpers/OpenGLBuffer.h>

#include <QtGui>
#include <QtWidgets>
#include <QSurfaceFormat>

#include <memory>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OpenGLWidget : public QOpenGLWidget, public OpenGLFunctions
#ifdef __BNN_USE_ANT_TWEAK_BAR
    , public AntTweakBarWrapper
#endif
{
    Q_OBJECT

public:

    OpenGLWidget(QWidget* parent);
    virtual ~OpenGLWidget();

    FPSCounter m_FPSCounter;

    void setPrintDebug(bool pdebug);
    void setUpdateTimeout(int timeout);
    void setDefaultSize(QSize size);
    void setClearColor(const glm::vec4& color);
    void setViewFrustum(float fov, float nearZ, float farZ);

    bool exportScreenToImage(int frame);
    ////////////////////////////////////////////////////////////////////////////////
    // => QWidget interface
public:
    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;

    // => mouse and key handling
protected:
    virtual void mousePressEvent(QMouseEvent* ev) override;
    virtual void mouseReleaseEvent(QMouseEvent* ev) override;
    virtual void mouseMoveEvent(QMouseEvent* ev) override;
    virtual void wheelEvent(QWheelEvent* ev) override;
    virtual void showEvent(QShowEvent* ev) override;

public:
    virtual void keyPressEvent(QKeyEvent* ev) override;
    virtual void keyReleaseEvent(QKeyEvent*) override;

    ////////////////////////////////////////////////////////////////////////////////
    // => QOpenGLWidget interface
private:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    // => protected members of class OpenGLWidget
protected:
    virtual void initOpenGL()                     = 0;
    virtual void resizeOpenGLWindow(int w, int h) = 0;
    virtual void renderOpenGL()                   = 0;

    void uploadCameraData();
    void resetClearColor();
    void checkGLErrors();
    void checkGLVersion();
    void checkGLExtensions(QVector<QString> extensions);


    ////////////////////////////////////////////////////////////////////////////////
    bool        m_bPrintDebug;
    int         m_WidgetUpdateTimeout;
    QSize       m_DefaultSize;
    glm::vec4   m_ClearColor;
    SpecialKey  m_SpecialKeyPressed;
    MouseButton m_MouseButtonPressed;
    QString     m_CapturePath = QString("");

    std::unique_ptr<QTimer>       m_UpdateTimer    = nullptr;
    std::unique_ptr<QImage>       m_CaptureImage   = nullptr;
    std::shared_ptr<OpenGLBuffer> m_UBufferCamData = nullptr;
    std::shared_ptr<Camera>       m_Camera         = std::make_shared<Camera>();

signals:
    void emitDebugString(QString str);
    void cameraPositionChanged(const glm::vec3 cameraPosition);

public slots:
    void printDebug(QString str)
    {
        if(m_bPrintDebug)
        {
            qDebug() << str;
        }
    }

    void setCapturePath(QString path);
};