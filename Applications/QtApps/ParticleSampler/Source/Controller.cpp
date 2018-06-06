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

#include "Common.h"
#include "Controller.h"
#include <QtAppHelpers/QtAppUtils.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const ParticleTools::SPHRelaxationParameters<float>& Controller::getSamplingParams()
{
    m_RelaxParams.maxIters           = static_cast<UInt>(std::stoi(m_cbMaxIterations->getComboBox()->currentText().toStdString()));
    m_RelaxParams.checkFrequency     = static_cast<UInt>(std::stoi(m_cbCheckFrequency->getComboBox()->currentText().toStdString()));
    m_RelaxParams.deleteFrequency    = static_cast<UInt>(std::stoi(m_cbDeleteFrequency->getComboBox()->currentText().toStdString()));
    m_RelaxParams.intersectThreshold = std::stof(m_txtIntersectionThreshold->text().toStdString());

    m_RelaxParams.pressureStiffness     = std::stof(m_txtSPHPressureStiffness->text().toStdString());
    m_RelaxParams.viscosity             = std::stof(m_txtSPHViscosity->text().toStdString());
    m_RelaxParams.nearKernelRadius      = std::stof(m_txtSPHNearKernelRadiusRatio->text().toStdString());
    m_RelaxParams.nearPressureStiffness = std::stof(m_txtSPHNearPressureStiffness->text().toStdString());

    return m_RelaxParams;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupGUI()
{
    m_MainTab->setTabText(0, "Render Controllers");
    m_RelaxationControllers->setLayout(m_LayoutRelaxationControllers);
    m_MainTab->insertTab(0, m_RelaxationControllers, "Relaxation");
    m_MainTab->setCurrentIndex(0);
    ////////////////////////////////////////////////////////////////////////////////
    setupMaterialControllers();
    setupColorModeControllers();
    setupCaptureControllers();
    setupSceneControllers();
    setupSamplingParametersControllers();
    setupButtons();
    ////////////////////////////////////////////////////////////////////////////////
    m_LayoutMainControllers->addStretch();
    m_LayoutRelaxationControllers->addStretch();
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
    connect( m_msParticleMaterial, &MaterialSelector::materialChanged,                                m_RenderWidget, &RenderWidget::setParticleMaterial);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // scene
    connect(m_OutputPath, &BrowsePathWidget::pathChanged, m_RenderWidget, &RenderWidget::setCapturePath);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // buttons
    connect(  m_btnResetCamera, &QPushButton::clicked, m_RenderWidget, &RenderWidget::resetCameraPosition);
    connect(m_btnClipViewPlane, &QPushButton::clicked, m_RenderWidget, &RenderWidget::enableClipPlane);
    ////////////////////////////////////////////////////////////////////////////////
    connect(      m_btnSaveObj, &QPushButton::clicked, [&]() { m_ParticleSampler->saveParticles(ParticleOutputType::Obj, m_chkDoubleData->isChecked()); });
    connect(     m_btnSaveBgeo, &QPushButton::clicked, [&]() { m_ParticleSampler->saveParticles(ParticleOutputType::Bgeo, m_chkDoubleData->isChecked()); });
    connect(      m_btnSaveBNN, &QPushButton::clicked, [&]() { m_ParticleSampler->saveParticles(ParticleOutputType::Bnn, m_chkDoubleData->isChecked()); });
    connect(   m_btnSaveBinary, &QPushButton::clicked, [&]() { m_ParticleSampler->saveParticles(ParticleOutputType::Binary, m_chkDoubleData->isChecked()); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupMaterialControllers()
{
    m_msParticleMaterial = new MaterialSelector;
    m_msParticleMaterial->setCustomMaterial(CUSTOM_PARTICLE_MATERIAL);
    m_msParticleMaterial->setDefaultCustomMaterial(true);
    QGridLayout* layoutMaterial = new QGridLayout;
    layoutMaterial->addLayout(m_msParticleMaterial->getLayout(), 0, 0, 1, 2);
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grMaterial = new QGroupBox("Material");
    grMaterial->setLayout(layoutMaterial);
    m_LayoutMainControllers->addWidget(grMaterial);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupColorModeControllers()
{
    ////////////////////////////////////////////////////////////////////////////////
    QRadioButton* rdbColorRandom  = new QRadioButton("Random");
    QRadioButton* rdbColorRamp    = new QRadioButton("Ramp");
    QRadioButton* rdbColorUniform = new QRadioButton("Uniform Color");
    QRadioButton* rdbColorObjIdx  = new QRadioButton("Object Index");
    rdbColorRamp->setChecked(true);
    ////////////////////////////////////////////////////////////////////////////////
    QGridLayout* layoutColorMode = new QGridLayout;
    layoutColorMode->addWidget( rdbColorRandom, 0, 0, 1, 1);
    layoutColorMode->addWidget(   rdbColorRamp, 0, 1, 1, 1);
    layoutColorMode->addWidget(rdbColorUniform, 1, 0, 1, 1);
    layoutColorMode->addWidget( rdbColorObjIdx, 1, 1, 1, 1);
    ////////////////////////////////////////////////////////////////////////////////
    m_smParticleColorMode = new QSignalMapper(this);
    connect( rdbColorRandom, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(   rdbColorRamp, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorUniform, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect( rdbColorObjIdx, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));

    m_smParticleColorMode->setMapping( rdbColorRandom, static_cast<int>(ParticleColorMode::Random));
    m_smParticleColorMode->setMapping(   rdbColorRamp, static_cast<int>(ParticleColorMode::Ramp));
    m_smParticleColorMode->setMapping(rdbColorUniform, static_cast<int>(ParticleColorMode::UniformMaterial));
    m_smParticleColorMode->setMapping( rdbColorObjIdx, static_cast<int>(ParticleColorMode::ObjectIndex));
    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* layoutColorCtrls = new QVBoxLayout;
    layoutColorCtrls->addLayout(layoutColorMode);
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grColorMode = new QGroupBox;
    grColorMode->setTitle("Particle Color");
    grColorMode->setLayout(layoutColorCtrls);
    m_LayoutMainControllers->addWidget(grColorMode);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupCaptureControllers()
{
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
    m_LayoutMainControllers->addWidget(grpOutput);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupSceneControllers()
{
    m_cbScene = new EnhancedComboBox;
    m_cbScene->getComboBox()->addItem(QString("None"));
    m_cbScene->getComboBox()->addItems(QtAppUtils::getFiles(QtAppUtils::getDefaultPath("Scenes")));
    m_btnReloadScene = new QPushButton(" Reload ");
    QHBoxLayout* layoutScene = new QHBoxLayout;
    layoutScene->addLayout(m_cbScene->getLayout(), 10);
    layoutScene->addStretch(1);
    layoutScene->addWidget(m_btnReloadScene, 10);
    QGroupBox* grScene = new QGroupBox;
    grScene->setTitle("Scene");
    grScene->setLayout(layoutScene);
    ////////////////////////////////////////////////////////////////////////////////
    m_LayoutRelaxationControllers->addWidget(grScene);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupSamplingParametersControllers()
{
    m_cbMaxIterations          = new EnhancedComboBox;
    m_cbCheckFrequency         = new EnhancedComboBox;
    m_cbDeleteFrequency        = new EnhancedComboBox;
    m_txtIntersectionThreshold = new QLineEdit;

    for(int i = 1; i <= 10; ++i) {
        m_cbMaxIterations->getComboBox()->addItem(QString("%1").arg(i * 1000));
        m_cbCheckFrequency->getComboBox()->addItem(QString("%1").arg(i * 10));
        m_cbDeleteFrequency->getComboBox()->addItem(QString("%1").arg(i * 10));
    }
    m_cbMaxIterations->setCurrentIndex(9);
    m_cbDeleteFrequency->setCurrentIndex(4);
    m_txtIntersectionThreshold->setText("1.8");

    QGridLayout* layoutStopCriteria = new QGridLayout;
    int          row                = 0;
    layoutStopCriteria->addWidget(new QLabel("Max. iters:    "), row, 0, 1, 1);
    layoutStopCriteria->addLayout(m_cbMaxIterations->getLayout(), row++, 1, 1, 2);
    layoutStopCriteria->addWidget(new QLabel("Check frequency:    "), row, 0, 1, 1);
    layoutStopCriteria->addLayout(m_cbCheckFrequency->getLayout(), row++, 1, 1, 2);
    layoutStopCriteria->addWidget(new QLabel("Delete frequency:    "), row, 0, 1, 1);
    layoutStopCriteria->addLayout(m_cbDeleteFrequency->getLayout(), row++, 1, 1, 2);
    layoutStopCriteria->addWidget(                       new QLabel("Intersection threshold:    "), row,   0, 1, 1);
    layoutStopCriteria->addWidget(m_txtIntersectionThreshold,                                       row++, 1, 1, 2);

    QGroupBox* grStopCriteria = new QGroupBox;
    grStopCriteria->setTitle("Stop Criteria");
    grStopCriteria->setLayout(layoutStopCriteria);
    m_LayoutRelaxationControllers->addWidget(grStopCriteria);
    ////////////////////////////////////////////////////////////////////////////////
    m_txtSPHCFLFactor             = new QLineEdit;
    m_txtSPHMinTimestep           = new QLineEdit;
    m_txtSPHMaxTimestep           = new QLineEdit;
    m_txtSPHPressureStiffness     = new QLineEdit;
    m_txtSPHViscosity             = new QLineEdit;
    m_txtSPHNearKernelRadiusRatio = new QLineEdit;
    m_txtSPHNearPressureStiffness = new QLineEdit;
    m_chkNormalizeDensity         = new QCheckBox;

    m_txtSPHCFLFactor->setText("0.4");
    m_txtSPHMinTimestep->setText("0.000001");
    m_txtSPHMaxTimestep->setText("0.333333");
    m_txtSPHPressureStiffness->setText("50000");
    m_txtSPHViscosity->setText("0.01");
    m_txtSPHNearKernelRadiusRatio->setText("2.0");
    m_txtSPHNearPressureStiffness->setText("10000");

    QGridLayout* layoutSPHParameters = new QGridLayout;
    row = 0;
    layoutSPHParameters->addWidget(                          new QLabel("CFL factor:    "),               row,   0, 1, 2);
    layoutSPHParameters->addWidget(            m_txtSPHCFLFactor,                                         row++, 2, 1, 1);
    layoutSPHParameters->addWidget(                          new QLabel("Min. timestep:    "),            row,   0, 1, 2);
    layoutSPHParameters->addWidget(          m_txtSPHMinTimestep,                                         row++, 2, 1, 1);
    layoutSPHParameters->addWidget(                          new QLabel("Max. timestep:    "),            row,   0, 1, 2);
    layoutSPHParameters->addWidget(          m_txtSPHMaxTimestep,                                         row++, 2, 1, 1);
    layoutSPHParameters->addWidget(                          new QLabel("Pressure stiffness:    "),       row,   0, 1, 2);
    layoutSPHParameters->addWidget(    m_txtSPHPressureStiffness,                                         row++, 2, 1, 1);
    layoutSPHParameters->addWidget(                          new QLabel("Viscosity:    "),                row,   0, 1, 2);
    layoutSPHParameters->addWidget(            m_txtSPHViscosity,                                         row++, 2, 1, 1);
    layoutSPHParameters->addWidget(                          new QLabel("Near kernel radius ratio:    "), row,   0, 1, 2);
    layoutSPHParameters->addWidget(m_txtSPHNearKernelRadiusRatio,                                         row++, 2, 1, 1);
    layoutSPHParameters->addWidget(                          new QLabel("Near pressure stiffness:    "),  row,   0, 1, 2);
    layoutSPHParameters->addWidget(m_txtSPHNearPressureStiffness,                                         row++, 2, 1, 1);

    layoutSPHParameters->addLayout(QtAppUtils::getLayoutSpacing(), row++, 0, 1, 2);
    layoutSPHParameters->addWidget(                  new QLabel("Normalize density:    "), row,   0, 1, 2);
    layoutSPHParameters->addWidget(m_chkNormalizeDensity,                                  row++, 2, 1, 1);

    QGroupBox* grSPHParameters = new QGroupBox;
    grSPHParameters->setTitle("SPH Solver Parameters");
    grSPHParameters->setLayout(layoutSPHParameters);
    ////////////////////////////////////////////////////////////////////////////////
    m_LayoutRelaxationControllers->addWidget(grSPHParameters);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupButtons()
{
    ////////////////////////////////////////////////////////////////////////////////
    m_btnStartStopRelaxation = new QPushButton("Start Relaxation");
    m_btnResetCamera         = new QPushButton("Reset Camera");
    m_btnEditClipPlane       = new QPushButton("Edit Clip Plane");
    m_btnClipViewPlane       = new QPushButton("Clip View");
    m_btnClipViewPlane->setCheckable(true);

    m_chkDoubleData = new QCheckBox("Save data as double numbers");
    m_btnSaveObj    = new QPushButton("Save .obj");
    m_btnSaveBgeo   = new QPushButton("Sabe .bgeo");
    m_btnSaveBNN    = new QPushButton("Save .bnn");
    m_btnSaveBinary = new QPushButton("Save binary");
    ////////////////////////////////////////////////////////////////////////////////
    QGridLayout* layoutButtons = new QGridLayout;
    layoutButtons->addWidget(m_btnStartStopRelaxation,                     0, 0, 1, 2);
    layoutButtons->addWidget(        m_btnResetCamera,                     1, 0, 1, 2);
    layoutButtons->addWidget(      m_btnClipViewPlane,                     2, 0, 1, 1);
    layoutButtons->addWidget(      m_btnEditClipPlane,                     2, 1, 1, 1);
    layoutButtons->addWidget(              QtAppUtils::getLineSeparator(), 3, 0, 1, 2);
    layoutButtons->addWidget(         m_chkDoubleData,                     4, 0, 1, 2);
    layoutButtons->addWidget(            m_btnSaveObj,                     5, 0, 1, 1);
    layoutButtons->addWidget(           m_btnSaveBgeo,                     5, 1, 1, 1);
    layoutButtons->addWidget(            m_btnSaveBNN,                     6, 0, 1, 1);
    layoutButtons->addWidget(         m_btnSaveBinary,                     6, 1, 1, 1);
    ////////////////////////////////////////////////////////////////////////////////
    m_MainLayout->addStretch();
    m_MainLayout->addLayout(layoutButtons);
}
