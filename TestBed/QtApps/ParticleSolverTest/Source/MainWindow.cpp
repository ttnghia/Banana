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

#include "MainWindow.h"

#include <QMouseEvent>
#include <Banana/Utils/NumberHelpers.h>
#include <Banana/System/MemoryUsage.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MainWindow::MainWindow(QWidget* parent) : OpenGLMainWindow(parent)
{
    instantiateOpenGLWidget();
    setupRenderWidgets();
    setupStatusBar();
    setArthurStyle();

    setWindowTitle("Particle Simulation");
    setFocusPolicy(Qt::StrongFocus);
    showFPS(false);
//    showCameraPosition(false);

    connectWidgets();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::showEvent(QShowEvent* ev)
{
    QMainWindow::showEvent(ev);

    static bool showed = false;
    if(!showed)
    {
        showed = true;

        Q_ASSERT(m_Simulator != nullptr);
        updateStatusMemoryUsage();
        updateStatusSimulationTime(0);
        m_Simulator->changeScene(m_Controller->m_cbSimulationScene->currentText());
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::instantiateOpenGLWidget()
{
    if(m_GLWidget != nullptr)
    {
        delete m_GLWidget;
    }

    m_Simulator    = std::make_unique<Simulator>();
    m_RenderWidget = new RenderWidget(m_Simulator->getSolver()->getParticlePositions(), this);
    m_Simulator->setParticleSystemData(m_RenderWidget->getParticleDataObj());
    setupOpenglWidget(m_RenderWidget);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MainWindow::processKeyPressEvent(QKeyEvent* event)
{
    switch(event->key())
    {
        case Qt::Key_Space:
            m_Controller->m_btnStartStopSimulation->click();
            return true;

        case Qt::Key_X:
            m_Controller->m_btnEnableClipPlane->click();
            return true;

        default:
            return OpenGLMainWindow::processKeyPressEvent(event);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::updateStatusSimulation(const QString& status)
{
    m_lblStatusSimInfo->setText(status);
}

void MainWindow::updateStatusMemoryUsage()
{
    m_lblStatusMemoryUsage->setText(QString("Memory usage: %1 (MBs)").arg(QString::fromStdString(NumberHelpers::formatWithCommas(getCurrentRSS() / 1048576.0))));
}

void MainWindow::updateStatusNumParticles(unsigned int numParticles)
{
    m_lblStatusNumParticles->setText(QString("Num. particles: %1").arg(QString::fromStdString(NumberHelpers::formatWithCommas(numParticles))));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::updateStatusSimulationTime(float time)
{
    m_lblStatusSimTime->setText(QString("System time: %1 (s)").arg(QString::fromStdString(NumberHelpers::formatWithCommas(time, 5))));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::finishFrame()
{
    ++m_FrameNumber;
    if(m_Controller->m_chkEnableOutput->isChecked())
        m_RenderWidget->exportScreenToImage(m_FrameNumber);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::finishSimulation()
{
    m_Controller->m_btnStartStopSimulation->setText(QString("Start"));
    updateStatusSimulation("Simulation Finished");
    m_BusyBar->reset();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::setupRenderWidgets()
{
    m_Controller = new Controller(this);

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
    m_BusyBar = new BusyBar(this, BusyBar::Cycle, 20);
//    m_prBusy->setBusy(true);
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
    connect(memTimer, &QTimer::timeout, [&]
    {
        updateStatusMemoryUsage();
    });
    memTimer->start(5000);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::connectWidgets()
{
    connect(m_ClipPlaneEditor.get(),                       SIGNAL(clipPlaneChanged(glm::vec4)),             m_RenderWidget, SLOT(setClipPlane(glm::vec4)));
    connect(m_Controller->m_btnEditClipPlane,              &QPushButton::clicked,                           [&] { m_ClipPlaneEditor->show(); });
    connect(m_Controller->m_btnEnableClipPlane,            &QPushButton::clicked,                           m_RenderWidget, &RenderWidget::enableClipPlane);
    ////////////////////////////////////////////////////////////////////////////////
    // textures
    connect(m_Controller->m_cbSkyTexture->getComboBox(),   SIGNAL(currentIndexChanged(int)),                m_RenderWidget, SLOT(setSkyBoxTexture(int)));
    connect(m_Controller->m_cbFloorTexture->getComboBox(), SIGNAL(currentIndexChanged(int)),                m_RenderWidget, SLOT(setFloorTexture(int)));
    connect(m_Controller->m_sldFloorSize->getSlider(),     SIGNAL(valueChanged(int)),                       m_RenderWidget, SLOT(setFloorSize(int)));
    connect(m_Controller->m_sldFloorExposure->getSlider(), SIGNAL(valueChanged(int)),                       m_RenderWidget, SLOT(setFloorExposure(int)));

    // render modes/colors
    connect(m_Controller->m_smParticleColorMode,           SIGNAL(mapped(int)),                             m_RenderWidget, SLOT(setParticleColorMode(int)));
    connect(m_Controller->m_msParticleMaterial,            SIGNAL(materialChanged(Material::MaterialData)), m_RenderWidget, SLOT(setParticleMaterial(Material::MaterialData)));

    ////////////////////////////////////////////////////////////////////////////////
    // lights
    connect(m_Controller->m_LightEditor, &PointLightEditor::lightsChanged, m_RenderWidget,              &RenderWidget::updateLights);
    connect(m_RenderWidget,              &RenderWidget::lightsObjChanged,  m_Controller->m_LightEditor, &PointLightEditor::setLights);

    ////////////////////////////////////////////////////////////////////////////////
    // simulation
    connect(m_Controller->m_cbSimulationScene, &QComboBox::currentTextChanged, [&](const QString& scene) { m_Simulator->changeScene(scene); });
    connect(m_Simulator.get(),                 &Simulator::boxChanged,         [&](const glm::vec3& boxMin, const glm::vec3& boxMax)
            {
                m_RenderWidget->setBox(boxMin, boxMax);
                Vec3r camPos((boxMin.x + boxMax.x) * 0.5f,
                             (boxMin.y + boxMax.y) * 0.5f + (boxMax.y - boxMin.y) * 0.3,
                             (boxMin.z + boxMax.z) * 0.5f + (boxMax.z - boxMin.z) * 1.5);

                Vec3r camFocus((boxMin.x + boxMax.x) * 0.5f,
                               (boxMin.y + boxMax.y) * 0.5f - (boxMax.y - boxMin.y) * 0.1,
                               (boxMin.z + boxMax.z) * 0.5f);
                m_RenderWidget->setCamera(camPos, camFocus);
            });

    connect(m_Controller->m_btnStartStopSimulation, &QPushButton::clicked, [&]
    {
        bool isRunning = m_Simulator->isRunning();

        if(!isRunning)
        {
            m_Simulator->startSimulation();
            updateStatusSimulation("Running simulation...");
        }
        else
        {
            m_Simulator->stop();
            updateStatusSimulation("Stopped");
        }

        m_Controller->m_btnStartStopSimulation->setText(!isRunning ? QString("Stop") : QString("Resume"));
//        m_Controller->disableParameters(!isRunning);
        m_BusyBar->setBusy(!isRunning);
    });

    connect(m_Simulator.get(), &Simulator::frameFinished, [&]
    {
        QMetaObject::invokeMethod(this, "finishFrame", Qt::QueuedConnection);
    });

    connect(m_Simulator.get(), &Simulator::simulationFinished, [&]
    {
        QMetaObject::invokeMethod(this, "finishSimulation", Qt::QueuedConnection);
    });

    connect(m_Simulator.get(), &Simulator::numParticleChanged, this,           &MainWindow::updateStatusNumParticles);
    connect(m_Simulator.get(), &Simulator::systemTimeChanged,  this,           &MainWindow::updateStatusSimulationTime);
    connect(m_Simulator.get(), &Simulator::particleChanged,    m_RenderWidget, &RenderWidget::updateParticleData);
}