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

#include <QtAppHelpers/OpenGLWidget.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent)
{
    m_UpdateTimer = std::make_unique<QTimer>(this);
    connect(m_UpdateTimer.get(), SIGNAL(timeout()), this, SLOT(update()));
    m_UpdateTimer->start(0);
    ////////////////////////////////////////////////////////////////////////////////
    connect(this, &OpenGLWidget::emitDebugString, this, &OpenGLWidget::printDebugString);
    ////////////////////////////////////////////////////////////////////////////////
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setClearColor(const Vec3f& color)
{
    m_ClearColor = color;
    if(isValid()) {
        makeCurrent();
        resetClearColor();
        doneCurrent();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool OpenGLWidget::exportScreenToImage(int frame)
{
    if(m_CapturePath.isEmpty()) {
        return false;
    }
    ////////////////////////////////////////////////////////////////////////////////
    makeCurrent();
    glCall(glReadPixels(0, 0, width(), height(), GL_RGB, GL_UNSIGNED_BYTE, m_CaptureImage->bits()));
    doneCurrent();
    m_CaptureImage->mirrored().save(QString(m_CapturePath + "/frame.%1.jpg").arg(frame, 4, 10, QChar('0')));
    ////////////////////////////////////////////////////////////////////////////////
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::mousePressEvent(QMouseEvent* ev)
{
    if(ev->button() == Qt::LeftButton) {
        m_MouseButtonPressed = MouseButton::LeftButton;
    } else if(ev->button() == Qt::RightButton) {
        m_MouseButtonPressed = MouseButton::RightButton;
    } else {
        m_MouseButtonPressed = MouseButton::NoButton;
    }
    ////////////////////////////////////////////////////////////////////////////////
    m_Camera->set_last_mouse_pos(ev->x(), ev->y());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::mouseMoveEvent(QMouseEvent* ev)
{
    if(m_MouseButtonPressed == MouseButton::LeftButton) {
        m_Camera->rotate_by_mouse(ev->x(), ev->y());
    } else if(m_MouseButtonPressed == MouseButton::RightButton) {
        if(m_SpecialKeyPressed == SpecialKey::NoKey) {
            m_Camera->translate_by_mouse(ev->x(), ev->y());
        } else {
            m_Camera->zoom_by_mouse(ev->x(), ev->y());
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::wheelEvent(QWheelEvent* ev)
{
    if(ev->angleDelta().isNull()) {
        return;
    }

    float zoomFactor = (ev->angleDelta().x() + ev->angleDelta().y()) / 5000.0f;
    m_Camera->zoom(zoomFactor);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::keyPressEvent(QKeyEvent* ev)
{
    switch(ev->key()) {
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
            m_Camera->translate(Vec2f(0.0f, 0.1f));
            break;

        case Qt::Key_Down:
            m_Camera->translate(Vec2f(0.0f, -0.1f));
            break;

        case Qt::Key_Left:
            m_Camera->translate(Vec2f(-0.1f, 0.0f));
            break;

        case Qt::Key_Right:
            m_Camera->translate(Vec2f(0.1f, 0.0f));
            break;

        case Qt::Key_C:
            m_Camera->reset();
            break;

        default:
            ;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    checkGLVersion();
    checkGLErrors();
    resetClearColor();
    glCall(glEnable(GL_DEPTH_TEST));
    glCall(glEnable(GL_MULTISAMPLE));
    ////////////////////////////////////////////////////////////////////////////////
    m_CaptureImage = std::make_unique<QImage>(width(), height(), QImage::Format_RGB888);

    ////////////////////////////////////////////////////////////////////////////////
    // view matrix, prj matrix, inverse view matrix, inverse proj matrix, shadow matrix, cam position
    m_UBufferCamData = std::make_shared<OpenGLBuffer>();
    m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER, 5 * sizeof(Mat4x4f) + sizeof(Vec4f), nullptr, GL_DYNAMIC_DRAW);
    emit cameraPositionInfoChanged(m_Camera->getCameraPosition(), m_Camera->getCameraFocus());

    ////////////////////////////////////////////////////////////////////////////////
    // call init function from derived class
    initOpenGL();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::resizeGL(int w, int h)
{
    glCall(glViewport(0, 0, w, h));
    m_Camera->resizeWindow((float)w, (float)h);
    m_CaptureImage.reset(new QImage(w, h, QImage::Format_RGB888));

    ////////////////////////////////////////////////////////////////////////////////
    // call init function from derived class
    resizeOpenGLWindow(w, h);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::paintGL()
{
    m_FPSCounter.countFrame();
    glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
    uploadCameraData();

    ////////////////////////////////////////////////////////////////////////////////
    // call init function from derived class
    renderOpenGL();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::uploadCameraData()
{
    m_Camera->updateCameraMatrices();
    if(m_Camera->isCameraChanged()) {
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()),              0,                   sizeof(Mat4x4f));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()),        sizeof(Mat4x4f),     sizeof(Mat4x4f));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getInverseViewMatrix()),       2 * sizeof(Mat4x4f), sizeof(Mat4x4f));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getInverseProjectionMatrix()), 3 * sizeof(Mat4x4f), sizeof(Mat4x4f));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getCameraPosition()),          5 * sizeof(Mat4x4f), sizeof(Vec3f));
        emit cameraPositionInfoChanged(m_Camera->getCameraPosition(), m_Camera->getCameraFocus());
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::checkGLErrors()
{
    glCall(GLenum glStatus = glGetError());
    if(glStatus == GL_NO_ERROR) {
        return;
    }

    QString glErrStr = "OpenGL error: ";

    switch(glStatus) {
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
    glCall(QString deviceStr = QString((const char*)glGetString(GL_RENDERER)));
    glCall(QString verStr    = QString((const char*)glGetString(GL_VERSION)));
    emit emitDebugString(QString("GPU: ") + deviceStr);
    emit emitDebugString(QString("OpenGL driver: ") + verStr);

    int major = verStr.left(verStr.indexOf(".")).toInt();
    int minor = verStr.mid(verStr.indexOf(".") + 1, 1).toInt();

    if(!(major >= 4 && minor >= 1)) {
        QMessageBox msgBox(QMessageBox::Critical, "Error", QString("Your OpenGL version is %1.%2 (Required: OpenGL >= 4.1).").arg(major).arg(minor));
        msgBox.exec();
        __BANANA_EARLY_TERMINATION
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::checkGLExtensions(const QVector<QString>& extensions)
{
    glCall(QString extStr = QString((const char*)glGetString(GL_EXTENSIONS)));
    emit emitDebugString(extStr);

    bool check = true;
    for(QString ext : extensions) {
        if(!extStr.contains(ext)) {
            QMessageBox msgBox(QMessageBox::Critical, "Error", QString("Extension %1 is not supported.").arg(ext));
            msgBox.exec();
            check = false;
        }
    }

    if(!check) {
        __BANANA_EARLY_TERMINATION
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setCapturePath(const QString& path)
{
    m_CapturePath = path;
    if(!QDir(path).exists()) {
        QDir().mkdir(path);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana