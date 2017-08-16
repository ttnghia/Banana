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
    instantiateOpenGLWidget();
    setupRenderWidgets();
    setupStatusBar();
    setArthurStyle();

    setWindowTitle("Signed Distance Field Visualizer");
    setFocusPolicy(Qt::StrongFocus);
    showFPS(false);
//    showCameraPosition(false);

    ////////////////////////////////////////////////////////////////////////////////
    m_SDFGrid = std::make_unique<SDFGrid>(m_RenderWidget->getParticleDataObj());
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
        updateStatusMemoryUsage();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::instantiateOpenGLWidget()
{
    if(m_GLWidget != nullptr)
    {
        delete m_GLWidget;
    }

    m_RenderWidget = new RenderWidget(this);
    setupOpenglWidget(m_RenderWidget);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::updateStatusMemoryUsage()
{
    m_lblStatusMemoryUsage->setText(QString("Memory usage: %1 (MBs)").arg(QString::fromStdString(NumberHelpers::formatWithCommas(getCurrentRSS() / 1048576.0))));
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
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::connectWidgets()
{
    connect(m_Controller->m_cbSkyTexture->getComboBox(), SIGNAL(currentIndexChanged(int)),                m_RenderWidget, SLOT(setSkyBoxTexture(int)));
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

    m_BusyBar->setBusy(true);
    m_SDFGrid->setSDFObjectType(static_cast<SDFObjectTypes>(0));
}