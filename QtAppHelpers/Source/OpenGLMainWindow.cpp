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

#include <QtAppHelpers/OpenGLMainWindow.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLMainWindow::~OpenGLMainWindow()
{
    delete m_GLWidget;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool OpenGLMainWindow::eventFilter(QObject * obj, QEvent * ev)
{
    ////////////////////////////////////////////////////////////////////////////////
    if(ev->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(ev);
        return processKeyPressEvent(keyEvent);
    }

    if(ev->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(ev);

        return processKeyReleaseEvent(keyEvent);
    }

    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool OpenGLMainWindow::processKeyPressEvent(QKeyEvent * ev)
{
    switch(ev->key())
    {
        case Qt::Key_Escape:
            close();
            exit(EXIT_SUCCESS);


        default:
            return false;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::setArthurStyle()
{
    QStyle* arthurStyle = new ArthurStyle();
    setStyle(arthurStyle);
    QList<QWidget*> widgets = findChildren<QWidget*>();

    foreach(QWidget* w, widgets)
    {
        QString className = QString(w->metaObject()->className());

        if(className == "QScrollBar" || className == "QComboBox" || className == "QCheckBox")
        {
            continue;
        }

        w->setStyle(arthurStyle);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::updatePaintGLTime(double avgFrameTime)
{
    m_lblStatusAvgFrameTime->setText(QString("PaintGL: %1 ms (~ %2 FPS)").arg(avgFrameTime).arg(1000.0 / avgFrameTime));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::updateFrameRate(double fps)
{
    m_lblStatusFPS->setText(QString("Real FPS: %1 | VSync: %2")
                            .arg(fps).arg(m_VSync ? "On" : "Off"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::setupOpenglWidget(OpenGLWidget * glWidget)
{
    if(m_GLWidget != nullptr)
    {
        delete m_GLWidget;
    }

    m_GLWidget = glWidget;
    setCentralWidget(m_GLWidget);
    connect(&m_GLWidget->m_AvgPaintGLTimer, &AvgTimer::avgTimeChanged, this, &OpenGLMainWindow::updatePaintGLTime);
    connect(&m_GLWidget->m_FPSCounter, &FPSCounter::fpsChanged, this, &OpenGLMainWindow::updateFrameRate);
}
