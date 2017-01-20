#include <QtAppHelpers/OpenGLWidget.h>
#include <QtAppHelpers/AntTweakBarWrapper.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLWidget::OpenGLWidget(QWidget* parent) :
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::printDebug(QString str)
{
    if(print_debug)
    {
        qDebug() << str.toStdString().c_str();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setPrintDebug(bool pdebug)
{
    print_debug = pdebug;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setUpdateTimeout(int timeout_in_ms)
{
    update_timeout = timeout_in_ms;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setDefaultSize(QSize size)
{
    defaultSize = size;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::setClearColor(QVector4D color)
{
    clearColor = color;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QSize OpenGLWidget::sizeHint() const
{
    return defaultSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::mousePressEvent(QMouseEvent* event)
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
void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if(TwMouseReleaseQt(this, event))
    {
        return;
    }

    mouseButtonPressed = MouseButton::NoButton;

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
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
void OpenGLWidget::wheelEvent(QWheelEvent* event)
{
    if(event->angleDelta().isNull())
    {
        return;
    }

    float zooming_factor = (event->angleDelta().x() + event->angleDelta().y()) / 2000.0f;

    camera.zoom(zooming_factor);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::keyPressEvent(QKeyEvent* event)
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
void OpenGLWidget::keyReleaseEvent(QKeyEvent* event)
{
    (void)event;

    if(specialKeyPressed != SpecialKey::NoKey)
    {
        specialKeyPressed = SpecialKey::NoKey;
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

    glClearColor(clearColor.x(), clearColor.y(), clearColor.z(),
                 clearColor.w());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::resizeGL(int w, int h)
{
    camera.resizeWindow((float)w, (float)h);

    resizeAntTweakBarWindow(w, h);
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

    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLWidget::checkGLErrors()
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
void OpenGLWidget::endFrameTimer()
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
