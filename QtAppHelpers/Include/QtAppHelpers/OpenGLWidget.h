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
#include <QtAppHelpers/QtAppUtils.h>

#include <OpenGLHelpers/Camera.h>
#include <OpenGLHelpers/Lights.h>
#include <OpenGLHelpers/OpenGLBuffer.h>
#include <OpenGLHelpers/RenderObjects.h>

#include <QtGui>
#include <QtWidgets>
#include <QSurfaceFormat>
#include <QDir>

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

    const auto& getClearColor() const { return m_ClearColor; }
    auto& getCamera() const { return m_Camera; }

    ////////////////////////////////////////////////////////////////////////////////
    // => QWidget interface
public:
    virtual QSize sizeHint() const override { return m_DefaultSize; }
    virtual QSize minimumSizeHint() const override { return QSize(10, 10); }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
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

    ////////////////////////////////////////////////////////////////////////////////
    // => QOpenGLWidget interface
protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
    ////////////////////////////////////////////////////////////////////////////////

    // => protected members of class OpenGLWidget
protected:
    virtual void initOpenGL()                     = 0;
    virtual void resizeOpenGLWindow(int w, int h) = 0;
    virtual void renderOpenGL()                   = 0;

    void uploadCameraData();
    void checkGLErrors();
    void checkGLVersion();
    void checkGLExtensions(const QVector<QString>& extensions);

    ////////////////////////////////////////////////////////////////////////////////
#ifdef __BANANA_DEBUG__
    bool m_bPrintDebug = true;
#else
    bool m_bPrintDebug = false;
#endif
    QSize       m_DefaultSize        = QtAppUtils::getDefaultWindowSize();
    Vec3f       m_ClearColor         = QtAppUtils::getDefaultClearColor();
    SpecialKey  m_SpecialKeyPressed  = SpecialKey::NoKey;
    MouseButton m_MouseButtonPressed = MouseButton::NoButton;
    QString     m_CapturePath        = QtAppUtils::getDefaultCapturePath();

    FPSCounter m_FPSCounter;

    UniquePtr<QTimer>       m_UpdateTimer    = nullptr;
    SharedPtr<OpenGLBuffer> m_UBufferCamData = nullptr;
    SharedPtr<Camera>       m_Camera         = std::make_shared<Camera>();

signals:
    void emitDebugString(const QString& str);
    void cameraPositionInfoChanged(const Vec3f& cameraPosition, const Vec3f& cameraFocus);

public slots:
    void printDebugString(const QString& str) { if(m_bPrintDebug) { qDebug() << str; } }
    void setCapturePath(const QString& path);
    void setPrintDebug(bool pdebug) { m_bPrintDebug = pdebug; }
    void setDefaultSize(QSize size) { m_DefaultSize = size; }
    void setClearColor(const Vec3f& color);
    void resetClearColor() { glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], 1.0f); }
    void setViewFrustum(float fov, float nearZ, float farZ) { m_Camera->setFrustum(fov, nearZ, farZ); }
    void setCamera(const Vec3f& cameraPosition, const Vec3f& cameraFocus) { m_Camera->setCamera(cameraPosition, cameraFocus, Vec3f(0, 1, 0)); }
    void setCamera(const std::pair<Vec3f, Vec3f>& cameraInfo) { m_Camera->setCamera(cameraInfo.first, cameraInfo.second, Vec3f(0, 1, 0)); }
    void resetCameraPosition() { m_Camera->reset(); }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // capture image
    void setCaptureImageExtension(const QString& ext) { m_CaptureImageExt = ext; }
    bool exportScreenToImage(int frame);
protected:
    QString           m_CaptureImageExt = QString("png");
    UniquePtr<QImage> m_CaptureImage    = nullptr;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // clip plane
public slots:
    void enableClipPlane(bool bEnable);
    void setClipPlane(const Vec4f& clipPlane) { m_ClipPlane = clipPlane; }

protected:
    Vec4f m_ClipPlane = Vec4f(1.0f, 0.0f, 0.0f, -0.5f);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // light
signals:
    void lightsObjChanged(const SharedPtr<PointLights>& lights);
public slots:
    void setLights(const Vector<PointLights::PointLightData>& lightData);
    void updateLights();
protected:
    void initRDataLight();
    void renderLight() { Q_ASSERT(m_LightRender != nullptr); m_LightRender->render(); }
    SharedPtr<PointLights>      m_Lights      = nullptr;
    UniquePtr<PointLightRender> m_LightRender = nullptr;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // sky box and checkerboard/grid background =>
public:
    enum BackgroundMode
    {
        SkyBox = 0,
        Color,
        Checkerboard,
        Grid,
        NumBackgroundMode
    };
public slots:
    void setBackgroundMode(int backgroundMode) { m_BackgroundMode = static_cast<BackgroundMode>(backgroundMode); }
    void setSkyBoxTextureIndex(int texIndex) { Q_ASSERT(m_SkyBoxRender != nullptr); m_SkyBoxRender->setRenderTextureIndex(texIndex); }
    void setCheckerboarrdColor1(const Vec3f& color) { Q_ASSERT(m_CheckerboardRender != nullptr); m_CheckerboardRender->setColor1(color); }
    void setCheckerboarrdColor2(const Vec3f& color) { Q_ASSERT(m_CheckerboardRender != nullptr); m_CheckerboardRender->setColor2(color); }
    void setCheckerboarrdScales(const Vec2i& scales) { Q_ASSERT(m_CheckerboardRender != nullptr); m_CheckerboardRender->setScales(scales); }
    void setGridBackgroundColor(const Vec3f& color) { Q_ASSERT(m_GridRender != nullptr); m_GridRender->setBackgroundColor(color); }
    void setGridLineColor(const Vec3f& color) { Q_ASSERT(m_GridRender != nullptr); m_GridRender->setLineColor(color); }
    void setGridScales(const Vec2i& scales) { Q_ASSERT(m_GridRender != nullptr); m_GridRender->setScales(scales); }
    void reloadSkyboxTextures() { m_SkyBoxRender->clearTextures(); m_SkyBoxRender->loadTextures(QtAppUtils::getTexturePath() + "/Sky/"); }
protected:
    void initRDataSkyBox();
    void initRDataCheckerboardBackground() { m_CheckerboardRender = std::make_unique<CheckerboardBackgroundRender>(); }
    void initRDataGridBackground() { m_GridRender = std::make_unique<GridBackgroundRender>(); }
    void renderSkyBox() { Q_ASSERT(m_SkyBoxRender != nullptr); m_SkyBoxRender->render(); }
    void renderCheckerboardBackground() { Q_ASSERT(m_CheckerboardRender != nullptr); m_CheckerboardRender->render(); }
    void renderGridBackground() { Q_ASSERT(m_GridRender != nullptr); m_GridRender->render(); }

    UniquePtr<SkyBoxRender>                 m_SkyBoxRender       = nullptr;
    UniquePtr<CheckerboardBackgroundRender> m_CheckerboardRender = nullptr;
    UniquePtr<GridBackgroundRender>         m_GridRender         = nullptr;
    BackgroundMode                          m_BackgroundMode     = BackgroundMode::SkyBox;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // floor =>
public slots:
    void setFloorTextureIndex(int texIndex) { Q_ASSERT(m_FloorRender != nullptr); m_FloorRender->setRenderTextureIndex(texIndex); }
    void setFloorExposure(int percentage) { m_FloorRender->setExposure(static_cast<float>(percentage) / 100.0f); }
    void setFloorSize(int size) { m_FloorRender->scale(Vec3f(static_cast<float>(size))); }
    void setFloorTexScales(int scale) { m_FloorRender->scaleTexCoord(scale, scale); }
    void setFloorHeight(float height) { m_FloorRender->translate(Vec3f(0, height, 0)); }
    void reloadFloorTextures() { m_FloorRender->clearTextures(); m_FloorRender->loadTextures(QtAppUtils::getTexturePath() + "/Floor/"); }
protected:
    void initRDataFloor();
    void renderFloor() { Q_ASSERT(m_FloorRender != nullptr); m_FloorRender->render(); }
    UniquePtr<PlaneRender> m_FloorRender = nullptr;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // box =>
public slots:
    void enableRenderBox(bool bRender) { m_bRenderBox = bRender; }
    void setBoxColor(const Vec3f& color) { Q_ASSERT(m_DomainBoxRender != nullptr); m_DomainBoxRender->setColor(color); }
    void setBox(const Vec3f& boxMin, const Vec3f& boxMax) { if(isValid()) { makeCurrent(); m_DomainBoxRender->setBox(boxMin, boxMax); doneCurrent(); } }
protected:
    void initRDataBox();
    void renderBox() { Q_ASSERT(m_DomainBoxRender != nullptr); if(m_bRenderBox) { m_DomainBoxRender->render(); } }
    UniquePtr<WireFrameBoxRender> m_DomainBoxRender = nullptr;
    bool                          m_bRenderBox      = true;
    ////////////////////////////////////////////////////////////////////////////////
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
