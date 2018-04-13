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
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include "MainWindow.h"

#include <QMouseEvent>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/System/MemoryUsage.h>
#include <QtAppHelpers/QtAppUtils.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MainWindow::MainWindow(QWidget* parent) : OpenGLMainWindow(parent)
{
    m_RenderWidget = new RenderWidget(this, m_Simulator->getVizData());
    ////////////////////////////////////////////////////////////////////////////////
    setupOpenglWidget(m_RenderWidget);
    setupRenderWidgets();
    setupStatusBar();
    connectWidgets();
    ////////////////////////////////////////////////////////////////////////////////
    setArthurStyle();
    setFocusPolicy(Qt::StrongFocus);
    showFPS(false);
    setWindowTitle("Particle Simulation");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::showEvent(QShowEvent* ev)
{
    QMainWindow::showEvent(ev);
    static bool showed = false;

    if(!showed) {
        showed = true;
        updateStatusMemoryUsage();
        updateStatusSimulationTime(0, 0);

        if(m_Controller->m_cbSimulationScene->count() == 2) {
            m_Controller->m_cbSimulationScene->setCurrentIndex(1);
        } else {
            m_Controller->m_cbSimulationScene->setCurrentIndex(QtAppUtils::getDefaultSceneID());
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MainWindow::processKeyPressEvent(QKeyEvent* event)
{
    switch(event->key()) {
        case Qt::Key_Space:
            m_Controller->m_btnStartStopSimulation->click();
            return true;

        case Qt::Key_X:
            m_Controller->m_btnClipViewPlane->click();
            return true;

        default:
            return OpenGLMainWindow::processKeyPressEvent(event);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::updateWindowTitle(const QString& filePath)
{
    setWindowTitle(QString("Particle Simulation: ") + filePath);
}

void MainWindow::updateStatusMemoryUsage()
{
    m_lblStatusMemoryUsage->setText(QString("Memory usage: %1 (MBs)").arg(QString::fromStdString(NumberHelpers::formatWithCommas(getCurrentRSS() / 1048576.0))));
}

void MainWindow::updateStatusHairInfo()
{
    m_lblStatusHairInfo->setText(QString("Num. particles: %1").arg(QString::fromStdString(NumberHelpers::formatWithCommas(numParticles))));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::setupRenderWidgets()
{
    m_Controller = new Controller(m_RenderWidget, this);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_GLWidget);
    mainLayout->addWidget(m_Controller);

    QWidget* mainWidget = new QWidget(this);
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::setupStatusBar()
{
    m_BusyBar = new BusyBar(this, BusyBar::Cycle, 10);
    statusBar()->addPermanentWidget(m_BusyBar);

    m_lblStatusSimInfo = new QLabel(this);
    m_lblStatusSimInfo->setMargin(5);
    m_lblStatusSimInfo->setText("Ready (Press Space to Start/Stop)");
    statusBar()->addPermanentWidget(m_lblStatusSimInfo, 1);

    m_lblStatusSimTime = new QLabel(this);
    m_lblStatusSimTime->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusSimTime, 1);

    m_lblStatusHairInfo = new QLabel(this);
    m_lblStatusHairInfo->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusHairInfo, 1);

    m_lblStatusMemoryUsage = new QLabel(this);
    m_lblStatusMemoryUsage->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusMemoryUsage, 1);

    QTimer* memTimer = new QTimer(this);
    connect(memTimer, &QTimer::timeout, [&] { updateStatusMemoryUsage(); });
    memTimer->start(5000);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::connectWidgets()
{
    connect(m_ClipPlaneEditor,                &ClipPlaneEditor::clipPlaneChanged, m_RenderWidget, &RenderWidget::setClipPlane);
    connect(m_Controller->m_btnEditClipPlane, &QPushButton::clicked,              [&] { m_ClipPlaneEditor->show(); });
    //    connect(m_Controller->m_chkEnableOutput,  &QCheckBox::toggled,                [&](bool checked)
    //            {
    //                m_bExportImg = checked;
    //                m_Simulator->enableExportImg(checked);
    //            });
    //    connect(m_Simulator, &Simulator::capturePathChanged, m_Controller->m_OutputPath,  &BrowsePathWidget::setPath);
    //    connect(m_Simulator, &Simulator::lightsChanged,      m_Controller->m_LightEditor, &PointLightEditor::changeLights);

    ////////////////////////////////////////////////////////////////////////////////
    //    connect(m_Simulator, &Simulator::numParticleChanged, this,           &MainWindow::updateStatusNumParticles);
    //    connect(m_Simulator, &Simulator::cameraChanged,      m_RenderWidget, &RenderWidget::updateCamera);
    //    connect(m_Simulator, &Simulator::vizDataChanged,     m_RenderWidget, &RenderWidget::updateVizData);
    ////////////////////////////////////////////////////////////////////////////////
}
