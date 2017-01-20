#ifndef __OPENGL_WIDGET_H__
#define __OPENGL_WIDGET_H__

#include <QtGui>
#include <QtWidgets>

#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/AvgTimer.h>
#include <QtAppHelpers/AntTweakBarWrapper.h>

#include <OpenGLHelpers/Camera.h>

//#include <glm/glm.hpp>
//#include <glm/gtx/string_cast.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OpenGLWidget : public QOpenGLWidget, public OpenGLFunctions,
    public AntTweakBarWrapper
{
    Q_OBJECT
public:
    explicit OpenGLWidget(QWidget* parent = 0);
    ~OpenGLWidget()
    {
        shutDownAntTweakBar();
    }

    AvgTimer FPSTimer;

signals:
    void emitDebugString(QString str);
    void frameRateChanged(float fps, float avgFrameTime);

    public slots:
    void printDebug(QString str);

    // => public accessing members
public:
    void setPrintDebug(bool pdebug);
    void setUpdateTimeout(int timeout_in_ms);
    void setDefaultSize(QSize size);
    void setClearColor(QVector4D color);
    void setViewFrustum(float fov, float near, float far);

    // => QWidget interface
public:
    virtual QSize sizeHint() const override;

    // => mouse and key handling
protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

public:
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;

    // => QOpenGLWidget interface
protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    // => protected members of class OpenGLWidget
protected:
    void checkGLErrors();
    void checkGLVersion();
    void checkGLExtensions(QVector<QString> extensions);
    void startFrameTimer();
    void endFrameTimer();

    bool print_debug;
    int update_timeout;
    QSize defaultSize;
    QVector4D clearColor;

    Camera camera;
    SpecialKey specialKeyPressed;
    MouseButton mouseButtonPressed;

    //    std::chrono::high_resolution_clock::time_point timeStartTimer;
    //    std::chrono::high_resolution_clock::time_point timeStartFrame;
    //    std::chrono::high_resolution_clock::time_point timeEndFrame;
    //    double renderTime;
    //    int renderFrameCount;
    //    bool timerStarted;

};

#endif // __OPENGL_WIDGET_H__