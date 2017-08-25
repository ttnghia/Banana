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
    setupSimulationControllers(controlLayout);

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
    m_cbSkyTexture->getComboBox()->addItems(getTextureFolders("Sky"));

    m_cbSkyTexture->getComboBox()->setCurrentIndex(currentSkyTexID > 0 ? currentSkyTexID : 0);

    ////////////////////////////////////////////////////////////////////////////////
    // floor textures
    int currentFloorTexID = m_cbFloorTexture->getComboBox()->currentIndex();
    m_cbFloorTexture->getComboBox()->clear();
    m_cbFloorTexture->getComboBox()->addItem("None");
    m_cbFloorTexture->getComboBox()->addItems(getTextureFiles("Floor"));

    m_cbFloorTexture->getComboBox()->setCurrentIndex(currentFloorTexID > 0 ? currentFloorTexID : 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupSimulationControllers(QVBoxLayout* ctrLayout)
{
    m_cbSimulationScene = new QComboBox;
    m_cbSimulationScene->addItems(getSceneFiles());

    m_cbResolution = new QComboBox;
    int resolutionIdx = 0;
    for(int i = 8; i <= 256; i += 8)
    {
        if(i == 32)
            resolutionIdx = m_cbResolution->count();
        m_cbResolution->addItem(QString("%1").arg(i));
    }
    m_cbResolution->setCurrentIndex(resolutionIdx);

    m_txtStopTime = new QLineEdit;
    m_txtStopTime->setFixedWidth(100);
    m_txtStopTime->setText("5.0");

    ////////////////////////////////////////////////////////////////////////////////
    int          row                 = 0;
    QGridLayout* simControllerLayout = new QGridLayout;
    simControllerLayout->addWidget(new QLabel("Scene: "),        row, 0, 1, 2);
    simControllerLayout->addWidget(m_cbSimulationScene,          row, 2, 1, 1);

    ++row;
    simControllerLayout->addWidget(new QLabel("Resolution: "),   row, 0, 1, 2);
    simControllerLayout->addWidget(m_cbResolution,               row, 2, 1, 1);

    ++row;
    simControllerLayout->addWidget(new QLabel("Stop time(s): "), row, 0, 1, 2);
    simControllerLayout->addWidget(m_txtStopTime,                row, 2, 1, 1);

    QGroupBox* grpSimControllers = new QGroupBox;
    grpSimControllers->setTitle("Simulation Parameters");
    grpSimControllers->setLayout(simControllerLayout);

    ////////////////////////////////////////////////////////////////////////////////
    QString capturePath = QDir::currentPath() + QString("/Capture/");
    m_OutputPath = new BrowsePathWidget("Browse");
    m_OutputPath->setPath(capturePath);

    m_chkEnableOutput = new QCheckBox("Export to Images");

    QVBoxLayout* layoutOutput = new QVBoxLayout;
    layoutOutput->addWidget(m_chkEnableOutput);
    layoutOutput->addLayout(m_OutputPath->getLayout());

    QGroupBox* grpOutput = new QGroupBox;
    grpOutput->setTitle("Output");
    grpOutput->setLayout(layoutOutput);

    ////////////////////////////////////////////////////////////////////////////////
    m_btnStartStopSimulation = new QPushButton("Start");
    QGridLayout* btnSimControlLayout = new QGridLayout;
    btnSimControlLayout->addWidget(m_btnStartStopSimulation, 0, 0, 1, 1);

    ////////////////////////////////////////////////////////////////////////////////
    ctrLayout->addWidget(grpSimControllers);
    ctrLayout->addWidget(grpOutput);
    ctrLayout->addStretch();
    ctrLayout->addLayout(btnSimControlLayout);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupTextureControllers(QBoxLayout* ctrLayout)
{
    ////////////////////////////////////////////////////////////////////////////////
    // sky textures
    m_cbSkyTexture = new EnhancedComboBox;
    ctrLayout->addWidget(m_cbSkyTexture->getGroupBox("Sky Texture"));

    ////////////////////////////////////////////////////////////////////////////////
    // floor textures
    m_cbFloorTexture = new EnhancedComboBox;

    m_sldFloorSize = new EnhancedSlider;
    m_sldFloorSize->setRange(1, 100);
    m_sldFloorSize->getSlider()->setValue(DEFAULT_FLOOR_SIZE);

    m_sldFloorExposure = new EnhancedSlider;
    m_sldFloorExposure->setRange(1, 100);
    m_sldFloorExposure->getSlider()->setValue(100);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QVBoxLayout* floorTexLayout = new QVBoxLayout;
    floorTexLayout->addLayout(m_cbFloorTexture->getLayout());
    floorTexLayout->addSpacing(10);
    floorTexLayout->addWidget(line);
    floorTexLayout->addLayout(m_sldFloorSize->getLayoutWithLabel("Size:"));
    floorTexLayout->addLayout(m_sldFloorExposure->getLayoutWithLabel("Exposure:"));

    QGroupBox* floorTexGroup = new QGroupBox;
    floorTexGroup->setTitle("Floor Texture");
    floorTexGroup->setLayout(floorTexLayout);
    ctrLayout->addWidget(floorTexGroup);

    ////////////////////////////////////////////////////////////////////////////////
    loadTextures();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupColorControllers(QBoxLayout* ctrLayout)
{
    ////////////////////////////////////////////////////////////////////////////////
    /// color modes
    QRadioButton* rdbColorRandom  = new QRadioButton("Random");
    QRadioButton* rdbColorRamp    = new QRadioButton("Ramp");
    QRadioButton* rdbColorUniform = new QRadioButton("Uniform");

    rdbColorRandom->setChecked(true);

    QGridLayout* colorModeLayout = new QGridLayout;
    colorModeLayout->addWidget(rdbColorRandom,  0, 0);
    colorModeLayout->addWidget(rdbColorRamp,    0, 1);
    colorModeLayout->addWidget(rdbColorUniform, 1, 0);

    QGroupBox* colorModeGroup = new QGroupBox;
    colorModeGroup->setTitle(tr("Particle Color Mode"));
    colorModeGroup->setLayout(colorModeLayout);
    ctrLayout->addWidget(colorModeGroup);

    m_smParticleColorMode = new QSignalMapper(this);
    connect(rdbColorRandom,  SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorRamp,    SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorUniform, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));

    m_smParticleColorMode->setMapping(rdbColorRandom,  static_cast<int>(ParticleColorMode::Random));
    m_smParticleColorMode->setMapping(rdbColorRamp,    static_cast<int>(ParticleColorMode::Ramp));
    m_smParticleColorMode->setMapping(rdbColorUniform, static_cast<int>(ParticleColorMode::Uniform));

    m_msParticleMaterial = new MaterialSelector;
    m_msParticleMaterial->setCustomMaterial(CUSTOM_PARTICLE_MATERIAL);
    m_msParticleMaterial->setDefaultCustomMaterial(true);

    QGridLayout* particleColorLayout = new QGridLayout;
    m_msParticleMaterial->getComboBox()->setEnabled(false);
    connect(rdbColorUniform, &QRadioButton::clicked, [&](bool checked) { m_msParticleMaterial->getComboBox()->setEnabled(checked); });
    connect(rdbColorRandom,  &QRadioButton::clicked, [&](bool checked) { m_msParticleMaterial->getComboBox()->setEnabled(!checked); });
    connect(rdbColorRamp,    &QRadioButton::clicked, [&](bool checked) { m_msParticleMaterial->getComboBox()->setEnabled(!checked); });
    particleColorLayout->addLayout(m_msParticleMaterial->getLayout(), 0, 1, 1, 2);

    QGroupBox* particleColorGroup = new QGroupBox("Uniform Color");
    particleColorGroup->setLayout(particleColorLayout);
    ctrLayout->addWidget(particleColorGroup);
}
