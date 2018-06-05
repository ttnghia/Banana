//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

#include "MainWindow.h"

#include <QMouseEvent>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/System/MemoryUsage.h>
#include <QtAppHelpers/QtAppUtils.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MainWindow::MainWindow(QWidget* parent) : OpenGLMainWindow(parent)
{
    m_Simulator    = new Simulator();
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
    setWindowTitle("Particle Generator");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::showEvent(QShowEvent* ev)
{
    QMainWindow::showEvent(ev);
    static bool showed = false;

    if(!showed) {
        showed = true;
        Q_ASSERT(m_Simulator != nullptr);
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
    setWindowTitle(QString("Particle Generator: ") + filePath);
}

void MainWindow::updateStatusSimulation(const QString& status)
{
    m_lblStatusSimInfo->setText(status);
}

void MainWindow::updateStatusMemoryUsage()
{
    m_lblStatusMemoryUsage->setText(QString("Memory usage: %1 (MBs)").arg(QString::fromStdString(NumberHelpers::formatWithCommas(getCurrentRSS() / 1048576.0))));
}

void MainWindow::updateStatusNumParticles(UInt numParticles)
{
    m_lblStatusNumParticles->setText(QString("Num. particles: %1").arg(QString::fromStdString(NumberHelpers::formatWithCommas(numParticles))));
}

void MainWindow::updateStatusSimulationTime(float time, UInt frame)
{
    m_lblStatusSimTime->setText(QString("System time: %1 (s) | Frames: %2")
                                    .arg(QString::fromStdString(NumberHelpers::formatWithCommas(time, 5)))
                                    .arg(QString::fromStdString(NumberHelpers::formatWithCommas(frame)))
                                );
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::finishFrame()
{
    ++m_FrameNumber;

    if(m_bExportImg) {
        m_RenderWidget->exportScreenToImage(m_FrameNumber);
        m_Simulator->finishImgExport();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::finishSimulation()
{
    m_Controller->m_btnStartStopSimulation->setText(QString("Start"));
    updateStatusSimulation("Simulation Finished");
    m_Controller->m_cbSimulationScene->setDisabled(false);
    m_Controller->m_btnReloadScene->setDisabled(false);
    m_BusyBar->reset();
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

    m_lblStatusNumParticles = new QLabel(this);
    m_lblStatusNumParticles->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusNumParticles, 1);

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
    ////////////////////////////////////////////////////////////////////////////////
    // simulation
    connect(m_Controller->m_btnReloadScene,   &QPushButton::clicked,
            [&]()
            {
                QString sceneFile = m_Controller->m_cbSimulationScene->currentText();

                if(sceneFile == "None") {
                    return;
                }
                m_Simulator->changeScene(sceneFile);
                m_FrameNumber = 0;
            });

    connect(m_Controller->m_cbSimulationScene, &QComboBox::currentTextChanged, [&](const QString& sceneFile)
            {
                if(sceneFile == "None") {
                    return;
                }

                m_Simulator->changeScene(sceneFile);
                m_FrameNumber = 0;
                updateWindowTitle(QtAppUtils::getDefaultPath("Scenes") + "/" + sceneFile);
            });

    connect(m_Controller->m_btnStartStopSimulation, &QPushButton::clicked, [&]()
            {
                if(m_Controller->m_cbSimulationScene->currentText() == "None") {
                    return;
                }
                bool isRunning = m_Simulator->isRunning();

                if(!isRunning) {
                    m_Simulator->startSimulation();
                    m_Controller->m_cbSimulationScene->setDisabled(true);
                    m_Controller->m_btnReloadScene->setDisabled(true);
                    updateStatusSimulation("Running simulation...");
                } else {
                    m_Simulator->stop();
                    m_Controller->m_cbSimulationScene->setDisabled(false);
                    m_Controller->m_btnReloadScene->setDisabled(false);
                    updateStatusSimulation("Stopped");
                }
                m_Controller->m_btnStartStopSimulation->setText(!isRunning ? QString("Stop") : QString("Resume"));
                m_BusyBar->setBusy(!isRunning);
            });

    connect(m_Controller->m_chkEnableOutput, &QCheckBox::toggled, [&](bool checked)
            {
                m_bExportImg = checked;
                m_Simulator->enableExportImg(checked);
            });
    connect(m_Simulator, &Simulator::capturePathChanged, m_Controller->m_OutputPath,  &BrowsePathWidget::setPath);
    connect(m_Simulator, &Simulator::lightsChanged,      m_Controller->m_LightEditor, &PointLightEditor::changeLights);
    ////////////////////////////////////////////////////////////////////////////////
    // sim status
    connect(m_Simulator, &Simulator::frameFinished,      [&] { QMetaObject::invokeMethod(this, "finishFrame", Qt::QueuedConnection); });
    connect(m_Simulator, &Simulator::simulationFinished, [&] { QMetaObject::invokeMethod(this, "finishSimulation", Qt::QueuedConnection); });
    connect(m_Simulator, &Simulator::numParticleChanged, this,           &MainWindow::updateStatusNumParticles);
    connect(m_Simulator, &Simulator::systemTimeChanged,  this,           &MainWindow::updateStatusSimulationTime);
    connect(m_Simulator, &Simulator::dimensionChanged,   m_RenderWidget, &RenderWidget::updateSolverDimension);
    connect(m_Simulator, &Simulator::domainChanged,      m_RenderWidget, &RenderWidget::setBox);
    connect(m_Simulator, &Simulator::cameraChanged,      m_RenderWidget, &RenderWidget::updateCamera);
    connect(m_Simulator, &Simulator::vizDataChanged,     m_RenderWidget, &RenderWidget::updateVizData);
    ////////////////////////////////////////////////////////////////////////////////
}
