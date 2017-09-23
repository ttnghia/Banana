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

#include "Common.h"
#include "Controller.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupGUI()
{
    QVBoxLayout* controlLayout = new QVBoxLayout;
    QVBoxLayout* btnLayout     = new QVBoxLayout;

    setupTextureControllers(controlLayout);
    setupColorControllers(controlLayout);
    setupGridResolutionControllers(controlLayout);
    setupParticleSizeControllers(controlLayout);
    setupParticleDisplayControllers(controlLayout);
    setupSDFObjectControllers(controlLayout);
    controlLayout->addStretch(1);
    setupButtons(controlLayout);

    m_LightEditor = new PointLightEditor(nullptr, this);
    QWidget* mainControls = new QWidget;
    mainControls->setLayout(controlLayout);

    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->setTabPosition(QTabWidget::South);
    tabWidget->setTabShape(QTabWidget::Triangular);
    tabWidget->addTab(mainControls,  "Main Params");
    tabWidget->addTab(m_LightEditor, "Lights");

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);

    ////////////////////////////////////////////////////////////////////////////////
    setFixedWidth(300);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::loadTextures()
{
    ////////////////////////////////////////////////////////////////////////////////
    // sky textures
    int currentSkyTexID = m_cbSkyTexture->getComboBox()->currentIndex();
    m_cbSkyTexture->getComboBox()->clear();
    m_cbSkyTexture->getComboBox()->addItem("None");
    QStringList skyTexFolders = getTextureFolders("Sky");

    foreach(QString tex, skyTexFolders)
    {
        m_cbSkyTexture->getComboBox()->addItem(tex);
    }

    m_cbSkyTexture->getComboBox()->setCurrentIndex(currentSkyTexID > 0 ? currentSkyTexID : 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupTextureControllers(QBoxLayout* ctrLayout)
{
    ////////////////////////////////////////////////////////////////////////////////
    // sky textures
    m_cbSkyTexture = new EnhancedComboBox;
    ctrLayout->addWidget(m_cbSkyTexture->getGroupBox("Sky Texture"));

    ////////////////////////////////////////////////////////////////////////////////
    loadTextures();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupColorControllers(QBoxLayout* ctrLayout)
{
    m_msNegativeParticleMaterial = new MaterialSelector;
    m_msNegativeParticleMaterial->setCustomMaterial(CUSTOM_PARTICLE_MATERIAL_INSIDE);
    m_msNegativeParticleMaterial->setDefaultCustomMaterial(true);

    m_msPositiveParticleMaterial = new MaterialSelector;
    m_msPositiveParticleMaterial->setCustomMaterial(CUSTOM_PARTICLE_MATERIAL_OUTSIDE);
    m_msPositiveParticleMaterial->setDefaultCustomMaterial(true);

    QGridLayout* layoutMaterial = new QGridLayout;
    layoutMaterial->setContentsMargins(5, 5, 5, 5);

    int row = 0;
    layoutMaterial->addWidget(new QLabel("Negative: "), row, 0, 1, 1);
    layoutMaterial->addLayout(m_msNegativeParticleMaterial->getLayout(), row, 1, 1, 4);

    ++row;
    layoutMaterial->addWidget(new QLabel("Positive: "), row, 0, 1, 1);
    layoutMaterial->addLayout(m_msPositiveParticleMaterial->getLayout(), row, 1, 1, 4);

    QGroupBox* grMaterial = new QGroupBox;
    grMaterial->setTitle("Materials");
    grMaterial->setLayout(layoutMaterial);
    ctrLayout->addWidget(grMaterial);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupGridResolutionControllers(QBoxLayout* ctrLayout)
{
    m_cbResolution = new EnhancedComboBox;
    m_cbResolution->getComboBox()->addItems(GridResolutions);
    ctrLayout->addWidget(m_cbResolution->getGroupBox("Resolution"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupParticleSizeControllers(QBoxLayout* ctrLayout)
{
    m_slNegativeParticleSize = new QSlider(Qt::Horizontal);
    m_slNegativeParticleSize->setRange(1, 120);
    m_slNegativeParticleSize->setValue(70);

    m_slPositiveParticleSize = new QSlider(Qt::Horizontal);
    m_slPositiveParticleSize->setRange(1, 120);
    m_slPositiveParticleSize->setValue(10);

    QGridLayout* layoutParticleSize = new QGridLayout;
    layoutParticleSize->setContentsMargins(5, 5, 5, 5);

    int row = 0;
    layoutParticleSize->addWidget(new QLabel("Negative: "), row, 0, 1, 1);
    layoutParticleSize->addWidget(m_slNegativeParticleSize, row, 1, 1, 4);

    ++row;
    layoutParticleSize->addWidget(new QLabel("Positive: "), row, 0, 1, 1);
    layoutParticleSize->addWidget(m_slPositiveParticleSize, row, 1, 1, 4);

    QGroupBox* grParticleSize = new QGroupBox;
    grParticleSize->setTitle("Particle Size");
    grParticleSize->setLayout(layoutParticleSize);
    ctrLayout->addWidget(grParticleSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupParticleDisplayControllers(QBoxLayout* ctrLayout)
{
    m_chkHideNegativeParticles = new QCheckBox("Hide negative particles");
    m_chkHidePositiveParticles = new QCheckBox("Hide positive particles");

    QVBoxLayout* layoutParticleDisplay = new QVBoxLayout;
    layoutParticleDisplay->addWidget(m_chkHideNegativeParticles);
    layoutParticleDisplay->addWidget(m_chkHidePositiveParticles);

    QGroupBox* grParticleDisplay = new QGroupBox;
    grParticleDisplay->setTitle("Display");
    grParticleDisplay->setLayout(layoutParticleDisplay);
    ctrLayout->addWidget(grParticleDisplay);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupSDFObjectControllers(QBoxLayout* ctrLayout)
{
    m_cbSDFObject = new EnhancedComboBox;
    m_cbSDFObject->getComboBox()->addItems(SDFObjectNames);
    ctrLayout->addWidget(m_cbSDFObject->getGroupBox("SDF Objects"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupButtons(QBoxLayout* ctrLayout)
{
    m_btnEditClipPlane   = new QPushButton("Edit Clip Plane");
    m_btnEnableClipPlane = new QPushButton("Clip View");
    m_btnEnableClipPlane->setCheckable(true);

    ctrLayout->addWidget(m_btnEditClipPlane);
    ctrLayout->addWidget(m_btnEnableClipPlane);
}