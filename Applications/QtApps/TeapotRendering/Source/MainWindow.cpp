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

#include <QMouseEvent>

#include "MainWindow.h"

#include <Banana/Utils/NumberHelpers.h>
#include <Banana/System/MemoryUsage.h>
#include <Banana/Macros.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MainWindow::MainWindow(QWidget* parent) : OpenGLMainWindow(parent)
{
    instantiateOpenGLWidget();
    setupRenderWidgets();
    setupStatusBar();
    connectWidgets();
    setArthurStyle();

    setWindowTitle("Real-time Teapot Rendering by OptiX");
    setFocusPolicy(Qt::StrongFocus);
    showFPS(true);
    showCameraPosition(false);
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
bool MainWindow::processKeyPressEvent(QKeyEvent* event)
{
    switch(event->key())
    {
        case Qt::Key_O:
//            m_OutputPath->browse();
            return true;

        case Qt::Key_R:
            m_RenderWidget->resetCamera();

        ////////////////////////////////////////////////////////////////////////////////
        default:
            return OpenGLMainWindow::processKeyPressEvent(event);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::updateStatusInfo(const QString& status)
{
    m_lblStatusInfo->setText(status);
}

void MainWindow::updateStatusMemoryUsage()
{
    m_lblStatusMemoryUsage->setText(QString("Host memory usage: %1 (MBs)").arg(QString::fromStdString(NumberHelpers::formatWithCommas(getCurrentRSS() / 1048576.0))));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::setupRenderWidgets()
{
    ////////////////////////////////////////////////////////////////////////////////
    // setup layouts
    QVBoxLayout* renderLayout = new QVBoxLayout;
    renderLayout->addWidget(m_GLWidget, 1);

    m_Controller = new Controller(this);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addLayout(renderLayout);
    mainLayout->addWidget(m_Controller);

    QWidget* mainWidget = new QWidget(this);
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::setupStatusBar()
{
    m_lblStatusInfo = new QLabel(this);
    m_lblStatusInfo->setMargin(5);
    m_lblStatusInfo->setText("Rendering...");
    statusBar()->addPermanentWidget(m_lblStatusInfo, 2);

    m_lblStatusMemoryUsage = new QLabel(this);
    m_lblStatusMemoryUsage->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusMemoryUsage, 1);

    QTimer* memTimer = new QTimer(this);
    connect(memTimer, &QTimer::timeout, [&]
    {
        updateStatusMemoryUsage();
    });
    memTimer->start(5000);

    m_BusyBar = new BusyBar(this, BusyBar::Cycle, 20);
    statusBar()->addPermanentWidget(m_BusyBar);
    m_BusyBar->setBusy(true);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::connectWidgets()
{
    ////////////////////////////////////////////////////////////////////////////////
    // textures
    connect(m_Controller->m_cbSkyTexture->getComboBox(),   SIGNAL(currentIndexChanged(int)),   m_RenderWidget, SLOT(setSkyBoxTexture(int)));
    connect(m_Controller->m_cbFloorTexture->getComboBox(), SIGNAL(currentIndexChanged(int)),   m_RenderWidget, SLOT(setFloorTexture(int)));
    connect(m_Controller->m_sldFloorSize->getSlider(),     SIGNAL(valueChanged(int)),          m_RenderWidget, SLOT(setFloorSize(int)));
    connect(m_Controller->m_sldFloorTexScale->getSlider(), SIGNAL(valueChanged(int)),          m_RenderWidget, SLOT(setFloorTexScale(int)));

    connect(m_Controller->m_msMeshMaterial,                &MaterialSelector::materialChanged, [&](const Material::MaterialData& material)
            {
                m_RenderWidget->setMeshMaterial(material, m_Controller->m_cbMeshMaterialID->currentIndex());
            });

    ////////////////////////////////////////////////////////////////////////////////
    // buttons
    connect(m_Controller->m_btnResetCamera,    SIGNAL(clicked(bool)), m_RenderWidget, SLOT(resetCamera()));
    connect(m_Controller->m_btnReloadTextures, SIGNAL(clicked(bool)), m_RenderWidget, SLOT(reloadTextures()));
    connect(m_Controller->m_btnExportImage,    SIGNAL(clicked(bool)), m_RenderWidget, SLOT(saveRenderImage()));
    connect(m_Controller->m_btnReloadTextures, SIGNAL(clicked(bool)), m_Controller,   SLOT(loadTextures()));

    ////////////////////////////////////////////////////////////////////////////////
    // lights
    connect(m_Controller->m_LightEditor, &PointLightEditor::lightsChanged, m_RenderWidget,              &RenderWidget::updateLights);
    connect(m_RenderWidget,              &RenderWidget::lightsObjChanged,  m_Controller->m_LightEditor, &PointLightEditor::setLights);
}
