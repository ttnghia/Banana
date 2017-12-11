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
    setupColorControllers();
    setupGridResolutionControllers();
    setupParticleSizeControllers();
    setupParticleDisplayControllers();
    setupObjectTransformationControllers();
    setupSDFObjectControllers();
    setupButtons();
}

void Controller::connectWidgets() {}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupColorControllers()
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
    m_LayoutRenderControllers->addWidget(grMaterial);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupGridResolutionControllers()
{
    m_cbResolution = new EnhancedComboBox;
    m_cbResolution->getComboBox()->addItems(GridResolutions);
    m_LayoutRenderControllers->addWidget(m_cbResolution->getGroupBox("Resolution"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupParticleSizeControllers()
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
    m_LayoutRenderControllers->addWidget(grParticleSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupParticleDisplayControllers()
{
    m_chkHideNegativeParticles = new QCheckBox("Hide negative particles");
    m_chkHidePositiveParticles = new QCheckBox("Hide positive particles");

    QVBoxLayout* layoutParticleDisplay = new QVBoxLayout;
    layoutParticleDisplay->addWidget(m_chkHideNegativeParticles);
    layoutParticleDisplay->addWidget(m_chkHidePositiveParticles);

    QGroupBox* grParticleDisplay = new QGroupBox;
    grParticleDisplay->setTitle("Display");
    grParticleDisplay->setLayout(layoutParticleDisplay);
    m_LayoutRenderControllers->addWidget(grParticleDisplay);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupObjectTransformationControllers()
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

    m_LayoutRenderControllers->addWidget(grTransformation);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupSDFObjectControllers()
{
    m_cbSDFObject = new EnhancedComboBox;
    m_cbSDFObject->getComboBox()->addItems(SDFObjectNames);
    m_LayoutRenderControllers->addWidget(m_cbSDFObject->getGroupBox("SDF Objects"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupButtons()
{
    m_btnEditClipPlane   = new QPushButton("Edit Clip Plane");
    m_btnEnableClipPlane = new QPushButton("Clip View");
    m_btnEnableClipPlane->setCheckable(true);

    m_MainLayout->addStretch();
    m_MainLayout->addWidget(m_btnEditClipPlane);
    m_MainLayout->addWidget(m_btnEnableClipPlane);
}