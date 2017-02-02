//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <QtAppHelpers/OpenGLMainWindow.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLMainWindow::OpenGLMainWindow(QWidget* parent) : QMainWindow(parent), m_GLWidget(nullptr)
{
    qApp->installEventFilter(this);

    m_lblStatusAvgFrameTime = new QLabel(this);
    m_lblStatusAvgFrameTime->setMargin(5);

    m_lblStatusFPS = new QLabel(this);
    m_lblStatusFPS->setMargin(5);

    statusBar()->addPermanentWidget(m_lblStatusAvgFrameTime, 1);
    statusBar()->addPermanentWidget(m_lblStatusFPS, 1);
    statusBar()->setMinimumHeight(30);
    //statusBar()->setSizeGripEnabled(false);

    ////////////////////////////////////////////////////////////////////////////////
    QSurfaceFormat sformat = QSurfaceFormat::defaultFormat();
    m_VSync = (sformat.swapInterval() > 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLMainWindow::~OpenGLMainWindow()
{
    delete m_GLWidget;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool OpenGLMainWindow::eventFilter(QObject * obj, QEvent * event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        processKeyPressEvent(keyEvent);
        return true;
    }
    else if(event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        processKeyReleaseEvent(keyEvent);
        return true;
    }
    else
    {
        return QObject::eventFilter(obj, event);
    }

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::processKeyPressEvent(QKeyEvent * event)
{
    switch(event->key())
    {
        case Qt::Key_Escape:
            close();
            exit(EXIT_SUCCESS);


        default:
            if(m_GLWidget != nullptr)
            {
                m_GLWidget->keyPressEvent(event);
            }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::processKeyReleaseEvent(QKeyEvent * event)
{
    if(m_GLWidget != nullptr)
    {
        m_GLWidget->keyReleaseEvent(event);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::setArthurStyle()
{
    QStyle* arthurStyle = new ArthurStyle();
    setStyle(arthurStyle);
    QList<QWidget*> widgets = findChildren<QWidget*>();

    foreach(QWidget* w, widgets)
    {
        QString className = QString(w->metaObject()->className());

        if(className == "QScrollBar" || className == "QComboBox" ||
           className == "QCheckBox")
        {
            continue;
        }

        w->setStyle(arthurStyle);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::updateAvgFrameTime(double avgFrameTime)
{
    m_lblStatusAvgFrameTime->setText(QString("PaintGL: %1 ms (~ %2 FPS)")
                                     .arg(avgFrameTime).arg(1000.0 / avgFrameTime));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::updateFrameRate(double fps)
{
    m_lblStatusFPS->setText(QString("Real FPS: %1 | VSync: %2")
                            .arg(fps).arg(m_VSync ? "On" : "Off"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::setupOpenglWidget(OpenGLWidget * glWidget)
{
    if(m_GLWidget != nullptr)
    {
        delete m_GLWidget;
    }

    m_GLWidget = glWidget;
    setCentralWidget(m_GLWidget);
    connect(&m_GLWidget->m_AvgFrameTimer, &AvgTimer::avgTimeChanged, this,
            &OpenGLMainWindow::updateAvgFrameTime);
    connect(&m_GLWidget->m_FPSCounter, &FPSCounter::fpsChanged, this,
            &OpenGLMainWindow::updateFrameRate);
}
