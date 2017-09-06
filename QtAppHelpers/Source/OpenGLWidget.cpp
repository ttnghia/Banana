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
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLWidget::OpenGLWidget(QWidget* parent) :
    QOpenGLWidget(parent),
    m_bPrintDebug(true),
    m_WidgetUpdateTimeout(0),
    m_DefaultSize(QSize(1920, 1080)),
    m_ClearColor(glm::vec4(0.38f, 0.52f, 0.10f, 1.0f)),
    m_SpecialKeyPressed(SpecialKey::NoKey)
{
    connect(this, SIGNAL(emitDebugString(QString)), this, SLOT(printDebug(QString)));

    ////////////////////////////////////////////////////////////////////////////////
    m_UpdateTimer = std::make_unique<QTimer>(this);
    connect(m_UpdateTimer.get(), SIGNAL(timeout()), this, SLOT(update()));
    m_UpdateTimer->start(m_WidgetUpdateTimeout);

    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLWidget::~OpenGLWidget()
{
#ifdef __BNN_USE_ANT_TWEAK_BAR
    shutDownAntTweakBar();
#endif
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
void OpenGLWidget::setClearColor(const Vec3f& color)
{
    m_ClearColor = color;
    if(isValid())
    {
        makeCurrent();
        resetClearColor();
        doneCurrent();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setViewFrustum(float fov, float nearZ, float farZ)
{
    m_Camera->setFrustum(fov, nearZ, farZ);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool OpenGLWidget::exportScreenToImage(int frame)
{
    if(m_CapturePath.isEmpty())
        return false;

    makeCurrent();
    glCall(glReadPixels(0, 0, width(), height(), GL_RGB, GL_UNSIGNED_BYTE, m_CaptureImage->bits()));
    doneCurrent();

    m_CaptureImage->mirrored().save(QString(m_CapturePath + "/frame.%1.jpg").arg(frame, 4, 10, QChar('0')));

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setCapturePath(QString path)
{
    m_CapturePath = path;
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
void OpenGLWidget::mousePressEvent(QMouseEvent* ev)
{
#ifdef __BNN_USE_ANT_TWEAK_BAR
    if(TwMousePressQt(this, ev))
    {
        return;
    }
#endif

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
void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
#ifdef __BNN_USE_ANT_TWEAK_BAR
    if(TwMouseReleaseQt(this, event))
    {
        return;
    }
#else
    __BNN_UNUSED(event);
#endif

    m_MouseButtonPressed = MouseButton::NoButton;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::mouseMoveEvent(QMouseEvent* ev)
{
#ifdef __BNN_USE_ANT_TWEAK_BAR
    if(TwMouseMotionQt(this, ev))
    {
        return;
    }
#endif

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
void OpenGLWidget::wheelEvent(QWheelEvent* ev)
{
    if(ev->angleDelta().isNull())
    {
        return;
    }

    float zoomFactor = (ev->angleDelta().x() + ev->angleDelta().y()) / 5000.0f;

    m_Camera->zoom(zoomFactor);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::showEvent(QShowEvent*)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::keyPressEvent(QKeyEvent* ev)
{
#ifdef __BNN_USE_ANT_TWEAK_BAR
    if(TwKeyPressQt(ev))
    {
        return;
    }
#endif

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
            m_Camera->translate(glm::vec2(0, 0.1));
            break;

        case Qt::Key_Down:
            m_Camera->translate(glm::vec2(0, -0.1));
            break;

        case Qt::Key_Left:
            m_Camera->translate(glm::vec2(-0.1, 0));
            break;

        case Qt::Key_Right:
            m_Camera->translate(glm::vec2(0.1, 0));
            break;

        case Qt::Key_R:
            m_Camera->reset();
            break;


        default:
            break;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::keyReleaseEvent(QKeyEvent*)
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
    //checkGLVersion();
    checkGLErrors();
#ifdef __BNN_USE_ANT_TWEAK_BAR
    initializeAntTweakBar();
    setupTweakBar();
#endif

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    resetClearColor();

    ////////////////////////////////////////////////////////////////////////////////
    m_CaptureImage = std::make_unique<QImage>(width(), height(), QImage::Format_RGB888);

    ////////////////////////////////////////////////////////////////////////////////
    // view matrix, prj matrix, inverse view matrix, inverse proj matrix, shadow matrix, cam position
    m_UBufferCamData = std::make_shared<OpenGLBuffer>();
    m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER, 5 * sizeof(glm::mat4) + sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);

    emit cameraPositionChanged(m_Camera->getCameraPosition());

    ////////////////////////////////////////////////////////////////////////////////
    // call init function from derived class
    initOpenGL();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void OpenGLWidget::resizeGL(int w, int h)
{
    glCall(glViewport(0, 0, w, h));
    m_Camera->resizeWindow((float)w, (float)h);
#ifdef __BNN_USE_ANT_TWEAK_BAR
    resizeAntTweakBarWindow(w, h);
#endif
    m_CaptureImage.reset(new QImage(w, h, QImage::Format_RGB888));

    ////////////////////////////////////////////////////////////////////////////////
    // call init function from derived class
    resizeOpenGLWindow(w, h);
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

    ////////////////////////////////////////////////////////////////////////////////
    // call init function from derived class
    renderOpenGL();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::uploadCameraData()
{
    m_Camera->updateCameraMatrices();
    if(m_Camera->isCameraChanged())
    {
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()),              0,                     sizeof(glm::mat4));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()),        sizeof(glm::mat4),     sizeof(glm::mat4));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getInverseViewMatrix()),       2 * sizeof(glm::mat4), sizeof(glm::mat4));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getInverseProjectionMatrix()), 3 * sizeof(glm::mat4), sizeof(glm::mat4));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getCameraPosition()),          5 * sizeof(glm::mat4), sizeof(glm::vec3));

        emit cameraPositionChanged(m_Camera->getCameraPosition());
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::resetClearColor()
{
    glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], 1.0f);
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
    emit    emitDebugString(QString("GLVersion: ") + verStr);

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
    emit    emitDebugString(extStr);

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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana