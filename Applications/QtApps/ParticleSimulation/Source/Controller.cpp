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
    setupBackgroundControllers(layoutCtr);
    setupFloorControllers(layoutCtr);
    setupMaterialControllers(layoutCtr);
    setupColorModeControllers(layoutCtr);
    setupMiscControllers(layoutCtr);
    setupSimulationControllers(layoutCtr);
    layoutCtr->addStretch();
    setupButtons(layoutButtons);
    ////////////////////////////////////////////////////////////////////////////////
    loadTextures();
    m_LightEditor = new PointLightEditor(nullptr, this);

    QWidget* mainControls = new QWidget;
    mainControls->setLayout(layoutCtr);
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
    setFixedWidth(300);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::connectWidgets()
{
    ////////////////////////////////////////////////////////////////////////////////
    // background mode
    connect(m_smBackgroundMode, SIGNAL(mapped(int)), m_RenderWidget, SLOT(setBackgroundMode(int)));
    connect(m_smBackgroundMode, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), [&](int backgroundMode)
            {
                m_chkRenderBox->setChecked(backgroundMode == BackgroundMode::SkyBox || backgroundMode == BackgroundMode::Color);
            });
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // sky box
    connect(m_cbSkyTexture->getComboBox(), SIGNAL(currentIndexChanged(int)), m_RenderWidget, SLOT(setSkyBoxTexture(int)));
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // background color
    connect(m_pkrBackgroundColor, &ColorPicker::colorChanged, [&](float r, float g, float b) { m_RenderWidget->setClearColor(Vec3f(r, g, b)); });
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // checkerboard background
    connect(m_pkrCheckerColor1, &ColorPicker::colorChanged, [&](float r, float g, float b) { m_RenderWidget->setCheckerboarrdColor1(Vec3f(r, g, b)); });
    connect(m_pkrCheckerColor2, &ColorPicker::colorChanged, [&](float r, float g, float b) { m_RenderWidget->setCheckerboarrdColor2(Vec3f(r, g, b)); });
    connect(m_sldCheckerboardScale->getSlider(), &QSlider::valueChanged, [&](int value) { m_RenderWidget->setCheckerboarrdScales(Vec2i(value)); });
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // grid background
    connect(m_pkrGridBackgroundColor, &ColorPicker::colorChanged, [&](float r, float g, float b) { m_RenderWidget->setGridBackgroundColor(Vec3f(r, g, b)); });
    connect(m_pkrGridLineColor, &ColorPicker::colorChanged, [&](float r, float g, float b) { m_RenderWidget->setGridLineColor(Vec3f(r, g, b)); });
    connect(m_sldGridScale->getSlider(), &QSlider::valueChanged, [&](int value) { m_RenderWidget->setGridScales(Vec2i(value)); });
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // floor
    connect(m_cbFloorTexture->getComboBox(), SIGNAL(currentIndexChanged(int)), m_RenderWidget, SLOT(setFloorTexture(int)));
    connect(m_sldFloorSize->getSlider(), &QSlider::valueChanged, m_RenderWidget, &RenderWidget::setFloorSize);
    connect(m_sldFloorExposure->getSlider(), &QSlider::valueChanged, m_RenderWidget, &RenderWidget::setFloorExposure);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // materials and particle color mode
    connect(m_smParticleColorMode, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), m_RenderWidget, &RenderWidget::setParticleColorMode);
    connect(m_smParticleColorMode, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), [&](int colorMode)
            {
                m_msParticleMaterial->getComboBox()->setEnabled(colorMode == ParticleColorMode::UniformMaterial);
            });
    connect(m_msParticleMaterial, &MaterialSelector::materialChanged, m_RenderWidget, &RenderWidget::setParticleMaterial);
    connect(m_pkrColorDataMin, &ColorPicker::colorChanged, [&](float r, float g, float b) { m_RenderWidget->setColorDataMin(Vec3f(r, g, b)); });
    connect(m_pkrColorDataMax, &ColorPicker::colorChanged, [&](float r, float g, float b) { m_RenderWidget->setColorDataMax(Vec3f(r, g, b)); });
    connect(m_btnRndColor, &QPushButton::clicked, [&]
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
    // misc controllers
    connect(m_chkUseAniKernel, &QCheckBox::toggled, m_RenderWidget, &RenderWidget::enableAniKernels);
    connect(m_chkRenderBox, &QCheckBox::toggled, m_RenderWidget, &RenderWidget::setRenderBox);
    connect(m_pkrBoxColor, &ColorPicker::colorChanged, [&](float r, float g, float b) { m_RenderWidget->setBoxColor(Vec3f(r, g, b)); });
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // scene
    connect(m_OutputPath, &BrowsePathWidget::pathChanged, m_RenderWidget, &RenderWidget::setCapturePath);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // buttons
    connect(m_btnResetCamera, &QPushButton::clicked, m_RenderWidget, &RenderWidget::resetCameraPosition);
    connect(m_btnClipViewPlane, &QPushButton::clicked, m_RenderWidget, &RenderWidget::enableClipPlane);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // lights
    connect(m_LightEditor, &PointLightEditor::lightsChanged, m_RenderWidget, &RenderWidget::updateLights);
    connect(m_RenderWidget, &RenderWidget::lightsObjChanged, m_LightEditor, &PointLightEditor::setLights);
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QStringList Controller::getTextureFolders(const QString& texType, const QString& texPath)
{
    QDir dataDir(texPath + "/" + texType);
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    return dataDir.entryList();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QStringList Controller::getTextureFiles(const QString& texType, const QString& texPath)
{
    QDir dataDir(texPath + "/" + texType);
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    return dataDir.entryList();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::loadTextures()
{
    auto texPath = getTexturePath();
    ////////////////////////////////////////////////////////////////////////////////
    // sky textures
    int currentSkyTexID = m_cbSkyTexture->getComboBox()->currentIndex();
    m_cbSkyTexture->getComboBox()->clear();
    m_cbSkyTexture->getComboBox()->addItem("None");
    m_cbSkyTexture->getComboBox()->addItems(getTextureFolders("Sky", texPath));
    m_cbSkyTexture->getComboBox()->setCurrentIndex(currentSkyTexID > 0 ? currentSkyTexID : 0);

    ////////////////////////////////////////////////////////////////////////////////
    // floor textures
    int currentFloorTexID = m_cbFloorTexture->getComboBox()->currentIndex();
    m_cbFloorTexture->getComboBox()->clear();
    m_cbFloorTexture->getComboBox()->addItem("None");
    m_cbFloorTexture->getComboBox()->addItems(getTextureFiles("Floor", texPath));
    m_cbFloorTexture->getComboBox()->setCurrentIndex(currentFloorTexID > 0 ? currentFloorTexID : 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupBackgroundControllers(QBoxLayout* layoutCtr)
{
    QRadioButton* rdbBackgroundSkyBox       = new QRadioButton("SkyBox");
    QRadioButton* rdbBackgroundColor        = new QRadioButton("Color");
    QRadioButton* rdbBackgroundCheckerboard = new QRadioButton("Checkerboard");
    QRadioButton* rdbBackgroundGrid         = new QRadioButton("Grid");
    rdbBackgroundSkyBox->setChecked(true);

    QGridLayout* layoutBackgroundType = new QGridLayout;
    layoutBackgroundType->addWidget(rdbBackgroundSkyBox, 0, 0, 1, 1);
    layoutBackgroundType->addWidget(rdbBackgroundColor, 0, 1, 1, 1);
    layoutBackgroundType->addWidget(rdbBackgroundCheckerboard, 1, 0, 1, 1);
    layoutBackgroundType->addWidget(rdbBackgroundGrid, 1, 1, 1, 1);
    ////////////////////////////////////////////////////////////////////////////////
    m_cbSkyTexture = new EnhancedComboBox;
    QWidget* wSkyTex = new QWidget;
    m_cbSkyTexture->getLayout()->setContentsMargins(0, 0, 0, 0);
    wSkyTex->setLayout(m_cbSkyTexture->getLayout());
    wSkyTex->setMinimumHeight(25);
    ////////////////////////////////////////////////////////////////////////////////
    m_pkrBackgroundColor = new ColorPicker;
    m_pkrBackgroundColor->setMaximumHeight(25);
    m_pkrBackgroundColor->setVisible(false);
    ////////////////////////////////////////////////////////////////////////////////
    m_pkrCheckerColor1     = new ColorPicker;
    m_pkrCheckerColor2     = new ColorPicker;
    m_sldCheckerboardScale = new EnhancedSlider;
    m_sldCheckerboardScale->setRange(2, 100);
    m_sldCheckerboardScale->setValue(DEFAULT_CHECKERBOARD_GRID_SIZE);
    m_pkrCheckerColor1->setColor(DEFAULT_CHECKERBOARD_COLOR1);
    m_pkrCheckerColor2->setColor(DEFAULT_CHECKERBOARD_COLOR2);

    QHBoxLayout* layoutCheckerboard = new QHBoxLayout;
    layoutCheckerboard->setContentsMargins(0, 0, 0, 0);
    layoutCheckerboard->addWidget(m_pkrCheckerColor1, 10);
    layoutCheckerboard->addStretch(1);
    layoutCheckerboard->addWidget(m_pkrCheckerColor2, 10);
    layoutCheckerboard->addStretch(1);
    layoutCheckerboard->addLayout(m_sldCheckerboardScale->getLayout(), 40);

    QWidget* wCheckerboard = new QWidget;
    wCheckerboard->setMinimumHeight(25);
    wCheckerboard->setLayout(layoutCheckerboard);
    wCheckerboard->setVisible(false);
    ////////////////////////////////////////////////////////////////////////////////
    m_pkrGridBackgroundColor = new ColorPicker;
    m_pkrGridLineColor       = new ColorPicker;
    m_pkrGridBackgroundColor->setColor(DEFAULT_CHECKERBOARD_COLOR1);
    m_pkrGridLineColor->setColor(DEFAULT_CHECKERBOARD_COLOR2);
    m_sldGridScale = new EnhancedSlider;
    m_sldGridScale->setRange(2, 100);
    m_sldGridScale->setValue(DEFAULT_CHECKERBOARD_GRID_SIZE);

    QHBoxLayout* layoutGrid = new QHBoxLayout;
    layoutGrid->setContentsMargins(0, 0, 0, 0);
    layoutGrid->addWidget(m_pkrGridBackgroundColor, 10);
    layoutGrid->addStretch(1);
    layoutGrid->addWidget(m_pkrGridLineColor, 10);
    layoutGrid->addStretch(1);
    layoutGrid->addLayout(m_sldGridScale->getLayout(), 40);

    QWidget* wGrid = new QWidget;
    wGrid->setLayout(layoutGrid);
    wGrid->setMinimumHeight(25);
    wGrid->setVisible(false);
    ////////////////////////////////////////////////////////////////////////////////
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* layoutBackground = new QVBoxLayout;
    layoutBackground->addLayout(layoutBackgroundType);
    layoutBackground->addSpacing(5);
    layoutBackground->addWidget(line);
    layoutBackground->addSpacing(5);
    layoutBackground->addWidget(wSkyTex);
    layoutBackground->addWidget(m_pkrBackgroundColor);
    layoutBackground->addWidget(wCheckerboard);
    layoutBackground->addWidget(wGrid);

    connect(rdbBackgroundSkyBox, &QRadioButton::toggled, wSkyTex, &QWidget::setVisible);
    connect(rdbBackgroundColor, &QRadioButton::toggled, m_pkrBackgroundColor, &ColorPicker::setVisible);
    connect(rdbBackgroundCheckerboard, &QRadioButton::toggled, wCheckerboard, &QWidget::setVisible);
    connect(rdbBackgroundGrid, &QRadioButton::toggled, wGrid, &QWidget::setVisible);
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grBackground = new QGroupBox("Background");
    grBackground->setLayout(layoutBackground);
    layoutCtr->addWidget(grBackground);
    ////////////////////////////////////////////////////////////////////////////////
    m_smBackgroundMode = new QSignalMapper(this);
    connect(rdbBackgroundSkyBox, SIGNAL(clicked()), m_smBackgroundMode, SLOT(map()));
    connect(rdbBackgroundColor, SIGNAL(clicked()), m_smBackgroundMode, SLOT(map()));
    connect(rdbBackgroundCheckerboard, SIGNAL(clicked()), m_smBackgroundMode, SLOT(map()));
    connect(rdbBackgroundGrid, SIGNAL(clicked()), m_smBackgroundMode, SLOT(map()));

    m_smBackgroundMode->setMapping(rdbBackgroundSkyBox, static_cast<int>(BackgroundMode::SkyBox));
    m_smBackgroundMode->setMapping(rdbBackgroundColor, static_cast<int>(BackgroundMode::Color));
    m_smBackgroundMode->setMapping(rdbBackgroundCheckerboard, static_cast<int>(BackgroundMode::Checkerboard));
    m_smBackgroundMode->setMapping(rdbBackgroundGrid, static_cast<int>(BackgroundMode::Grid));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupFloorControllers(QBoxLayout* layoutCtr)
{
    m_cbFloorTexture = new EnhancedComboBox;
    m_sldFloorSize   = new EnhancedSlider;
    m_sldFloorSize->setRange(1, 100);
    m_sldFloorSize->getSlider()->setValue(DEFAULT_FLOOR_SIZE);
    m_sldFloorExposure = new EnhancedSlider;
    m_sldFloorExposure->setRange(1, 100);
    m_sldFloorExposure->getSlider()->setValue(DEFAULT_FLOOR_EXPOSURE);
    ////////////////////////////////////////////////////////////////////////////////
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* layoutFloorCtr = new QVBoxLayout;
    layoutFloorCtr->addLayout(m_cbFloorTexture->getLayout());
    layoutFloorCtr->addSpacing(10);
    layoutFloorCtr->addWidget(line);
    layoutFloorCtr->addLayout(m_sldFloorSize->getLayoutWithLabel("Size:"));
    layoutFloorCtr->addLayout(m_sldFloorExposure->getLayoutWithLabel("Exposure:"));
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grFloor = new QGroupBox;
    grFloor->setTitle("Floor Texture");
    grFloor->setLayout(layoutFloorCtr);
    layoutCtr->addWidget(grFloor);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QStringList Controller::getSceneFiles()
{
    QDir dataDir(getScenePath());
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);

    return dataDir.entryList();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupSimulationControllers(QBoxLayout* layoutCtr)
{
    m_cbSimulationScene = new QComboBox;
    m_cbSimulationScene->addItem(QString("None"));
    m_cbSimulationScene->addItems(getSceneFiles());
    QVBoxLayout* layoutScene = new QVBoxLayout;
    layoutScene->addWidget(m_cbSimulationScene);
    QGroupBox* grScene = new QGroupBox;
    grScene->setTitle("Scene");
    grScene->setLayout(layoutScene);
    ////////////////////////////////////////////////////////////////////////////////
    m_OutputPath = new BrowsePathWidget("Browse");
    m_OutputPath->setPath(getCapturePath());
    m_chkEnableOutput = new QCheckBox("Export to Images");
    QVBoxLayout* layoutOutput = new QVBoxLayout;
    layoutOutput->addWidget(m_chkEnableOutput);
    layoutOutput->addLayout(m_OutputPath->getLayout());
    QGroupBox* grpOutput = new QGroupBox;
    grpOutput->setTitle("Output");
    grpOutput->setLayout(layoutOutput);
    ////////////////////////////////////////////////////////////////////////////////
    layoutCtr->addWidget(grScene);
    layoutCtr->addWidget(grpOutput);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupMaterialControllers(QBoxLayout* layoutCtr)
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
    layoutCtr->addWidget(grMaterial);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupColorModeControllers(QBoxLayout* layoutCtr)
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
    layoutColorMode->addWidget(rdbColorRandom, 0, 0, 1, 1);
    layoutColorMode->addWidget(rdbColorRamp, 0, 1, 1, 1);
    layoutColorMode->addWidget(rdbColorUniform, 1, 0, 1, 1);
    layoutColorMode->addWidget(rdbColorObjIdx, 1, 1, 1, 1);
    layoutColorMode->addWidget(rdbColorVelMag, 2, 0, 1, 1);
    ////////////////////////////////////////////////////////////////////////////////
    m_smParticleColorMode = new QSignalMapper(this);
    connect(rdbColorRandom, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorRamp, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorUniform, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorObjIdx, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorVelMag, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));

    m_smParticleColorMode->setMapping(rdbColorRandom, static_cast<int>(ParticleColorMode::Random));
    m_smParticleColorMode->setMapping(rdbColorRamp, static_cast<int>(ParticleColorMode::Ramp));
    m_smParticleColorMode->setMapping(rdbColorUniform, static_cast<int>(ParticleColorMode::UniformMaterial));
    m_smParticleColorMode->setMapping(rdbColorObjIdx, static_cast<int>(ParticleColorMode::ObjectIndex));
    m_smParticleColorMode->setMapping(rdbColorVelMag, static_cast<int>(ParticleColorMode::VelocityMagnitude));
    ////////////////////////////////////////////////////////////////////////////////
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
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
    layoutColorCtrls->addWidget(line);
    layoutColorCtrls->addSpacing(5);
    layoutColorCtrls->addLayout(layoutColorData);
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grColorMode = new QGroupBox;
    grColorMode->setTitle("Particle Color");
    grColorMode->setLayout(layoutColorCtrls);
    layoutCtr->addWidget(grColorMode);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupMiscControllers(QBoxLayout* layoutCtr)
{
    m_chkUseAniKernel = new QCheckBox("Enable anisotropic kernel (if available)");
    ////////////////////////////////////////////////////////////////////////////////
    m_chkRenderBox = new QCheckBox("Render domain box");
    m_chkRenderBox->setChecked(true);
    m_pkrBoxColor = new ColorPicker;
    m_pkrBoxColor->setMinimumHeight(20);
    m_pkrBoxColor->setColor(DEFAULT_BOX_COLOR);
    connect(m_chkRenderBox, &QCheckBox::toggled, m_pkrBoxColor, &ColorPicker::setEnabled);
    QHBoxLayout* layoutBox = new QHBoxLayout;
    layoutBox->addWidget(new QLabel("Box color:"));
    layoutBox->addWidget(m_pkrBoxColor);
    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* layoutMiscCtrls = new QVBoxLayout;
    QGroupBox*   grMiscCtrls     = new QGroupBox("MISC");
    ////////////////////////////////////////////////////////////////////////////////
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    ////////////////////////////////////////////////////////////////////////////////
    layoutMiscCtrls->addWidget(m_chkUseAniKernel);
    layoutMiscCtrls->addSpacing(5);
    layoutMiscCtrls->addWidget(line);
    layoutMiscCtrls->addSpacing(5);
    layoutMiscCtrls->addWidget(m_chkRenderBox);
    layoutMiscCtrls->addLayout(layoutBox);
    grMiscCtrls->setLayout(layoutMiscCtrls);
    layoutCtr->addWidget(grMiscCtrls);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupButtons(QBoxLayout* layoutCtr)
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
    ////////////////////////////////////////////////////////////////////////////////
    layoutCtr->addLayout(layoutButtons);
}
