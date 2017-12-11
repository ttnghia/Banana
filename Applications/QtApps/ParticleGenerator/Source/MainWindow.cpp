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
#include <Banana/System/MemoryUsage.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MainWindow::MainWindow(QWidget* parent) : OpenGLMainWindow(parent)
{
    m_RenderWidget = new RenderWidget(this);
    m_SDFGrid      = std::make_unique<SDFGrid>(m_RenderWidget->getParticleDataObj());
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
        updateStatusMemoryUsage();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MainWindow::processKeyPressEvent(QKeyEvent* event)
{
    switch(event->key()) {
        case Qt::Key_X:
            m_Controller->m_btnEnableClipPlane->click();
            return true;

        default:
            return OpenGLMainWindow::processKeyPressEvent(event);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::updateStatusMemoryUsage()
{
    m_lblStatusMemoryUsage->setText(QString("Memory usage: %1 (MBs)").arg(QString::fromStdString(NumberHelpers::formatWithCommas(getCurrentRSS() / 1048576.0))));
}

void MainWindow::updateStatusSDFGenerationTime(double runTime)
{
    m_lblStatusSDFGenerationTime->setText(QString("SDF generation: %1 (milliseconds)").arg(runTime));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::finilizeSDFGeneration()
{
    m_BusyBar->setBusy(false);
    m_BusyBar->reset();
    m_RenderWidget->updateParticleData();
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
    m_BusyBar = new BusyBar(this, BusyBar::Cycle, 0);
//    m_prBusy->setBusy(true);
    statusBar()->addPermanentWidget(m_BusyBar);

    m_lblStatusMemoryUsage = new QLabel(this);
    m_lblStatusMemoryUsage->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusMemoryUsage, 1);

    QTimer* memTimer = new QTimer(this);
    connect(memTimer, &QTimer::timeout, [&]
            {
                updateStatusMemoryUsage();
            });
    memTimer->start(5000);

    m_lblStatusSDFGenerationTime = new QLabel(this);
    m_lblStatusSDFGenerationTime->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusSDFGenerationTime, 1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::connectWidgets()
{
    connect(m_ClipPlaneEditor.get(),                     &ClipPlaneEditor::clipPlaneChanged,   m_RenderWidget, &RenderWidget::setClipPlane);
    connect(m_Controller->m_btnEditClipPlane,            &QPushButton::clicked,                           [&] { m_ClipPlaneEditor->show(); });
    connect(m_Controller->m_btnEnableClipPlane,          &QPushButton::clicked,                           m_RenderWidget, &RenderWidget::enableClipPlane);
    connect(m_SDFGrid.get(),                             &SDFGrid::generationTimeChanged,                 this,           &MainWindow::updateStatusSDFGenerationTime);

    ////////////////////////////////////////////////////////////////////////////////
    connect(m_Controller->m_msNegativeParticleMaterial,  SIGNAL(materialChanged(Material::MaterialData)), m_RenderWidget, SLOT(setNegativeParticleMaterial(Material::MaterialData)));
    connect(m_Controller->m_msPositiveParticleMaterial,  SIGNAL(materialChanged(Material::MaterialData)), m_RenderWidget, SLOT(setPositiveParticleMaterial(Material::MaterialData)));
    connect(m_Controller->m_slNegativeParticleSize,      SIGNAL(valueChanged(int)),                       m_RenderWidget, SLOT(setNegativeParticleSize(int)));
    connect(m_Controller->m_slPositiveParticleSize,      SIGNAL(valueChanged(int)),                       m_RenderWidget, SLOT(setPositiveParticleSize(int)));

    ////////////////////////////////////////////////////////////////////////////////
    // lights
    connect(m_Controller->m_LightEditor, &PointLightEditor::lightsChanged, m_RenderWidget,              &RenderWidget::updateLights);
    connect(m_RenderWidget,              &RenderWidget::lightsObjChanged,  m_Controller->m_LightEditor, &PointLightEditor::setLights);

    ////////////////////////////////////////////////////////////////////////////////
    // SDF generator
    connect(m_Controller->m_cbResolution->getComboBox(), &QComboBox::currentTextChanged,
            [&](const QString& resolutionStr)
            {
                m_BusyBar->setBusy(true);
                m_SDFGrid->setResolution(resolutionStr.toInt());
            });

    connect(m_Controller->m_cbSDFObject->getComboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [&](int idx)
            {
                m_BusyBar->setBusy(true);
                m_SDFGrid->setSDFObjectType(static_cast<SDFObjectTypes>(idx));
            });

    connect(m_SDFGrid.get(), &SDFGrid::dataReady, [&]()
            {
                QMetaObject::invokeMethod(this, "finilizeSDFGeneration", Qt::QueuedConnection);
            });

    connect(m_Controller->m_chkHideNegativeParticles, &QCheckBox::clicked, m_RenderWidget, &RenderWidget::hideNegativeParticles);
    connect(m_Controller->m_chkHidePositiveParticles, &QCheckBox::clicked, m_RenderWidget, &RenderWidget::hidePositiveParticles);
    connect(m_Controller, &Controller::transformationChanged, m_SDFGrid.get(), &SDFGrid::setTransformation);

    m_BusyBar->setBusy(true);
    m_SDFGrid->setSDFObjectType(static_cast<SDFObjectTypes>(0));
}