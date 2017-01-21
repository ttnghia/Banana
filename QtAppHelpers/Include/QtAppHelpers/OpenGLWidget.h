//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <QtGui>
#include <QtWidgets>

#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/AvgTimer.h>
#include <QtAppHelpers/AntTweakBarWrapper.h>

#include <OpenGLHelpers/Camera.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OpenGLWidget : public QOpenGLWidget, public OpenGLFunctions,
    public AntTweakBarWrapper
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget * parent) :
        QOpenGLWidget(parent),
        print_debug(true),
        update_timeout(0),
        defaultSize(QSize(1500, 1500)),
        clearColor(QVector4D(0.2, 0.2, 0.2, 1.0)),
        specialKeyPressed(SpecialKey::NoKey)
    {
        connect(this, SIGNAL(emitDebugString(QString)), this, SLOT(printDebug(QString)));

        ////////////////////////////////////////////////////////////////////////////////
        QTimer* timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(update()));
        timer->start(update_timeout);

        setFocusPolicy(Qt::StrongFocus);
    }
    ~OpenGLWidget()
    {
        shutDownAntTweakBar();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    AvgTimer FPSTimer;

signals:
    void emitDebugString(QString str);
    void frameRateChanged(float fps, float avgFrameTime);

    public slots:
    void printDebug(QString str);

    // => public accessing members
public:
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void printDebug(QString str)
    {
        if(print_debug)
        {
            qDebug() << str.toStdString().c_str();
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setPrintDebug(bool pdebug)
    {
        print_debug = pdebug;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setUpdateTimeout(int timeout_in_ms)
    {
        update_timeout = timeout_in_ms;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setDefaultSize(QSize size)
    {
        defaultSize = size;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setClearColor(QVector4D color)
    {
        clearColor = color;
    }
    void setViewFrustum(float fov, float near, float far);

    // => QWidget interface
public:
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual QSize sizeHint() const override
    {
        return defaultSize;
    }

    // => mouse and key handling
protected:
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void mousePressEvent(QMouseEvent* event) override
    {
        if(TwMousePressQt(this, event))
        {
            return;
        }

        if(event->button() == Qt::LeftButton)
        {
            mouseButtonPressed = MouseButton::LeftButton;
        }
        else if(event->button() == Qt::RightButton)
        {
            mouseButtonPressed = MouseButton::RightButton;
        }
        else
        {
            mouseButtonPressed = MouseButton::NoButton;
        }

        camera.set_last_mouse_pos(event->x(), event->y());


    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void mouseReleaseEvent(QMouseEvent* event) override
    {
        if(TwMouseReleaseQt(this, event))
        {
            return;
        }

        mouseButtonPressed = MouseButton::NoButton;

    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void mouseMoveEvent(QMouseEvent* event) override
    {
        if(TwMouseMotionQt(this, event))
        {
            return;
        }

        if(mouseButtonPressed == MouseButton::LeftButton)
        {
            camera.rotate_by_mouse(event->x(), event->y());
        }
        else if(mouseButtonPressed == MouseButton::RightButton)
        {
            if(specialKeyPressed == SpecialKey::NoKey)
            {
                camera.translate_by_mouse(event->x(), event->y());
            }
            else
            {
                camera.zoom_by_mouse(event->x(), event->y());
            }
        }

    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void wheelEvent(QWheelEvent* event) override
    {
        if(event->angleDelta().isNull())
        {
            return;
        }

        float zooming_factor = (event->angleDelta().x() + event->angleDelta().y()) / 2000.0f;

        camera.zoom(zooming_factor);
    }

public:
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void keyPressEvent(QKeyEvent* event) override
    {
        if(TwKeyPressQt(event))
        {
            return;
        }

        switch(event->key())
        {
            case Qt::Key_Shift:
                specialKeyPressed = SpecialKey::ShiftKey;
                break;

            case Qt::Key_Plus:
                camera.zoom(-0.1f);
                break;

            case Qt::Key_Minus:
                camera.zoom(0.1f);
                break;

            case Qt::Key_Up:
                camera.translate(glm::vec2(0, 0.5));
                break;

            case Qt::Key_Down:
                camera.translate(glm::vec2(0, -0.5));
                break;

            case Qt::Key_Left:
                camera.translate(glm::vec2(-0.5, 0));
                break;

            case Qt::Key_Right:
                camera.translate(glm::vec2(0.5, 0));
                break;

            case Qt::Key_R:
                camera.reset();
                break;


            default:
                break;
        }

    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void keyReleaseEvent(QKeyEvent* event) override
    {
        (void)event;

        if(specialKeyPressed != SpecialKey::NoKey)
        {
            specialKeyPressed = SpecialKey::NoKey;
        }
    }

    // => QOpenGLWidget interface
protected:
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void initializeGL() override
    {
        initializeOpenGLFunctions();
        checkGLErrors();
        // checkGLVersion();

        initializeAntTweakBar();
        setupTweakBar();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);

        glClearColor(clearColor.x(), clearColor.y(), clearColor.z(),
                     clearColor.w());
    }
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void resizeGL(int w, int h) override
    {
        camera.resizeWindow((float)w, (float)h);

        resizeAntTweakBarWindow(w, h);
    }
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void paintGL() override
    {
#if 0
        glClearColor(rand() / (float)RAND_MAX,
                     rand() / (float)RAND_MAX,
                     rand() / (float)RAND_MAX,
                     1);
#endif

        //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // => protected members of class OpenGLWidget
protected:
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void checkGLErrors()
    {
        QString glErrStr = "OpenGL initialized: ";

        switch(glGetError())
        {
            case GL_NO_ERROR:
                glErrStr += "No error.";
                break;

            case GL_INVALID_ENUM:
                glErrStr += "Invalid enum.";
                break;

            case GL_INVALID_VALUE:
                glErrStr += "Invalid value.";
                break;

            case GL_INVALID_OPERATION:
                glErrStr += "Invalid operation.";
                break;

            case GL_STACK_OVERFLOW:
                glErrStr += "Stack overflow.";
                break;

            case GL_STACK_UNDERFLOW:
                glErrStr += "Stack underflow.";
                break;

            case GL_OUT_OF_MEMORY:
                glErrStr += "Out of memory.";
                break;

            default:
                glErrStr += "Unknown error.";
                break;
        }

        emit emitDebugString(glErrStr);
    }
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void checkGLVersion()
    {
        QString verStr = QString((const char*)glGetString(GL_VERSION));
        emit emitDebugString(QString("GLVersion: ") + verStr);

        int major = verStr.left(verStr.indexOf(".")).toInt();
        int minor = verStr.mid(verStr.indexOf(".") + 1, 1).toInt();

        if(!(major >= 4 && minor >= 1))
        {
            QMessageBox msgBox(QMessageBox::Critical, "Error",
                               QString("Your OpenGL version is %1.%2 (Required: OpenGL >= 4.1).")
                               .arg(major).arg(minor));
            msgBox.exec();
            exit(EXIT_FAILURE);
        }

    }
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void checkGLExtensions(QVector<QString> extensions)
    {
        QString extStr = QString((const char*)glGetString(GL_EXTENSIONS));
        emit emitDebugString(extStr);

        bool check = true;

        for(QString ext : extensions)
        {
            if(!extStr.contains(ext))
            {
                QMessageBox msgBox(QMessageBox::Critical, "Error",
                                   QString("Extension %1 is not supported.")
                                   .arg(ext));
                msgBox.exec();
                check = false;
            }
        }

        if(!check)
        {
            exit(EXIT_FAILURE);
        }
    }
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void startFrameTimer()
    {
        assert(isValid());
        glFinish(); // // block until all opengl operations finished

                    ////////////////////////////////////////////////////////////////////////////////
                    //    timeStartFrame = std::chrono::high_resolution_clock::now();
                    //    timerStarted = true;

                    //    if(renderFrameCount == 0)
                    //    {
                    //        timeStartTimer = std::chrono::high_resolution_clock::now();
                    //    }

        FPSTimer.tick();
    }
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void endFrameTimer()
    {
        assert(isValid());
        glFinish(); // // block until all opengl operations finished

                    ////////////////////////////////////////////////////////////////////////////////
                    //    timeEndFrame = std::chrono::high_resolution_clock::now();

                    //    std::chrono::duration<double, std::milli> frameDuration =
                    //        std::chrono::duration_cast<std::chrono::duration<double, std::milli> >
                    //        (timeEndFrame - timeStartFrame);
                    //    std::chrono::duration<double, std::milli> timerDuration =
                    //        std::chrono::duration_cast<std::chrono::duration<double, std::milli> >
                    //        (timeEndFrame - timeStartTimer);
                    //    renderTime += frameDuration.count();
                    //    timerStarted = false;
                    //    ++renderFrameCount;

                    //    if(timerDuration.count() >= 2000)
                    //    {
                    //        renderTime /= (double)renderFrameCount;

                    ////        qDebug() << "FPS: " << 1000.0 / renderTime << ", t = " << renderTime;
                    //        emit frameRateChanged(1000.0 / renderTime, renderTime);

                    //        renderFrameCount = 0;
                    //        renderTime = 0;

                    //    }

        FPSTimer.tock();
    }

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
