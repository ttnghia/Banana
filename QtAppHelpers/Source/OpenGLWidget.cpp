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

#include <QtAppHelpers/OpenGLWidget.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLWidget::OpenGLWidget(QWidget* parent) :
    QOpenGLWidget(parent),
    m_bPrintDebug(true),
    m_WidgetUpdateTimeout(0),
    m_DefaultSize(QSize(1920, 1080)),
    m_ClearColor(glm::vec4(0.38f, 0.52f, 0.10f, 1.0f)),
    m_SpecialKeyPressed(SpecialKey::NoKey),
    m_CaptureImage(nullptr),
    m_UBufferCamData(nullptr),
    m_Camera(new Camera)
{
    connect(this, SIGNAL(emitDebugString(QString)), this, SLOT(printDebug(QString)));

    ////////////////////////////////////////////////////////////////////////////////
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(m_WidgetUpdateTimeout);

    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLWidget::~OpenGLWidget()
{
    shutDownAntTweakBar();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setPrintDebug(bool pdebug)
{
    m_bPrintDebug = pdebug;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setUpdateTimeout(int timeout)
{
    m_WidgetUpdateTimeout = timeout;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setDefaultSize(QSize size)
{
    m_DefaultSize = size;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setClearColor(const glm::vec4& color)
{
    m_ClearColor = color;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setViewFrustum(float fov, float nearZ, float farZ)
{
    m_Camera->setFrustum(fov, nearZ, farZ);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::exportScreenToImage(int frame)
{
    glCall(glReadPixels(0, 0, width(), height(), GL_RGBA, GL_UNSIGNED_BYTE,
           m_CaptureImage->bits()));
    m_CaptureImage->mirrored().save(QString(m_CapturePath + "/frame.%1.png").arg(frame));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QSize OpenGLWidget::sizeHint() const
{
    return m_DefaultSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QSize OpenGLWidget::minimumSizeHint() const
{
    return QSize(10, 10);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::mousePressEvent(QMouseEvent * ev)
{
    if(TwMousePressQt(this, ev))
    {
        return;
    }

    if(ev->button() == Qt::LeftButton)
    {
        m_MouseButtonPressed = MouseButton::LeftButton;
    }
    else if(ev->button() == Qt::RightButton)
    {
        m_MouseButtonPressed = MouseButton::RightButton;
    }
    else
    {
        m_MouseButtonPressed = MouseButton::NoButton;
    }

    m_Camera->set_last_mouse_pos(ev->x(), ev->y());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::mouseReleaseEvent(QMouseEvent * event)
{
    if(TwMouseReleaseQt(this, event))
    {
        return;
    }

    m_MouseButtonPressed = MouseButton::NoButton;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::mouseMoveEvent(QMouseEvent * ev)
{
    if(TwMouseMotionQt(this, ev))
    {
        return;
    }

    if(m_MouseButtonPressed == MouseButton::LeftButton)
    {
        m_Camera->rotate_by_mouse(ev->x(), ev->y());
    }
    else if(m_MouseButtonPressed == MouseButton::RightButton)
    {
        if(m_SpecialKeyPressed == SpecialKey::NoKey)
        {
            m_Camera->translate_by_mouse(ev->x(), ev->y());
        }
        else
        {
            m_Camera->zoom_by_mouse(ev->x(), ev->y());
        }
    }

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::wheelEvent(QWheelEvent * ev)
{
    if(ev->angleDelta().isNull())
    {
        return;
    }

    float zoomFactor = (ev->angleDelta().x() + ev->angleDelta().y()) / 2000.0f;

    m_Camera->zoom(zoomFactor);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::showEvent(QShowEvent* ev)
{
    (void*)ev;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::keyPressEvent(QKeyEvent * ev)
{
    if(TwKeyPressQt(ev))
    {
        return;
    }

    switch(ev->key())
    {
        case Qt::Key_Shift:
            m_SpecialKeyPressed = SpecialKey::ShiftKey;
            break;

        case Qt::Key_Plus:
            m_Camera->zoom(-0.01f);
            break;

        case Qt::Key_Minus:
            m_Camera->zoom(0.01f);
            break;

        case Qt::Key_Up:
            m_Camera->translate(glm::vec2(0, 0.5));
            break;

        case Qt::Key_Down:
            m_Camera->translate(glm::vec2(0, -0.5));
            break;

        case Qt::Key_Left:
            m_Camera->translate(glm::vec2(-0.5, 0));
            break;

        case Qt::Key_Right:
            m_Camera->translate(glm::vec2(0.5, 0));
            break;

        case Qt::Key_R:
            m_Camera->reset();
            break;


        default:
            break;
    }

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::keyReleaseEvent(QKeyEvent *)
{
    if(m_SpecialKeyPressed != SpecialKey::NoKey)
    {
        m_SpecialKeyPressed = SpecialKey::NoKey;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    checkGLErrors();
    // checkGLVersion();

    initializeAntTweakBar();
    setupTweakBar();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    resetClearColor();

    ////////////////////////////////////////////////////////////////////////////////
    m_CaptureImage = new QImage(width(), height(), QImage::Format_RGBA8888);

    ////////////////////////////////////////////////////////////////////////////////
    // view matrix, prj matrix, inverse view matrix, inverse proj matrix, shadow matrix, cam position
    m_UBufferCamData = new OpenGLBuffer;
    m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER, 5 * sizeof(glm::mat4) + sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void OpenGLWidget::resizeGL(int w, int h)
{
    glCall(glViewport(0, 0, w, h));
    m_Camera->resizeWindow((float)w, (float)h);

    resizeAntTweakBarWindow(w, h);

    delete m_CaptureImage;
    m_CaptureImage = new QImage(w, h, QImage::Format_RGBA8888);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::paintGL()
{
#if 0
    glClearColor(rand() / (float)RAND_MAX,
                 rand() / (float)RAND_MAX,
                 rand() / (float)RAND_MAX,
                 1);
#endif

    m_FPSCounter.countFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    uploadCameraData();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::uploadCameraData()
{
    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()), 0, sizeof(glm::mat4));
    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()), sizeof(glm::mat4), sizeof(glm::mat4));
    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getInverseViewMatrix()), 2 * sizeof(glm::mat4), sizeof(glm::mat4));
    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getInverseProjectionMatrix()), 3 * sizeof(glm::mat4), sizeof(glm::mat4));
    // todo: shadow matrix
    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->m_CameraPosition), 5 * sizeof(glm::mat4), sizeof(glm::vec3));

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::resetClearColor()
{
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::checkGLErrors()
{
    GLenum glStatus = glGetError();
    if(glStatus == GL_NO_ERROR)
        return;

    QString glErrStr = "OpenGL error: ";

    switch(glStatus)
    {
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::checkGLVersion()
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::checkGLExtensions(QVector<QString> extensions)
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
