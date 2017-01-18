//------------------------------------------------------------------------------------------
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 09/07/2016
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
//------------------------------------------------------------------------------------------
#include <Mango/Core/OpenGLMainWindow.h>

//------------------------------------------------------------------------------------------
OpenGLMainWindow::OpenGLMainWindow(QWidget* parent)
    : QMainWindow(parent), glWidget(nullptr)
{
    qApp->installEventFilter(this);
//    connect(glWidget, &OpenGLWidget::frameRateChanged, this, &OpenGLMainWindow::updateFrameRate);

    lblStatusFPS = new QLabel(this);
    lblStatusFPS->setMargin(5);
    statusBar()->addPermanentWidget(lblStatusFPS);
    statusBar()->setMinimumHeight(30);
    statusBar()->setSizeGripEnabled(false);
}

//------------------------------------------------------------------------------------------
void OpenGLMainWindow::processKeyPressEvent(QKeyEvent* event)
{
    switch(event->key())
    {
    case Qt::Key_Escape:
        exit(EXIT_SUCCESS);

    default:
        if(glWidget != nullptr)
        {
            glWidget->keyPressEvent(event);
        }
    }
}

//------------------------------------------------------------------------------------------
void OpenGLMainWindow::processKeyReleaseEvent(QKeyEvent* event)
{
    (void)event;

}

//------------------------------------------------------------------------------------------
void OpenGLMainWindow::updateFrameRate(double avgFrameTime)
{
    lblStatusFPS->setText(QString("Frame time: %1ms (~ %2 FPS)")
                          .arg(avgFrameTime).arg(1000.0 / avgFrameTime));
}

//------------------------------------------------------------------------------------------
void OpenGLMainWindow::setupOpenglWidget(OpenGLWidget* _glWidget)
{
    if(glWidget != nullptr)
    {
        delete glWidget;
    }

    glWidget = _glWidget;
    setCentralWidget(glWidget);
//    connect(glWidget, &OpenGLWidget::frameRateChanged, this,
//            &OpenGLMainWindow::updateFrameRate);
    connect(&glWidget->FPSTimer, &AvgTimer::avgTimeChanged, this,
            &OpenGLMainWindow::updateFrameRate);
}

//------------------------------------------------------------------------------------------
bool OpenGLMainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        processKeyPressEvent(keyEvent);
    }
    else if(event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        processKeyReleaseEvent(keyEvent);
    }

    return QObject::eventFilter(obj, event);
}
