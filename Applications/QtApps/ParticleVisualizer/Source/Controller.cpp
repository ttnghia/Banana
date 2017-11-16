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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupGUI()
{
    QVBoxLayout* controlLayout = new QVBoxLayout;
    QVBoxLayout* btnLayout     = new QVBoxLayout;

    setupTextureControllers(controlLayout);
    setupFrameControllers(controlLayout);
    setupColorControllers(controlLayout);
    setupShadowControllers(controlLayout);

    setupButtons(btnLayout);

    ////////////////////////////////////////////////////////////////////////////////
    m_LightEditor = new PointLightEditor(nullptr, this);
    QWidget* mainControls = new QWidget;
    mainControls->setLayout(controlLayout);

    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->setTabPosition(QTabWidget::South);
    tabWidget->setTabShape(QTabWidget::Triangular);
    tabWidget->addTab(mainControls, "Main Controls");
    tabWidget->addTab(m_LightEditor, "Lights");

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);

    ////////////////////////////////////////////////////////////////////////////////
    //    setLayout(controlLayout);
    setFixedWidth(350);
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

    ////////////////////////////////////////////////////////////////////////////////
    // floor textures
    int currentFloorTexID = m_cbFloorTexture->getComboBox()->currentIndex();
    m_cbFloorTexture->getComboBox()->clear();
    QStringList floorTexFolders = getTextureFiles("Floor");
    m_cbFloorTexture->getComboBox()->addItem("None");

    foreach(QString tex, floorTexFolders)
    {
        m_cbFloorTexture->getComboBox()->addItem(tex);
    }

    m_cbFloorTexture->getComboBox()->setCurrentIndex(currentFloorTexID > 0 ? currentFloorTexID : 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupTextureControllers(QBoxLayout* ctrLayout)
{
    ////////////////////////////////////////////////////////////////////////////////
    // background
    m_cbSkyTexture       = new EnhancedComboBox;
    m_pkrBackgroundColor = new ColorPicker;
    m_pkrBackgroundColor->setMaximumHeight(25);
    QGridLayout* layoutBackground = new QGridLayout;
    layoutBackground->addWidget(new QLabel("Skybox:"), 0, 0, 1, 1);
    layoutBackground->addLayout(m_cbSkyTexture->getLayout(), 0, 1, 1, 2);
    layoutBackground->addWidget(new QLabel("Background Color: "), 1, 0, 1, 1);
    layoutBackground->addWidget(m_pkrBackgroundColor, 1, 1, 1, 2);

    QGroupBox* grBackground = new QGroupBox("Background");
    grBackground->setLayout(layoutBackground);
    ctrLayout->addWidget(grBackground);


    ////////////////////////////////////////////////////////////////////////////////
    // floor textures
    m_cbFloorTexture = new EnhancedComboBox;

    m_sldFloorSize = new EnhancedSlider;
    m_sldFloorSize->setRange(1, 100);
    m_sldFloorSize->getSlider()->setValue(10);

    m_sldFloorExposure = new EnhancedSlider;
    m_sldFloorExposure->setRange(1, 100);
    m_sldFloorExposure->getSlider()->setValue(50);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QVBoxLayout* shadowLayout = new QVBoxLayout;
    shadowLayout->addLayout(m_cbFloorTexture->getLayout());
    shadowLayout->addSpacing(10);
    shadowLayout->addWidget(line);
    shadowLayout->addLayout(m_sldFloorSize->getLayoutWithLabel("Size:"));
    shadowLayout->addLayout(m_sldFloorExposure->getLayoutWithLabel("Exposure:"));

    QGroupBox* shadowGroup = new QGroupBox;
    shadowGroup->setTitle("Floor Texture");
    shadowGroup->setLayout(shadowLayout);
    ctrLayout->addWidget(shadowGroup);

    ////////////////////////////////////////////////////////////////////////////////
    loadTextures();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupFrameControllers(QBoxLayout* ctrLayout)
{
    ////////////////////////////////////////////////////////////////////////////////
    // delay time between frames
    m_sldFrameDelay = new EnhancedSlider;
    m_sldFrameDelay->setRange(0, 1000);
    m_sldFrameDelay->setTracking(false);
    m_sldFrameDelay->setValue(DEFAULT_DELAY_TIME);

    ///////////////////////////////////////////////////////////////////////////////
    // frame step
    m_sldFrameStep = new EnhancedSlider;
    m_sldFrameStep->setRange(1, 100);
    m_sldFrameStep->setTracking(false);

    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* frameControlLayout = new QVBoxLayout;
    frameControlLayout->addLayout(m_sldFrameDelay->getLayoutWithLabel("Delay:"));
    frameControlLayout->addLayout(m_sldFrameStep->getLayoutWithLabel("Step:"));
    QGroupBox* grpFrameControl = new QGroupBox("Frame Controls");
    grpFrameControl->setLayout(frameControlLayout);
    ctrLayout->addWidget(grpFrameControl);
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
    colorModeLayout->addWidget(rdbColorRandom, 0, 0);
    colorModeLayout->addWidget(rdbColorRamp, 0, 1);
    colorModeLayout->addWidget(rdbColorUniform, 1, 0);

    QGroupBox* colorModeGroup = new QGroupBox;
    colorModeGroup->setTitle(tr("Particle Color"));
    colorModeGroup->setLayout(colorModeLayout);
    ctrLayout->addWidget(colorModeGroup);

    m_smParticleColorMode = new QSignalMapper(this);
    connect(rdbColorRandom, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorRamp, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorUniform, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));

    m_smParticleColorMode->setMapping(rdbColorRandom, static_cast<int>(ParticleColorMode::Random));
    m_smParticleColorMode->setMapping(rdbColorRamp, static_cast<int>(ParticleColorMode::Ramp));
    m_smParticleColorMode->setMapping(rdbColorUniform, static_cast<int>(ParticleColorMode::Uniform));

    m_msParticleMaterial = new MaterialSelector;
    m_msParticleMaterial->setCustomMaterial(CUSTOM_PARTICLE_MATERIAL);
    m_msParticleMaterial->setDefaultCustomMaterial(true);

//    m_msMeshMaterial->setMaterial(DEFAULT_MESH_MATERIAL);
//    m_cbMeshMaterialID = new QComboBox;

//    for(int i = 0; i < MAX_NUM_MESHES; ++i) {
//        m_cbMeshMaterialID->addItem(QString("%1").arg(i));
//    }

//    QHBoxLayout* meshMaterialSelectorLayout = new QHBoxLayout;
//    meshMaterialSelectorLayout->addWidget(m_cbMeshMaterialID);
//    meshMaterialSelectorLayout->addLayout(m_msMeshMaterial->getLayout());

    QGridLayout* particleColorLayout = new QGridLayout;
    particleColorLayout->addWidget(new QLabel("Sphere View: "), 0, 0, Qt::AlignRight);
    particleColorLayout->addLayout(m_msParticleMaterial->getLayout(), 0, 1, 1, 2);
    particleColorLayout->addWidget(new QLabel("Mesh: "), 1, 0, Qt::AlignRight);
//    particleColorLayout->addLayout(meshMaterialSelectorLayout, 1, 1, 1, 2);

    QGroupBox* particleColorGroup = new QGroupBox("Color");
    particleColorGroup->setLayout(particleColorLayout);
    ctrLayout->addWidget(particleColorGroup);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupShadowControllers(QBoxLayout* ctrLayout)
{
    ////////////////////////////////////////////////////////////////////////////////
    /// show shadow
    m_chkRenderShadow    = new QCheckBox("Render Shadow");
    m_sldShadowIntensity = new EnhancedSlider;
    m_sldShadowIntensity->getSlider()->setValue(100);
    m_sldShadowIntensity->setEnabled(false);
    connect(m_chkRenderShadow, &QCheckBox::toggled, m_sldShadowIntensity, &EnhancedSlider::setEnabled);

    m_chkVisualizeShadowRegion = new QCheckBox("Hightlight Shadow FOV");
    m_chkVisualizeShadowRegion->setEnabled(false);
    connect(m_chkRenderShadow, &QCheckBox::toggled, m_chkVisualizeShadowRegion, &QCheckBox::setEnabled);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QVBoxLayout* shadowLayout = new QVBoxLayout;
    shadowLayout->addWidget(m_chkRenderShadow);
    shadowLayout->addWidget(m_chkVisualizeShadowRegion);
    shadowLayout->addSpacing(10);
    shadowLayout->addWidget(line);
    shadowLayout->addLayout(m_sldShadowIntensity->getLayoutWithLabel("Intensity:"));

    QGroupBox* shadowGroup = new QGroupBox;
    shadowGroup->setTitle("Render Shadow");
    shadowGroup->setLayout(shadowLayout);
    ctrLayout->addWidget(shadowGroup);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupButtons(QBoxLayout* ctrLayout)
{
    m_btnReloadTextures = new QPushButton("Reload Textures");
    m_btnResetCamera    = new QPushButton("Reset Camera");

    QHBoxLayout* btnReloadLayout = new QHBoxLayout;
    btnReloadLayout->addWidget(m_btnReloadTextures);
    btnReloadLayout->addWidget(m_btnResetCamera);
    ctrLayout->addLayout(btnReloadLayout);

    ////////////////////////////////////////////////////////////////////////////////
    /// other buttons
    m_btnPause = new QPushButton(QString("Pause"));
    m_btnPause->setCheckable(true);
    m_btnNextFrame = new QPushButton(QString("Next Frame"));
    m_btnReset     = new QPushButton(QString("Reset"));
    m_btnReverse   = new QPushButton(QString("Reverse"));
    m_btnReverse->setCheckable(true);
    m_btnRepeatPlay = new QPushButton(QString("Repeat"));
    m_btnRepeatPlay->setCheckable(true);
    m_btnClipViewPlane = new QPushButton(QString("Clip YZ Plane"));
    m_btnClipViewPlane->setCheckable(true);

    QGridLayout* btnLayout = new QGridLayout;
    btnLayout->addWidget(m_btnPause, 0, 0, 1, 1);
    btnLayout->addWidget(m_btnNextFrame, 0, 1, 1, 1);
    btnLayout->addWidget(m_btnReset, 1, 0, 1, 1);
    btnLayout->addWidget(m_btnRepeatPlay, 1, 1, 1, 1);
    btnLayout->addWidget(m_btnReverse, 2, 0, 1, 1);
    btnLayout->addWidget(m_btnClipViewPlane, 2, 1, 1, 1);
    ctrLayout->addLayout(btnLayout);
}