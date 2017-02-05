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

#include <QtGui>
#include <QtWidgets>
#include <QSurfaceFormat>

#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/AvgTimer.h>
#include <QtAppHelpers/FPSCounter.h>
#include <QtAppHelpers/AntTweakBarWrapper.h>

#include <OpenGLHelpers/Camera.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OpenGLWidget : public QOpenGLWidget, public OpenGLFunctions,
    public AntTweakBarWrapper
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget* parent);
    ~OpenGLWidget();

    AvgTimer   m_AvgFrameTimer;
    FPSCounter m_FPSCounter;

    void setPrintDebug(bool pdebug);
    void setUpdateTimeout(int timeout);
    void setDefaultSize(QSize size);
    void setClearColor(QVector4D color);
    void setViewFrustum(float fov, float nearZ, float farZ);

    ////////////////////////////////////////////////////////////////////////////////
    // => QWidget interface
public:
    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;

    // => mouse and key handling
protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

public:
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent*) override;

    ////////////////////////////////////////////////////////////////////////////////
    // => QOpenGLWidget interface
protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    // => protected members of class OpenGLWidget
protected:
    void resetClearColor();
    void checkGLErrors();
    void checkGLVersion();
    void checkGLExtensions(QVector<QString> extensions);

    void startFrameTimer();
    void endFrameTimer();
    void exportScreenToImage(int frame);

    ////////////////////////////////////////////////////////////////////////////////
    bool        m_bPrintDebug;
    int         m_WidgetUpdateTimeout;
    QSize       m_DefaultSize;
    QVector4D   m_ClearColor;
    SpecialKey  m_SpecialKeyPressed;
    MouseButton m_MouseButtonPressed;
    QString     m_CapturePath;
    QImage*     m_CaptureImage;
    Camera*     m_Camera;

signals:
    void emitDebugString(QString str);

    public slots:
    void printDebug(QString str)
    {
        if(m_bPrintDebug)
        {
            qDebug() << str;
        }
    }

};