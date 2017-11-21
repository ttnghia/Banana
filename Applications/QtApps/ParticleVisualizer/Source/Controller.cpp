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
    QVBoxLayout* layoutCtr     = new QVBoxLayout;
    QVBoxLayout* layoutButtons = new QVBoxLayout;
    ////////////////////////////////////////////////////////////////////////////////
    setupTextureControllers(layoutCtr);
    setupFrameControllers(layoutCtr);
    setupAniKernel(layoutCtr);
    setupMaterialControllers(layoutCtr);
    setupColorModeControllers(layoutCtr);
    setupButtons(layoutButtons);
    ////////////////////////////////////////////////////////////////////////////////

    QWidget* mainControls = new QWidget;
    mainControls->setLayout(layoutCtr);

    m_LightEditor = new PointLightEditor(nullptr, this);

    ////////////////////////////////////////////////////////////////////////////////
    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->setTabPosition(QTabWidget::South);
    tabWidget->setTabShape(QTabWidget::Triangular);
    tabWidget->addTab(mainControls, "Main Controls");
    tabWidget->addTab(m_LightEditor, "Lights");

    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addStretch();
    mainLayout->addLayout(layoutButtons);
    setLayout(mainLayout);

    ////////////////////////////////////////////////////////////////////////////////
    setFixedWidth(350);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::connectWidgets()
{
    ////////////////////////////////////////////////////////////////////////////////
    // background and floor
    connect(m_cbSkyTexture->getComboBox(), SIGNAL(currentIndexChanged(int)), m_RenderWidget, SLOT(setSkyBoxTexture(int)));
    connect(m_pkrBackgroundColor, &ColorPicker::colorChanged, [&](float r, float g, float b) { m_RenderWidget->setClearColor(Vec3f(r, g, b)); });
    connect(m_cbFloorTexture->getComboBox(), SIGNAL(currentIndexChanged(int)), m_RenderWidget, SLOT(setFloorTexture(int)));
    connect(m_sldFloorSize->getSlider(), &QSlider::valueChanged, m_RenderWidget, &RenderWidget::setFloorSize);

    ////////////////////////////////////////////////////////////////////////////////
    // materials
    connect(m_msParticleMaterial, &MaterialSelector::materialChanged, m_RenderWidget, &RenderWidget::setParticleMaterial);
//    connect(m_msMeshMaterial, &MaterialSelector::materialChanged,
//            [&](const Material::MaterialData& material)
//            {
//                m_RenderWidget->setMeshMaterial(material, m_cbMeshMaterialID->currentIndex());
//            });

    ////////////////////////////////////////////////////////////////////////////////
    // ani kernel
    connect(m_chkUseAniKernel, &QCheckBox::toggled, m_RenderWidget, &RenderWidget::enableAniKernels);
    connect(m_chkUseAniKernel, &QCheckBox::toggled, m_DataReader, &DataReader::enableAniKernel);

    ////////////////////////////////////////////////////////////////////////////////
    // buttons
    connect(m_btnResetCamera, &QPushButton::clicked, m_RenderWidget, &RenderWidget::resetCameraPosition);
    connect(m_btnPause, &QPushButton::clicked, m_DataReader, &DataReader::pause);
    connect(m_btnNextFrame, &QPushButton::clicked, m_DataReader, &DataReader::readNextFrame);
    connect(m_btnReset, &QPushButton::clicked, m_DataReader, &DataReader::readFirstFrame);
    connect(m_btnRepeatPlay, &QPushButton::clicked, m_DataReader, &DataReader::enableRepeat);
    connect(m_btnReverse, &QPushButton::clicked, m_DataReader, &DataReader::enableReverse);
    connect(m_btnClipViewPlane, &QPushButton::clicked, m_RenderWidget, &RenderWidget::enableClipPlane);

    ////////////////////////////////////////////////////////////////////////////////
    //  data handle
    connect(m_DataReader, &DataReader::particleDataChanged, m_RenderWidget, &RenderWidget::updateData);

    ////////////////////////////////////////////////////////////////////////////////
    // lights
    connect(m_LightEditor, &PointLightEditor::lightsChanged, m_RenderWidget, &RenderWidget::updateLights);
    connect(m_RenderWidget, &RenderWidget::lightsObjChanged, m_LightEditor, &PointLightEditor::setLights);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QStringList Controller::getTextureFolders(QString texType)
{
    QDir dataDir(QDir::currentPath() + "/Textures/" + texType);
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);

    return dataDir.entryList();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QStringList Controller::getTextureFiles(QString texType)
{
    QDir dataDir(QDir::currentPath() + "/Textures/" + texType);
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);

    return dataDir.entryList();
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

    foreach(QString tex, skyTexFolders) {
        m_cbSkyTexture->getComboBox()->addItem(tex);
    }
    m_cbSkyTexture->getComboBox()->setCurrentIndex(currentSkyTexID > 0 ? currentSkyTexID : 0);

    ////////////////////////////////////////////////////////////////////////////////
    // floor textures
    int currentFloorTexID = m_cbFloorTexture->getComboBox()->currentIndex();
    m_cbFloorTexture->getComboBox()->clear();
    QStringList floorTexFolders = getTextureFiles("Floor");
    m_cbFloorTexture->getComboBox()->addItem("None");

    foreach(QString tex, floorTexFolders) {
        m_cbFloorTexture->getComboBox()->addItem(tex);
    }
    m_cbFloorTexture->getComboBox()->setCurrentIndex(currentFloorTexID > 0 ? currentFloorTexID : 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupTextureControllers(QBoxLayout* layoutCtr)
{
    m_cbSkyTexture       = new EnhancedComboBox;
    m_pkrBackgroundColor = new ColorPicker;
    m_pkrBackgroundColor->setMaximumHeight(25);
    ////////////////////////////////////////////////////////////////////////////////
    QGridLayout* layoutBackground = new QGridLayout;
    layoutBackground->addWidget(new QLabel("Skybox:"), 0, 0, 1, 1);
    layoutBackground->addLayout(m_cbSkyTexture->getLayout(), 0, 1, 1, 2);
    layoutBackground->addWidget(new QLabel("Background Color: "), 1, 0, 1, 1);
    layoutBackground->addWidget(m_pkrBackgroundColor, 1, 1, 1, 2);
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grBackground = new QGroupBox("Background");
    grBackground->setLayout(layoutBackground);
    layoutCtr->addWidget(grBackground);

    ////////////////////////////////////////////////////////////////////////////////
    m_cbFloorTexture = new EnhancedComboBox;
    m_sldFloorSize   = new EnhancedSlider;
    m_sldFloorSize->setRange(1, 100);
    m_sldFloorSize->getSlider()->setValue(10);
    m_sldFloorExposure = new EnhancedSlider;
    m_sldFloorExposure->setRange(1, 100);
    m_sldFloorExposure->getSlider()->setValue(50);
    ////////////////////////////////////////////////////////////////////////////////
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* shadowLayout = new QVBoxLayout;
    shadowLayout->addLayout(m_cbFloorTexture->getLayout());
    shadowLayout->addSpacing(10);
    shadowLayout->addWidget(line);
    shadowLayout->addLayout(m_sldFloorSize->getLayoutWithLabel("Size:"));
    shadowLayout->addLayout(m_sldFloorExposure->getLayoutWithLabel("Exposure:"));
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* shadowGroup = new QGroupBox;
    shadowGroup->setTitle("Floor Texture");
    shadowGroup->setLayout(shadowLayout);
    layoutCtr->addWidget(shadowGroup);
    ////////////////////////////////////////////////////////////////////////////////
    loadTextures();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupFrameControllers(QBoxLayout* layoutCtr)
{
    ////////////////////////////////////////////////////////////////////////////////
    m_sldFrameDelay = new EnhancedSlider;
    m_sldFrameDelay->setRange(0, 1000);
    m_sldFrameDelay->setTracking(false);
    m_sldFrameDelay->setValue(DEFAULT_DELAY_TIME);
    ///////////////////////////////////////////////////////////////////////////////
    m_sldFrameStep = new EnhancedSlider;
    m_sldFrameStep->setRange(1, 100);
    m_sldFrameStep->setTracking(false);
    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* layoutFrameCtr = new QVBoxLayout;
    layoutFrameCtr->addLayout(m_sldFrameDelay->getLayoutWithLabel("Delay:"));
    layoutFrameCtr->addLayout(m_sldFrameStep->getLayoutWithLabel("Step:"));
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grpFrameControl = new QGroupBox("Frame Controls");
    grpFrameControl->setLayout(layoutFrameCtr);
    layoutCtr->addWidget(grpFrameControl);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupAniKernel(QBoxLayout* layoutCtr)
{
    m_chkUseAniKernel = new QCheckBox("Enable anisotropic kernel");
    QVBoxLayout* layoutAniKernel = new QVBoxLayout;
    QGroupBox*   grAniKernel     = new QGroupBox("Anisotropic Kernel");
    ////////////////////////////////////////////////////////////////////////////////
    layoutAniKernel->addWidget(m_chkUseAniKernel);
    grAniKernel->setLayout(layoutAniKernel);
    layoutCtr->addWidget(grAniKernel);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupMaterialControllers(QBoxLayout* layoutCtr)
{
    m_msParticleMaterial = new MaterialSelector;
    m_msParticleMaterial->setCustomMaterial(CUSTOM_PARTICLE_MATERIAL);
    m_msParticleMaterial->setDefaultCustomMaterial(true);
    ////////////////////////////////////////////////////////////////////////////////
    m_msMeshMaterial = new MaterialSelector;
    m_msMeshMaterial->setMaterial(DEFAULT_MESH_MATERIAL);
    m_cbMeshMaterialID = new QComboBox;
    for(int i = 0; i < NUM_MESHES; ++i) {
        m_cbMeshMaterialID->addItem(QString("%1").arg(i));
    }
    ////////////////////////////////////////////////////////////////////////////////
    QHBoxLayout* layoutMeshMaterialSelector = new QHBoxLayout;
    layoutMeshMaterialSelector->addWidget(m_cbMeshMaterialID);
    layoutMeshMaterialSelector->addLayout(m_msMeshMaterial->getLayout());
    ////////////////////////////////////////////////////////////////////////////////
    QGridLayout* layoutMaterial = new QGridLayout;
    layoutMaterial->addWidget(new QLabel("Particle: "), 0, 0, Qt::AlignRight);
    layoutMaterial->addLayout(m_msParticleMaterial->getLayout(), 0, 1, 1, 2);
    layoutMaterial->addWidget(new QLabel("Mesh: "), 1, 0, Qt::AlignRight);
    layoutMaterial->addLayout(layoutMeshMaterialSelector, 1, 1, 1, 2);
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grMaterial = new QGroupBox("Material");
    grMaterial->setLayout(layoutMaterial);
    layoutCtr->addWidget(grMaterial);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupColorModeControllers(QBoxLayout* layoutCtr)
{
    ////////////////////////////////////////////////////////////////////////////////
    QRadioButton* rdbColorRandom  = new QRadioButton("Random");
    QRadioButton* rdbColorRamp    = new QRadioButton("Ramp");
    QRadioButton* rdbColorUniform = new QRadioButton("Uniform Material");
    QRadioButton* rdbColorData    = new QRadioButton("From Data");
    rdbColorRamp->setChecked(true);
    ////////////////////////////////////////////////////////////////////////////////
    QGridLayout* layoutColorMode = new QGridLayout;
    layoutColorMode->addWidget(rdbColorRandom, 0, 0, 1, 1);
    layoutColorMode->addWidget(rdbColorRamp, 0, 1, 1, 1);
    layoutColorMode->addWidget(rdbColorUniform, 1, 0, 1, 1);
    layoutColorMode->addWidget(rdbColorData, 1, 1, 1, 1);
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grColorMode = new QGroupBox;
    grColorMode->setTitle("Particle Color");
    grColorMode->setLayout(layoutColorMode);
    layoutCtr->addWidget(grColorMode);
    ////////////////////////////////////////////////////////////////////////////////
    m_smParticleColorMode = new QSignalMapper(this);
    connect(rdbColorRandom, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorRamp, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorUniform, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorData, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    ////////////////////////////////////////////////////////////////////////////////
    m_smParticleColorMode->setMapping(rdbColorRandom, static_cast<int>(ParticleColorMode::Random));
    m_smParticleColorMode->setMapping(rdbColorRamp, static_cast<int>(ParticleColorMode::Ramp));
    m_smParticleColorMode->setMapping(rdbColorUniform, static_cast<int>(ParticleColorMode::UniformMaterial));
    m_smParticleColorMode->setMapping(rdbColorData, static_cast<int>(ParticleColorMode::FromData));
    ////////////////////////////////////////////////////////////////////////////////
    m_lstParticleData = new QListWidget;;
    QVBoxLayout* layoutDataList = new QVBoxLayout;
    layoutDataList->addSpacing(10);
    layoutDataList->addWidget(m_lstParticleData);
    QGroupBox* grDataList = new QGroupBox;
    layoutColorMode->addWidget(grDataList, 2, 0, 1, 2);
    grDataList->setVisible(false);
    grDataList->setLayout(layoutDataList);
    connect(rdbColorData, &QRadioButton::toggled, grDataList, &QGroupBox::setVisible);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupButtons(QBoxLayout* layoutCtr)
{
    m_btnResetCamera = new QPushButton("Reset Camera");
    ////////////////////////////////////////////////////////////////////////////////
    QHBoxLayout* layoutBtnResetCamera = new QHBoxLayout;
    layoutBtnResetCamera->addWidget(m_btnResetCamera);
    layoutCtr->addLayout(layoutBtnResetCamera);
    ////////////////////////////////////////////////////////////////////////////////
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
    ////////////////////////////////////////////////////////////////////////////////
    QGridLayout* layoutButtons = new QGridLayout;
    layoutButtons->addWidget(m_btnPause, 0, 0, 1, 1);
    layoutButtons->addWidget(m_btnNextFrame, 0, 1, 1, 1);
    layoutButtons->addWidget(m_btnReset, 1, 0, 1, 1);
    layoutButtons->addWidget(m_btnRepeatPlay, 1, 1, 1, 1);
    layoutButtons->addWidget(m_btnReverse, 2, 0, 1, 1);
    layoutButtons->addWidget(m_btnClipViewPlane, 2, 1, 1, 1);
    layoutCtr->addLayout(layoutButtons);
}