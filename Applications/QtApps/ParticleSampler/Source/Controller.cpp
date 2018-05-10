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
SamplingParameters Controller::getSamplingParams()
{
    SamplingParameters params;
    params.maxIters         = static_cast<UInt>(std::stoi(m_cbMaxIterations->getComboBox()->currentText().toStdString()));
    params.checkFrequency   = static_cast<UInt>(std::stoi(m_cbCheckFrequency->getComboBox()->currentText().toStdString()));
    params.deleteFrequency  = static_cast<UInt>(std::stoi(m_cbDeleteFrequency->getComboBox()->currentText().toStdString()));
    params.overlapThreshold = static_cast<float>(std::stof(m_txtOverlapThreshold->text().toStdString()));
    return params;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupGUI()
{
    setupMaterialControllers();
    setupColorModeControllers();
    setupSceneControllers();
    setupSamplingParametersControllers();
    setupCaptureControllers();
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
    connect(m_btnSaveObj,       &QPushButton::clicked, [&]() { m_ParticleSampler->saveParticles(ParticleOutputType::Obj, m_chkDoubleData->isChecked()); });
    connect(m_btnSaveBgeo,      &QPushButton::clicked, [&]() { m_ParticleSampler->saveParticles(ParticleOutputType::Bgeo, m_chkDoubleData->isChecked()); });
    connect(m_btnSaveBNN,       &QPushButton::clicked, [&]() { m_ParticleSampler->saveParticles(ParticleOutputType::Bnn, m_chkDoubleData->isChecked()); });
    connect(m_btnSaveBinary,    &QPushButton::clicked, [&]() { m_ParticleSampler->saveParticles(ParticleOutputType::Binary, m_chkDoubleData->isChecked()); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupSceneControllers()
{
    m_cbScene = new QComboBox;
    m_cbScene->addItem(QString("None"));
    m_cbScene->addItems(QtAppUtils::getFiles(QtAppUtils::getDefaultPath("Scenes")));
    m_btnReloadScene = new QPushButton(" Reload ");
    QHBoxLayout* layoutScene = new QHBoxLayout;
    layoutScene->addWidget(m_cbScene, 10);
    layoutScene->addStretch(1);
    layoutScene->addWidget(m_btnReloadScene, 10);
    QGroupBox* grScene = new QGroupBox;
    grScene->setTitle("Scene");
    grScene->setLayout(layoutScene);
    ////////////////////////////////////////////////////////////////////////////////
    m_LayoutRenderControllers->addWidget(grScene);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupSamplingParametersControllers()
{
    m_cbMaxIterations   = new EnhancedComboBox;
    m_cbCheckFrequency  = new EnhancedComboBox;
    m_cbDeleteFrequency = new EnhancedComboBox;
    for(int i = 1; i <= 10; ++i) {
        m_cbMaxIterations->getComboBox()->addItem(QString("%1").arg(i * 1000));
        m_cbCheckFrequency->getComboBox()->addItem(QString("%1").arg(i * 10));
        m_cbDeleteFrequency->getComboBox()->addItem(QString("%1").arg(i * 10));
    }
    m_cbMaxIterations->setCurrentIndex(9);
    m_cbDeleteFrequency->setCurrentIndex(4);

    m_txtOverlapThreshold = new QLineEdit;
    m_txtOverlapThreshold->setText("1.8");

    QGridLayout* layoutParameters = new QGridLayout;
    layoutParameters->addWidget(new QLabel("Max. iters:    "), 0, 0, 1, 1);
    layoutParameters->addLayout(m_cbMaxIterations->getLayout(), 0, 1, 1, 2);
    layoutParameters->addWidget(new QLabel("Check frequency:    "), 1, 0, 1, 1);
    layoutParameters->addLayout(m_cbCheckFrequency->getLayout(), 1, 1, 1, 2);
    layoutParameters->addWidget(new QLabel("Delete frequency:    "), 2, 0, 1, 1);
    layoutParameters->addLayout(m_cbDeleteFrequency->getLayout(), 2, 1, 1, 2);
    layoutParameters->addWidget(new QLabel("Overlap threshold:    "), 3, 0, 1, 1);
    layoutParameters->addWidget(m_txtOverlapThreshold,                3, 1, 1, 2);

    QGroupBox* grParameters = new QGroupBox;
    grParameters->setTitle("Sampling Parameters");
    grParameters->setLayout(layoutParameters);
    ////////////////////////////////////////////////////////////////////////////////
    m_LayoutRenderControllers->addWidget(grParameters);
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
    rdbColorRamp->setChecked(true);
    ////////////////////////////////////////////////////////////////////////////////
    QGridLayout* layoutColorMode = new QGridLayout;
    layoutColorMode->addWidget(rdbColorRandom,  0, 0, 1, 1);
    layoutColorMode->addWidget(rdbColorRamp,    0, 1, 1, 1);
    layoutColorMode->addWidget(rdbColorUniform, 1, 0, 1, 1);
    layoutColorMode->addWidget(rdbColorObjIdx,  1, 1, 1, 1);
    ////////////////////////////////////////////////////////////////////////////////
    m_smParticleColorMode = new QSignalMapper(this);
    connect(rdbColorRandom,  SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorRamp,    SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorUniform, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorObjIdx,  SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));

    m_smParticleColorMode->setMapping(rdbColorRandom,  static_cast<int>(ParticleColorMode::Random));
    m_smParticleColorMode->setMapping(rdbColorRamp,    static_cast<int>(ParticleColorMode::Ramp));
    m_smParticleColorMode->setMapping(rdbColorUniform, static_cast<int>(ParticleColorMode::UniformMaterial));
    m_smParticleColorMode->setMapping(rdbColorObjIdx,  static_cast<int>(ParticleColorMode::ObjectIndex));
    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* layoutColorCtrls = new QVBoxLayout;
    layoutColorCtrls->addLayout(layoutColorMode);
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
    m_btnStartStopRelaxation = new QPushButton("Start");
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
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    ////////////////////////////////////////////////////////////////////////////////
    QGridLayout* layoutButtons = new QGridLayout;
    layoutButtons->addWidget(m_btnStartStopRelaxation, 0, 0, 1, 2);
    layoutButtons->addWidget(m_btnResetCamera,         1, 0, 1, 2);
    layoutButtons->addWidget(m_btnClipViewPlane,       2, 0, 1, 1);
    layoutButtons->addWidget(m_btnEditClipPlane,       2, 1, 1, 1);
    layoutButtons->addWidget(line,                     3, 0, 1, 2);
    layoutButtons->addWidget(m_chkDoubleData,          4, 0, 1, 2);
    layoutButtons->addWidget(m_btnSaveObj,             5, 0, 1, 1);
    layoutButtons->addWidget(m_btnSaveBgeo,            5, 1, 1, 1);
    layoutButtons->addWidget(m_btnSaveBNN,             6, 0, 1, 1);
    layoutButtons->addWidget(m_btnSaveBinary,          6, 1, 1, 1);
    ////////////////////////////////////////////////////////////////////////////////
    m_MainLayout->addStretch();
    m_MainLayout->addLayout(layoutButtons);
}
