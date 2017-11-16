//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
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

#include <Banana/Setup.h>
#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/AvgTimer.h>
#include <QtAppHelpers/FPSCounter.h>

#include <OpenGLHelpers/Camera.h>
#include <OpenGLHelpers/OpenGLBuffer.h>

#include <QtGui>
#include <QtWidgets>
#include <QSurfaceFormat>

#include <memory>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OpenGLWidget : public QOpenGLWidget, public OpenGLFunctions
{
    Q_OBJECT

    friend class OpenGLMainWindow;
public:
    OpenGLWidget(QWidget* parent);
    ~OpenGLWidget() = default;

    void setPrintDebug(bool pdebug) { m_bPrintDebug = pdebug; }
    void setUpdateTimeout(int timeout) { m_WidgetUpdateTimeout = timeout; }
    void setDefaultSize(QSize size) { m_DefaultSize = size; }
    void setClearColor(const Vec3f& color);
    void resetClearColor() { glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], 1.0f); }
    void setViewFrustum(float fov, float nearZ, float farZ) { m_Camera->setFrustum(fov, nearZ, farZ); }

    bool         exportScreenToImage(int frame);
    const Vec3f& getClearColor() const { return m_ClearColor; }
    ////////////////////////////////////////////////////////////////////////////////
    // => QWidget interface
public:
    virtual QSize sizeHint() const override { return m_DefaultSize; }
    virtual QSize minimumSizeHint() const override { return QSize(10, 10); }

    // => mouse and key handling
protected:
    virtual void mousePressEvent(QMouseEvent* ev) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override { m_MouseButtonPressed = MouseButton::NoButton; }
    virtual void mouseMoveEvent(QMouseEvent* ev) override;
    virtual void wheelEvent(QWheelEvent* ev) override;
    virtual void showEvent(QShowEvent*) override { setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); }

public:
    virtual void keyPressEvent(QKeyEvent* ev) override;
    virtual void keyReleaseEvent(QKeyEvent*) override { m_SpecialKeyPressed = SpecialKey::NoKey; }

    ////////////////////////////////////////////////////////////////////////////////
    // => QOpenGLWidget interface
private:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    // => protected members of class OpenGLWidget
protected:
    virtual void initOpenGL() = 0;
    virtual void resizeOpenGLWindow(int w, int h) = 0;
    virtual void renderOpenGL() = 0;

    void uploadCameraData();
    void checkGLErrors();
    void checkGLVersion();
    void checkGLExtensions(QVector<QString> extensions);

    ////////////////////////////////////////////////////////////////////////////////
    bool        m_bPrintDebug;
    int         m_WidgetUpdateTimeout;
    QSize       m_DefaultSize;
    Vec3f       m_ClearColor;
    SpecialKey  m_SpecialKeyPressed;
    MouseButton m_MouseButtonPressed;
    QString     m_CapturePath = QString("");

    FPSCounter m_FPSCounter;

    UniquePtr<QTimer>       m_UpdateTimer    = nullptr;
    UniquePtr<QImage>       m_CaptureImage   = nullptr;
    SharedPtr<OpenGLBuffer> m_UBufferCamData = nullptr;
    SharedPtr<Camera>       m_Camera         = std::make_shared<Camera>();

signals:
    void emitDebugString(QString str);
    void cameraPositionInfoChanged(const Vec3f& cameraPosition, const Vec3f& cameraFocus);

public slots:
    void printDebug(QString str) { if(m_bPrintDebug) { qDebug() << str; } }
    void setCapturePath(QString path) { m_CapturePath = path; }
    void resetCameraPosition() { m_Camera->reset(); }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
