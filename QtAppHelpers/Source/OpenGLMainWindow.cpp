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

#include <QtAppHelpers/OpenGLMainWindow.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
OpenGLMainWindow::OpenGLMainWindow(QWidget* parent, bool bShowFPS /*= true*/, bool bShowCamPosition /*= true*/) : QMainWindow(parent)
{
    qApp->installEventFilter(this);
    m_VSync = (QSurfaceFormat::defaultFormat().swapInterval() > 0);
    ////////////////////////////////////////////////////////////////////////////////
    // setup status bar
    m_lblStatusFPS = new QLabel(this);
    m_lblStatusFPS->setMargin(5);

    m_lblStatusCameraInfo = new QLabel(this);
    m_lblStatusCameraInfo->setMargin(5);

    statusBar()->addPermanentWidget(m_lblStatusFPS, 1);
    statusBar()->addPermanentWidget(m_lblStatusCameraInfo, 1);
    statusBar()->setMinimumHeight(30);
    //statusBar()->setSizeGripEnabled(false);
    ////////////////////////////////////////////////////////////////////////////////
    if(!bShowFPS) {
        m_lblStatusFPS->hide();
    }
    if(!bShowCamPosition) {
        m_lblStatusCameraInfo->hide();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool OpenGLMainWindow::processKeyPressEvent(QKeyEvent* ev)
{
    switch(ev->key()) {
        case Qt::Key_Escape:
            close();
            __BANANA_EARLY_TERMINATION
        //exit(EXIT_SUCCESS);

        default:
            return false;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool OpenGLMainWindow::eventFilter(QObject*, QEvent* ev)
{
    switch(ev->type()) {
        case QEvent::KeyPress:
            return processKeyPressEvent(static_cast<QKeyEvent*>(ev));
        case QEvent::KeyRelease:
            return processKeyReleaseEvent(static_cast<QKeyEvent*>(ev));
        default:
            return false;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::setArthurStyle()
{
    QStyle* arthurStyle = new ArthurStyle();
    setStyle(arthurStyle);
    ////////////////////////////////////////////////////////////////////////////////
    QList<QWidget*> widgets = findChildren<QWidget*>();
    foreach(QWidget* w, widgets) {
        QString className = QString(w->metaObject()->className());
        if(className == "QScrollBar" || className == "QComboBox" || className == "QCheckBox") {
            continue;
        }
        w->setStyle(arthurStyle);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::showFPS(bool bShowFPS)
{
    if(bShowFPS) {
        m_lblStatusFPS->show();
    } else {
        m_lblStatusFPS->hide();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::showCameraPosition(bool bShowCamPosition)
{
    if(bShowCamPosition) {
        m_lblStatusCameraInfo->show();
    } else {
        m_lblStatusCameraInfo->hide();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::updateStatusFrameRate(double fps)
{
    m_lblStatusFPS->setText(QString("FPS: %1 (%2 ms/frame) | VSync: %3").arg(fps).arg(1000.0 / fps).arg(m_VSync ? "On" : "Off"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::updateStatusCameraInfo(const Vec3f& camPosition, const Vec3f& camFocus)
{
    m_lblStatusCameraInfo->setText(QString("Camera: [%1, %2, %3] | Focus: [%4, %5, %6]")
                                       .arg(camPosition[0], 0, 'f', 2)
                                       .arg(camPosition[1], 0, 'f', 2)
                                       .arg(camPosition[2], 0, 'f', 2)
                                       .arg(camFocus[0],    0, 'f', 2)
                                       .arg(camFocus[1],    0, 'f', 2)
                                       .arg(camFocus[2],    0, 'f', 2));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLMainWindow::setupOpenglWidget(OpenGLWidget* glWidget)
{
    __BNN_REQUIRE(glWidget != nullptr);
    if(m_GLWidget != nullptr) {
        delete m_GLWidget;
    }

    m_GLWidget = glWidget;
    setCentralWidget(m_GLWidget);
    connect(&m_GLWidget->m_FPSCounter, &FPSCounter::fpsChanged,                  this, &OpenGLMainWindow::updateStatusFrameRate);
    connect(m_GLWidget,                &OpenGLWidget::cameraPositionInfoChanged, this, &OpenGLMainWindow::updateStatusCameraInfo);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana