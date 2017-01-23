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
#include <QSurfaceFormat>

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
    OpenGLWidget(QWidget* parent) :
        QOpenGLWidget(parent),
        m_bPrintDebug(true),
        m_WidgetUpdateTimeout(0),
        m_DefaultSize(QSize(1024, 1024)),
        m_ClearColor(QVector4D(0.38, 0.52, 0.10, 1.0)),
        m_SpecialKeyPressed(SpecialKey::NoKey)
    {
        connect(this, SIGNAL(emitDebugString(QString)), this, SLOT(printDebug(QString)));

        ////////////////////////////////////////////////////////////////////////////////
        QTimer* timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(update()));
        timer->start(m_WidgetUpdateTimeout);

        setFocusPolicy(Qt::StrongFocus);
    }
    ~OpenGLWidget()
    {
        shutDownAntTweakBar();
    }

    AvgTimer    m_FPSTimer;

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setVsync(bool vsync)
    {
        QSurfaceFormat format = context()->format();
        format.setSwapInterval(vsync ? 1 : 0);

        QSurfaceFormat::setDefaultFormat(format);
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setPrintDebug(bool pdebug)
    {
        m_bPrintDebug = pdebug;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setUpdateTimeout(int timeout)
    {
        m_WidgetUpdateTimeout = timeout;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setDefaultSize(QSize size)
    {
        m_DefaultSize = size;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setClearColor(QVector4D color)
    {
        m_ClearColor = color;
    }
    void setViewFrustum(float fov, float nearZ, float farZ)
    {
        m_Camera.setFrustum(fov, nearZ, farZ);
    }

    // => QWidget interface
public:
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual QSize sizeHint() const override
    {
        return m_DefaultSize;
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
            m_MouseButtonPressed = MouseButton::LeftButton;
        }
        else if(event->button() == Qt::RightButton)
        {
            m_MouseButtonPressed = MouseButton::RightButton;
        }
        else
        {
            m_MouseButtonPressed = MouseButton::NoButton;
        }

        m_Camera.set_last_mouse_pos(event->x(), event->y());
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void mouseReleaseEvent(QMouseEvent* event) override
    {
        if(TwMouseReleaseQt(this, event))
        {
            return;
        }

        m_MouseButtonPressed = MouseButton::NoButton;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void mouseMoveEvent(QMouseEvent* event) override
    {
        if(TwMouseMotionQt(this, event))
        {
            return;
        }

        if(m_MouseButtonPressed == MouseButton::LeftButton)
        {
            m_Camera.rotate_by_mouse(event->x(), event->y());
        }
        else if(m_MouseButtonPressed == MouseButton::RightButton)
        {
            if(m_SpecialKeyPressed == SpecialKey::NoKey)
            {
                m_Camera.translate_by_mouse(event->x(), event->y());
            }
            else
            {
                m_Camera.zoom_by_mouse(event->x(), event->y());
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

        float zoomFactor = (event->angleDelta().x() + event->angleDelta().y()) / 2000.0f;

        m_Camera.zoom(zoomFactor);
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
            m_SpecialKeyPressed = SpecialKey::ShiftKey;
            break;

        case Qt::Key_Plus:
            m_Camera.zoom(-0.1f);
            break;

        case Qt::Key_Minus:
            m_Camera.zoom(0.1f);
            break;

        case Qt::Key_Up:
            m_Camera.translate(glm::vec2(0, 0.5));
            break;

        case Qt::Key_Down:
            m_Camera.translate(glm::vec2(0, -0.5));
            break;

        case Qt::Key_Left:
            m_Camera.translate(glm::vec2(-0.5, 0));
            break;

        case Qt::Key_Right:
            m_Camera.translate(glm::vec2(0.5, 0));
            break;

        case Qt::Key_R:
            m_Camera.reset();
            break;


        default:
            break;
        }

    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void keyReleaseEvent(QKeyEvent*) override
    {
        if(m_SpecialKeyPressed != SpecialKey::NoKey)
        {
            m_SpecialKeyPressed = SpecialKey::NoKey;
        }
    }

    // => QOpenGLWidget interface
public:
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

        glClearColor(m_ClearColor.x(), m_ClearColor.y(), m_ClearColor.z(),
                     m_ClearColor.w());
    }
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void resizeGL(int w, int h) override
    {
        m_Camera.resizeWindow((float)w, (float)h);

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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
        m_FPSTimer.tick();
    }

    void endFrameTimer()
    {
        assert(isValid());
        m_FPSTimer.tock();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    bool        m_bPrintDebug;
    int         m_WidgetUpdateTimeout;
    QSize       m_DefaultSize;
    QVector4D   m_ClearColor;
    Camera      m_Camera;
    SpecialKey  m_SpecialKeyPressed;
    MouseButton m_MouseButtonPressed;

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