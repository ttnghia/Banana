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
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLMainWindow::OpenGLMainWindow(QWidget* parent, bool bShowFPS /*= true*/, bool bShowCamPosition /*= true*/) : QMainWindow(parent), m_GLWidget(nullptr)
{
    qApp->installEventFilter(this);

    m_lblStatusFPS = new QLabel(this);
    m_lblStatusFPS->setMargin(5);

    m_lblStatusCamPosition = new QLabel(this);
    m_lblStatusCamPosition->setMargin(5);

    statusBar()->addPermanentWidget(m_lblStatusFPS, 1);
    statusBar()->addPermanentWidget(m_lblStatusCamPosition, 1);
    statusBar()->setMinimumHeight(30);
    //statusBar()->setSizeGripEnabled(false);

    if(!bShowFPS)
    {
        m_lblStatusFPS->hide();
    }
    if(!bShowCamPosition)
    {
        m_lblStatusCamPosition->hide();
    }

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
bool OpenGLMainWindow::eventFilter(QObject*, QEvent* ev)
{
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
bool OpenGLMainWindow::processKeyPressEvent(QKeyEvent* ev)
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
void OpenGLMainWindow::showFPS(bool bShowFPS)
{
    if(bShowFPS)
    {
        m_lblStatusFPS->show();
    }
    else
    {
        m_lblStatusFPS->hide();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::showCameraPosition(bool bShowCamPosition)
{
    if(bShowCamPosition)
        m_lblStatusCamPosition->show();
    else
        m_lblStatusCamPosition->hide();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::setArthurStyle()
{
    QStyle* arthurStyle = new ArthurStyle();
    setStyle(arthurStyle);

    QList<QWidget*> widgets = findChildren<QWidget*>();

    foreach(QWidget * w, widgets)
    {
        QString className = QString(w->metaObject()->className());

        if((className == "QScrollBar") || (className == "QComboBox") || (className == "QCheckBox"))
        {
            continue;
        }

        w->setStyle(arthurStyle);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::updateStatusFrameRate(double fps)
{
    m_lblStatusFPS->setText(QString("FPS: %1 (%2 ms/frame) | VSync: %3").arg(fps).arg(1000.0 / fps).arg(m_VSync ? "On" : "Off"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::updateStatusCameraPosition(const glm::vec3& camPosition)
{
    m_lblStatusCamPosition->setText(QString("Camera: [%1, %2, %3]").arg(camPosition[0]).arg(camPosition[1]).arg(camPosition[2]));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::setupOpenglWidget(OpenGLWidget* glWidget)
{
    if(m_GLWidget != nullptr)
    {
        delete m_GLWidget;
    }

    m_GLWidget = glWidget;
    setCentralWidget(m_GLWidget);
    connect(&m_GLWidget->m_FPSCounter, &FPSCounter::fpsChanged,              this, &OpenGLMainWindow::updateStatusFrameRate);
    connect(m_GLWidget,                &OpenGLWidget::cameraPositionChanged, this, &OpenGLMainWindow::updateStatusCameraPosition);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana