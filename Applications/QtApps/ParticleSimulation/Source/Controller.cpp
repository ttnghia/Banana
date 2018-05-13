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

#include "Common.h"
#include "Controller.h"
#include <QtAppHelpers/QtAppUtils.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupGUI()
{
    setupMaterialControllers();
    setupColorModeControllers();
    setupSimulationControllers();
    setupButtons();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::connectWidgets()
{
    connect(m_smBackgroundMode, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), [&](int backgroundMode)
            {
                m_chkRenderBox->setChecked(backgroundMode == OpenGLWidget::BackgroundMode::SkyBox || backgroundMode == OpenGLWidget::BackgroundMode::Color);
            });

    ////////////////////////////////////////////////////////////////////////////////
    // materials and particle color mode
    connect(m_smParticleColorMode, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), m_RenderWidget, &RenderWidget::setParticleColorMode);
    connect(m_smParticleColorMode, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), [&](int colorMode)
            {
                m_msParticleMaterial->getComboBox()->setEnabled(colorMode == ParticleColorMode::UniformMaterial);
            });
    connect(m_msParticleMaterial, &MaterialSelector::materialChanged, m_RenderWidget, &RenderWidget::setParticleMaterial);
    connect(m_pkrColorDataMin,    &ColorPicker::colorChanged,         [&](float r, float g, float b) { m_RenderWidget->setColorDataMin(Vec3f(r, g, b)); });
    connect(m_pkrColorDataMax,    &ColorPicker::colorChanged,         [&](float r, float g, float b) { m_RenderWidget->setColorDataMax(Vec3f(r, g, b)); });
    connect(m_btnRndColor,        &QPushButton::clicked,              [&]()
            {
                auto colorMin = Vec3f(NumberHelpers::generateRandomReal(0.0f, 1.0f),
                                      NumberHelpers::generateRandomReal(0.0f, 1.0f),
                                      NumberHelpers::generateRandomReal(0.0f, 1.0f));
                auto colorMax = Vec3f(NumberHelpers::generateRandomReal(0.0f, 1.0f),
                                      NumberHelpers::generateRandomReal(0.0f, 1.0f),
                                      NumberHelpers::generateRandomReal(0.0f, 1.0f));
                m_pkrColorDataMin->setColor(colorMin);
                m_pkrColorDataMax->setColor(colorMax);
                m_RenderWidget->setColorDataMin(colorMin);
                m_RenderWidget->setColorDataMax(colorMax);
            });
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // scene
    connect(m_OutputPath, &BrowsePathWidget::pathChanged, m_RenderWidget, &RenderWidget::setCapturePath);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // buttons
    connect(m_btnResetCamera,   &QPushButton::clicked, m_RenderWidget, &RenderWidget::resetCameraPosition);
    connect(m_btnClipViewPlane, &QPushButton::clicked, m_RenderWidget, &RenderWidget::enableClipPlane);
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupSimulationControllers()
{
    m_cbSimulationScene = new QComboBox;
    m_cbSimulationScene->addItem(QString("None"));
    m_cbSimulationScene->addItems(QtAppUtils::getFiles(QtAppUtils::getDefaultPath("Scenes")));
    m_btnReloadScene = new QPushButton(" Reload ");
    QHBoxLayout* layoutScene = new QHBoxLayout;
    layoutScene->addWidget(m_cbSimulationScene, 10);
    layoutScene->addStretch(1);
    layoutScene->addWidget(m_btnReloadScene, 10);
    QGroupBox* grScene = new QGroupBox;
    grScene->setTitle("Scene");
    grScene->setLayout(layoutScene);
    ////////////////////////////////////////////////////////////////////////////////
    m_OutputPath = new BrowsePathWidget("Browse");
    m_OutputPath->setPath(QtAppUtils::getDefaultCapturePath());
    m_chkEnableOutput = new QCheckBox("Export to Images");
    QVBoxLayout* layoutOutput = new QVBoxLayout;
    layoutOutput->addWidget(m_chkEnableOutput);
    layoutOutput->addLayout(m_OutputPath->getLayout());
    QGroupBox* grpOutput = new QGroupBox;
    grpOutput->setTitle("Screenshot");
    grpOutput->setLayout(layoutOutput);
    ////////////////////////////////////////////////////////////////////////////////
    m_LayoutRenderControllers->addWidget(grScene);
    m_LayoutRenderControllers->addWidget(grpOutput);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupMaterialControllers()
{
    m_msParticleMaterial = new MaterialSelector;
    m_msParticleMaterial->setCustomMaterial(CUSTOM_PARTICLE_MATERIAL);
    m_msParticleMaterial->setDefaultCustomMaterial(true);
    m_msParticleMaterial->getComboBox()->setEnabled(false);
    QGridLayout* layoutMaterial = new QGridLayout;
    layoutMaterial->addLayout(m_msParticleMaterial->getLayout(), 0, 0, 1, 2);
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grMaterial = new QGroupBox("Material");
    grMaterial->setLayout(layoutMaterial);
    m_LayoutRenderControllers->addWidget(grMaterial);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupColorModeControllers()
{
    ////////////////////////////////////////////////////////////////////////////////
    QRadioButton* rdbColorRandom  = new QRadioButton("Random");
    QRadioButton* rdbColorRamp    = new QRadioButton("Ramp");
    QRadioButton* rdbColorUniform = new QRadioButton("Uniform Material");
    QRadioButton* rdbColorObjIdx  = new QRadioButton("Object Index");
    QRadioButton* rdbColorVelMag  = new QRadioButton("Velocity Magnitude");
    rdbColorRamp->setChecked(true);
    ////////////////////////////////////////////////////////////////////////////////
    QGridLayout* layoutColorMode = new QGridLayout;
    layoutColorMode->addWidget(rdbColorRandom,  0, 0, 1, 1);
    layoutColorMode->addWidget(rdbColorRamp,    0, 1, 1, 1);
    layoutColorMode->addWidget(rdbColorUniform, 1, 0, 1, 1);
    layoutColorMode->addWidget(rdbColorObjIdx,  1, 1, 1, 1);
    layoutColorMode->addWidget(rdbColorVelMag,  2, 0, 1, 1);
    ////////////////////////////////////////////////////////////////////////////////
    m_smParticleColorMode = new QSignalMapper(this);
    connect(rdbColorRandom,  SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorRamp,    SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorUniform, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorObjIdx,  SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorVelMag,  SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));

    m_smParticleColorMode->setMapping(rdbColorRandom,  static_cast<int>(ParticleColorMode::Random));
    m_smParticleColorMode->setMapping(rdbColorRamp,    static_cast<int>(ParticleColorMode::Ramp));
    m_smParticleColorMode->setMapping(rdbColorUniform, static_cast<int>(ParticleColorMode::UniformMaterial));
    m_smParticleColorMode->setMapping(rdbColorObjIdx,  static_cast<int>(ParticleColorMode::ObjectIndex));
    m_smParticleColorMode->setMapping(rdbColorVelMag,  static_cast<int>(ParticleColorMode::VelocityMagnitude));
    ////////////////////////////////////////////////////////////////////////////////
    m_pkrColorDataMin = new ColorPicker;
    m_pkrColorDataMax = new ColorPicker;
    m_pkrColorDataMin->setColor(DEFAULT_COLOR_DATA_MIN);
    m_pkrColorDataMax->setColor(DEFAULT_COLOR_DATA_MAX);
    m_btnRndColor = new QPushButton("Rnd Color");
    QHBoxLayout* layoutColorData = new QHBoxLayout;
    layoutColorData->addWidget(new QLabel("Color min/max:"), 10);
    layoutColorData->addStretch(1);
    layoutColorData->addWidget(m_pkrColorDataMin, 10);
    layoutColorData->addStretch(1);
    layoutColorData->addWidget(m_pkrColorDataMax, 10);
    layoutColorData->addStretch(1);
    layoutColorData->addWidget(m_btnRndColor, 10);
    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* layoutColorCtrls = new QVBoxLayout;
    layoutColorCtrls->addLayout(layoutColorMode);
    layoutColorCtrls->addSpacing(5);
    layoutColorCtrls->addWidget(QtAppUtils::getLineSeparator());
    layoutColorCtrls->addSpacing(5);
    layoutColorCtrls->addLayout(layoutColorData);
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grColorMode = new QGroupBox;
    grColorMode->setTitle("Particle Color");
    grColorMode->setLayout(layoutColorCtrls);
    m_LayoutRenderControllers->addWidget(grColorMode);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupButtons()
{
    ////////////////////////////////////////////////////////////////////////////////
    m_btnStartStopSimulation = new QPushButton("Start");
    m_btnResetCamera         = new QPushButton("Reset Camera");
    m_btnEditClipPlane       = new QPushButton("Edit Clip Plane");
    m_btnClipViewPlane       = new QPushButton("Clip View");
    m_btnClipViewPlane->setCheckable(true);
    ////////////////////////////////////////////////////////////////////////////////
    QGridLayout* layoutButtons = new QGridLayout;
    layoutButtons->addWidget(m_btnStartStopSimulation, 0, 0, 1, 1);
    layoutButtons->addWidget(m_btnResetCamera,         1, 0, 1, 1);
    layoutButtons->addWidget(m_btnClipViewPlane,       2, 0, 1, 1);
    layoutButtons->addWidget(m_btnEditClipPlane,       3, 0, 1, 1);
    m_MainLayout->addStretch();
    m_MainLayout->addLayout(layoutButtons);
}
