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

#include <QtAppHelpers/OpenGLController.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLController::setupBasicGUI(Int width)
{
    setupBackgroundControllers();
    setupFloorControllers();
    setupBoxControllers();
    ////////////////////////////////////////////////////////////////////////////////
    m_RenderControllers->setLayout(m_LayoutRenderControllers);
    m_LightEditor = new PointLightEditor(nullptr, this);
    ////////////////////////////////////////////////////////////////////////////////
    m_MainTab->setTabPosition(QTabWidget::South);
    m_MainTab->setTabShape(QTabWidget::Triangular);
    m_MainTab->addTab(m_RenderControllers, "Main Controllers");
    m_MainTab->addTab(m_LightEditor,       "Lights");
    ////////////////////////////////////////////////////////////////////////////////
    m_MainLayout->addWidget(m_MainTab);
    setLayout(m_MainLayout);
    ////////////////////////////////////////////////////////////////////////////////
    setFixedWidth(width);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLController::connectBasicWidgets()
{
    ////////////////////////////////////////////////////////////////////////////////
    // lights
    connect(m_LightEditor, &PointLightEditor::lightsChanged, m_GLWidget,    &OpenGLWidget::updateLights);
    connect(m_GLWidget,    &OpenGLWidget::lightsObjChanged,  m_LightEditor, &PointLightEditor::setLightObject);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // background mode
    connect(m_smBackgroundMode, SIGNAL(mapped(int)), m_GLWidget, SLOT(setBackgroundMode(int)));
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // sky box
    connect(m_cbSkyTexture->getComboBox(), SIGNAL(currentIndexChanged(int)), m_GLWidget, SLOT(setSkyBoxTextureIndex(int)));
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // background color
    connect(m_pkrBackgroundColor, &ColorPicker::colorChanged, [&](float r, float g, float b) { m_GLWidget->setClearColor(Vec3f(r, g, b)); });
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // checkerboard background
    connect(m_pkrCheckerColor1,                  &ColorPicker::colorChanged, [&](float r, float g, float b) { m_GLWidget->setCheckerboarrdColor1(Vec3f(r, g, b)); });
    connect(m_pkrCheckerColor2,                  &ColorPicker::colorChanged, [&](float r, float g, float b) { m_GLWidget->setCheckerboarrdColor2(Vec3f(r, g, b)); });
    connect(m_sldCheckerboardScale->getSlider(), &QSlider::valueChanged,     [&](Int value) { m_GLWidget->setCheckerboarrdScales(Vec2i(value)); });
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // grid background
    connect(m_pkrGridBackgroundColor,    &ColorPicker::colorChanged, [&](float r, float g, float b) { m_GLWidget->setGridBackgroundColor(Vec3f(r, g, b)); });
    connect(m_pkrGridLineColor,          &ColorPicker::colorChanged, [&](float r, float g, float b) { m_GLWidget->setGridLineColor(Vec3f(r, g, b)); });
    connect(m_sldGridScale->getSlider(), &QSlider::valueChanged,     [&](Int value) { m_GLWidget->setGridScales(Vec2i(value)); });
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // floor
    connect(m_cbFloorTexture->getComboBox(), SIGNAL(currentIndexChanged(int)), m_GLWidget, SLOT(setFloorTextureIndex(int)));
    connect(m_sldFloorSize->getSlider(),     &QSlider::valueChanged,           m_GLWidget, &OpenGLWidget::setFloorSize);
    connect(m_sldFloorSize->getSlider(),     &QSlider::valueChanged,           m_GLWidget, &OpenGLWidget::setFloorTexScales);
    connect(m_sldFloorExposure->getSlider(), &QSlider::valueChanged,           m_GLWidget, &OpenGLWidget::setFloorExposure);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // box
    connect(m_chkRenderBox, &QCheckBox::toggled,        m_GLWidget, &OpenGLWidget::enableRenderBox);
    connect(m_pkrBoxColor,  &ColorPicker::colorChanged, [&](float r, float g, float b) { m_GLWidget->setBoxColor(Vec3f(r, g, b)); });
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLController::setupBackgroundControllers()
{
    QRadioButton* rdbBackgroundSkyBox       = new QRadioButton("SkyBox");
    QRadioButton* rdbBackgroundColor        = new QRadioButton("Color");
    QRadioButton* rdbBackgroundCheckerboard = new QRadioButton("Checkerboard");
    QRadioButton* rdbBackgroundGrid         = new QRadioButton("Grid");
    rdbBackgroundSkyBox->setChecked(true);

    QGridLayout* layoutBackgroundType = new QGridLayout;
    layoutBackgroundType->addWidget(rdbBackgroundSkyBox,       0, 0, 1, 1);
    layoutBackgroundType->addWidget(rdbBackgroundColor,        0, 1, 1, 1);
    layoutBackgroundType->addWidget(rdbBackgroundCheckerboard, 1, 0, 1, 1);
    layoutBackgroundType->addWidget(rdbBackgroundGrid,         1, 1, 1, 1);
    ////////////////////////////////////////////////////////////////////////////////
    QWidget* wSkyTex = new QWidget;
    m_cbSkyTexture->getLayout()->setContentsMargins(0, 0, 0, 0);
    wSkyTex->setLayout(m_cbSkyTexture->getLayout());
    wSkyTex->setMinimumHeight(25);
    ////////////////////////////////////////////////////////////////////////////////
    m_pkrBackgroundColor->setMaximumHeight(25);
    m_pkrBackgroundColor->setVisible(false);
    m_pkrBackgroundColor->setColor(m_GLWidget->getClearColor());
    ////////////////////////////////////////////////////////////////////////////////
    m_sldCheckerboardScale->setRange(2, 100);
    m_sldCheckerboardScale->setValue(CheckerboardBackgroundRender::getDefaultScales().x);
    m_pkrCheckerColor1->setColor(CheckerboardBackgroundRender::getDefaultColor1());
    m_pkrCheckerColor2->setColor(CheckerboardBackgroundRender::getDefaultColor2());

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
    m_pkrGridBackgroundColor->setColor(GridBackgroundRender::getDefaultBackgroundColor());
    m_pkrGridLineColor->setColor(GridBackgroundRender::getDefaultLineColor());
    m_sldGridScale = new EnhancedSlider;
    m_sldGridScale->setRange(2, 100);
    m_sldGridScale->setValue(GridBackgroundRender::getDefaultScales().x);

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
    QVBoxLayout* layoutBackground = new QVBoxLayout;
    layoutBackground->addLayout(layoutBackgroundType);
    layoutBackground->addSpacing(5);
    layoutBackground->addWidget(QtAppUtils::getLineSeparator());
    layoutBackground->addSpacing(5);
    layoutBackground->addWidget(wSkyTex);
    layoutBackground->addWidget(m_pkrBackgroundColor);
    layoutBackground->addWidget(wCheckerboard);
    layoutBackground->addWidget(wGrid);

    connect(rdbBackgroundSkyBox,       &QRadioButton::toggled, wSkyTex,              &QWidget::setVisible);
    connect(rdbBackgroundColor,        &QRadioButton::toggled, m_pkrBackgroundColor, &ColorPicker::setVisible);
    connect(rdbBackgroundCheckerboard, &QRadioButton::toggled, wCheckerboard,        &QWidget::setVisible);
    connect(rdbBackgroundGrid,         &QRadioButton::toggled, wGrid,                &QWidget::setVisible);
    ////////////////////////////////////////////////////////////////////////////////
    m_grBackgroundCtrl->setLayout(layoutBackground);
    m_LayoutRenderControllers->addWidget(m_grBackgroundCtrl);
    ////////////////////////////////////////////////////////////////////////////////
    connect(rdbBackgroundSkyBox,       SIGNAL(clicked()), m_smBackgroundMode, SLOT(map()));
    connect(rdbBackgroundColor,        SIGNAL(clicked()), m_smBackgroundMode, SLOT(map()));
    connect(rdbBackgroundCheckerboard, SIGNAL(clicked()), m_smBackgroundMode, SLOT(map()));
    connect(rdbBackgroundGrid,         SIGNAL(clicked()), m_smBackgroundMode, SLOT(map()));

    m_smBackgroundMode->setMapping(rdbBackgroundSkyBox,       static_cast<int>(OpenGLWidget::BackgroundMode::SkyBox));
    m_smBackgroundMode->setMapping(rdbBackgroundColor,        static_cast<int>(OpenGLWidget::BackgroundMode::Color));
    m_smBackgroundMode->setMapping(rdbBackgroundCheckerboard, static_cast<int>(OpenGLWidget::BackgroundMode::Checkerboard));
    m_smBackgroundMode->setMapping(rdbBackgroundGrid,         static_cast<int>(OpenGLWidget::BackgroundMode::Grid));
    ////////////////////////////////////////////////////////////////////////////////
    loadSkyBoxTextures();
}

void OpenGLController::loadSkyBoxTextures()
{
    auto cbSkyTex        = m_cbSkyTexture->getComboBox();
    Int  currentSkyTexID = cbSkyTex->currentIndex();
    cbSkyTex->clear();
    cbSkyTex->addItem("None");
    ////////////////////////////////////////////////////////////////////////////////
    auto skyTexFolders = QtAppUtils::getTextureFolders("Sky");
    foreach(const auto& tex, skyTexFolders) {
        cbSkyTex->addItem(tex);
    }
    cbSkyTex->setCurrentIndex(currentSkyTexID > 0 ? currentSkyTexID : 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLController::setupFloorControllers()
{
    m_sldFloorSize->setRange(1, 100);
    m_sldFloorSize->getSlider()->setValue(10);
    auto layoutFloorSize = new QHBoxLayout;
    layoutFloorSize->addWidget(new QLabel("Size:"), 1);
    layoutFloorSize->addLayout(m_sldFloorSize->getLayout(), 5);
    ////////////////////////////////////////////////////////////////////////////////
    m_sldFloorExposure->setRange(0, 100);
    m_sldFloorExposure->getSlider()->setValue(50);
    auto layoutFloorExposure = new QHBoxLayout;
    layoutFloorExposure->addWidget(new QLabel("Exposure:"), 1);
    layoutFloorExposure->addLayout(m_sldFloorExposure->getLayout(), 5);
    ////////////////////////////////////////////////////////////////////////////////
    auto floorLayout = new QVBoxLayout;
    floorLayout->addLayout(m_cbFloorTexture->getLayout());
    floorLayout->addSpacing(10);
    floorLayout->addWidget(QtAppUtils::getLineSeparator());
    floorLayout->addLayout(layoutFloorSize);
    floorLayout->addLayout(layoutFloorExposure);
    ////////////////////////////////////////////////////////////////////////////////
    m_grFloorCtrl->setLayout(floorLayout);
    m_LayoutRenderControllers->addWidget(m_grFloorCtrl);
    ////////////////////////////////////////////////////////////////////////////////
    loadFloorTextures();
}

void OpenGLController::loadFloorTextures()
{
    auto cbFloorTex        = m_cbFloorTexture->getComboBox();
    Int  currentFloorTexID = cbFloorTex->currentIndex();
    cbFloorTex->clear();
    cbFloorTex->addItem("None");
    ////////////////////////////////////////////////////////////////////////////////
    auto floorTexFolders = QtAppUtils::getTextureFiles("Floor");
    foreach(const auto& tex, floorTexFolders) {
        cbFloorTex->addItem(tex);
    }
    cbFloorTex->setCurrentIndex(currentFloorTexID > 0 ? currentFloorTexID : 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OpenGLController::setupBoxControllers()
{
    m_chkRenderBox->setChecked(true);
    m_pkrBoxColor->setMinimumHeight(20);
    m_pkrBoxColor->setColor(WireFrameBoxRender::getDefaultBoxColor());
    connect(m_chkRenderBox, &QCheckBox::toggled, m_pkrBoxColor, &ColorPicker::setEnabled);
    ////////////////////////////////////////////////////////////////////////////////
    QHBoxLayout* layoutBoxCtrl = new QHBoxLayout;
    layoutBoxCtrl->addWidget(m_chkRenderBox);
    layoutBoxCtrl->addWidget(m_pkrBoxColor);
    m_grBoxCtrl->setLayout(layoutBoxCtrl);
    m_LayoutRenderControllers->addWidget(m_grBoxCtrl);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana