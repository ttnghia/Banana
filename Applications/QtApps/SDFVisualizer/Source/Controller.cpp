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
    setupObjectTransformationControllers(controlLayout);
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
void Controller::setupObjectTransformationControllers(QBoxLayout* ctrLayout)
{
    m_txtTranslationX = new QLineEdit;
    m_txtTranslationY = new QLineEdit;
    m_txtTranslationZ = new QLineEdit;

    m_txtRotationAxisX = new QLineEdit;
    m_txtRotationAxisY = new QLineEdit;
    m_txtRotationAxisZ = new QLineEdit;
    m_txtRotationAngle = new QLineEdit;

    m_txtUniformScale = new QLineEdit;

    m_txtTranslationX->setText("0");
    m_txtTranslationY->setText("0");
    m_txtTranslationZ->setText("0");
    m_txtRotationAxisX->setText("1");
    m_txtRotationAxisY->setText("1");
    m_txtRotationAxisZ->setText("1");
    m_txtRotationAngle->setText("0");

    m_txtUniformScale->setText("1");

    ////////////////////////////////////////////////////////////////////////////////
    QHBoxLayout* loTranslation = new QHBoxLayout;
    loTranslation->addWidget(m_txtTranslationX);
    loTranslation->addWidget(m_txtTranslationY);
    loTranslation->addWidget(m_txtTranslationZ);
    QGroupBox* grTranslation = new QGroupBox;
    grTranslation->setTitle("Translation");
    grTranslation->setLayout(loTranslation);

    QGridLayout* loRotation = new QGridLayout;
    loRotation->addWidget(new QLabel("Axis: "), 0, 0, 1, 1);
    loRotation->addWidget(m_txtRotationAxisX, 0, 1, 1, 1);
    loRotation->addWidget(m_txtRotationAxisY, 0, 2, 1, 1);
    loRotation->addWidget(m_txtRotationAxisZ, 0, 3,  1, 1);
    loRotation->addWidget(new QLabel("Angle: "), 1, 0, 1, 1);
    loRotation->addWidget(m_txtRotationAngle, 1, 1,  1, 1);
    QGroupBox* grRotation = new QGroupBox;
    grRotation->setTitle("Rotation");
    grRotation->setLayout(loRotation);

    QHBoxLayout* loScale = new QHBoxLayout;
    loScale->addWidget(m_txtUniformScale);
    QGroupBox* grScale = new QGroupBox;
    grScale->setTitle("Uniform Scale");
    grScale->setLayout(loScale);

    ////////////////////////////////////////////////////////////////////////////////
    m_btnApplyTransform = new QPushButton("Apply");
    QHBoxLayout* loApplyButton = new QHBoxLayout;
    loApplyButton->addStretch(1);
    loApplyButton->addWidget(m_btnApplyTransform, 2);
    loApplyButton->addStretch(1);

    connect(m_btnApplyTransform, &QPushButton::clicked, this,
            [&]()
            {
                Vec3f translation = Vec3f(m_txtTranslationX->text().toFloat(),
                                          m_txtTranslationY->text().toFloat(),
                                          m_txtTranslationZ->text().toFloat());
                Vec4f rotation = Vec4f(m_txtRotationAxisX->text().toFloat(),
                                       m_txtRotationAxisY->text().toFloat(),
                                       m_txtRotationAxisZ->text().toFloat(),
                                       m_txtRotationAngle->text().toFloat());
                float scale = m_txtUniformScale->text().toFloat();

                emit transformationChanged(translation, rotation, scale);
            });

    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* loTransformation = new QVBoxLayout;
    loTransformation->addWidget(grTranslation);
    loTransformation->addWidget(grRotation);
    loTransformation->addWidget(grScale);
    loTransformation->addLayout(loApplyButton);
    QGroupBox* grTransformation = new QGroupBox;
    grTransformation->setTitle("Transformation");
    grTransformation->setLayout(loTransformation);

    ctrLayout->addWidget(grTransformation);
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