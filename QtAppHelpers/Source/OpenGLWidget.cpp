//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/24/2017
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

#include <QtAppHelpers/OpenGLWidget.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLWidget::OpenGLWidget(QWidget* parent) :
    QOpenGLWidget(parent),
    m_bPrintDebug(true),
    m_WidgetUpdateTimeout(0),
    m_DefaultSize(QSize(1024, 1024)),
    m_ClearColor(QVector4D(0.38f, 0.52f, 0.10f, 1.0f)),
    m_SpecialKeyPressed(SpecialKey::NoKey),
    m_CaptureImage(nullptr)
{
    connect(this, SIGNAL(emitDebugString(QString)), this, SLOT(printDebug(QString)));

    ////////////////////////////////////////////////////////////////////////////////
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(m_WidgetUpdateTimeout);

    setFocusPolicy(Qt::StrongFocus);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLWidget::~OpenGLWidget()
{
    shutDownAntTweakBar();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setPrintDebug(bool pdebug)
{
    m_bPrintDebug = pdebug;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setUpdateTimeout(int timeout)
{
    m_WidgetUpdateTimeout = timeout;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setDefaultSize(QSize size)
{
    m_DefaultSize = size;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setClearColor(QVector4D color)
{
    m_ClearColor = color;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setViewFrustum(float fov, float nearZ, float farZ)
{
    m_Camera.setFrustum(fov, nearZ, farZ);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::exportScreenToImage(int frame)
{
    glCall(glReadPixels(0, 0, width(), height(), GL_RGBA, GL_UNSIGNED_BYTE,
           m_CaptureImage->bits()));
    m_CaptureImage->mirrored().save(QString(m_CapturePath + "/frame.%1.png").arg(frame));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QSize OpenGLWidget::sizeHint() const
{
    return m_DefaultSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QSize OpenGLWidget::minimumSizeHint() const
{
    return QSize(10, 10);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::mousePressEvent(QMouseEvent * event)
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
void OpenGLWidget::mouseReleaseEvent(QMouseEvent * event)
{
    if(TwMouseReleaseQt(this, event))
    {
        return;
    }

    m_MouseButtonPressed = MouseButton::NoButton;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::mouseMoveEvent(QMouseEvent * event)
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
void OpenGLWidget::wheelEvent(QWheelEvent * event)
{
    if(event->angleDelta().isNull())
    {
        return;
    }

    float zoomFactor = (event->angleDelta().x() + event->angleDelta().y()) / 2000.0f;

    m_Camera.zoom(zoomFactor);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::keyPressEvent(QKeyEvent * event)
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
void OpenGLWidget::keyReleaseEvent(QKeyEvent *)
{
    if(m_SpecialKeyPressed != SpecialKey::NoKey)
    {
        m_SpecialKeyPressed = SpecialKey::NoKey;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::initializeGL()
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

void OpenGLWidget::resizeGL(int w, int h)
{
    m_Camera.resizeWindow((float)w, (float)h);

    resizeAntTweakBarWindow(w, h);

    delete m_CaptureImage;
    m_CaptureImage = new QImage(w, h, QImage::Format_RGBA8888);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::paintGL()
{
#if 0
    glClearColor(rand() / (float)RAND_MAX,
                 rand() / (float)RAND_MAX,
                 rand() / (float)RAND_MAX,
                 1);
#endif

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::startFrameTimer()
{
    assert(isValid());
    m_AvgFrameTimer.tick();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::endFrameTimer()
{
    assert(isValid());
    m_AvgFrameTimer.tock();
}
