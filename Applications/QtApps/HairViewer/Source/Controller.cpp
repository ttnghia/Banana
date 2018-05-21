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
//    /                    Created: 2018 by Nghia Truong                     \
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
#include <Banana/Utils/MathHelpers.h>
#include <QtAppHelpers/QtAppUtils.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupGUI()
{
    setupHairRenderModeControllers();
    setupHairColorModeControllers();
    setupHairMaterialControllers();
    setupMeshMaterialControllers();
    setupIOControllers();
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
    connect(m_smHairRenderMode, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), m_RenderWidget, &RenderWidget::setRenderMode);
    connect(m_smHairColorMode,  static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), m_RenderWidget, &RenderWidget::setColorMode);
    connect(m_smHairColorMode,  static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), [&](int colorMode)
            {
                m_msHairMaterial->getComboBox()->setEnabled(colorMode == HairColorMode::UniformMaterial);
            });
    connect(m_msHairMaterial,  &MaterialSelector::materialChanged, m_RenderWidget, &RenderWidget::setHairMaterial);
    connect(m_msMeshMaterial,  &MaterialSelector::materialChanged, m_RenderWidget, &RenderWidget::setMeshMaterial);
    connect(m_pkrColorDataMin, &ColorPicker::colorChanged,         [&](float r, float g, float b) { m_RenderWidget->setColorDataMin(Vec3f(r, g, b)); });
    connect(m_pkrColorDataMax, &ColorPicker::colorChanged,         [&](float r, float g, float b) { m_RenderWidget->setColorDataMax(Vec3f(r, g, b)); });
    connect(m_btnRndColor,     &QPushButton::clicked,              [&]()
            {
                auto colorMin = MathHelpers::vrand<Vec3f>();
                auto colorMax = MathHelpers::vrand<Vec3f>();
                m_pkrColorDataMin->setColor(colorMin);
                m_pkrColorDataMax->setColor(colorMax);
                m_RenderWidget->setColorDataMin(colorMin);
                m_RenderWidget->setColorDataMax(colorMax);
            });
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // scene
    connect(m_InputPath, &BrowsePathWidget::pathChanged, [&](const QString& inputFolder)
            {
                auto models = QtAppUtils::getFiles(inputFolder, QStringList({ "*.hair", "*.data", "*.bnn" }));
                m_cbModels->getComboBox()->clear();
                m_cbModels->getComboBox()->addItems(models);
                m_lblModelCount->setText(QString("Model count: %1").arg(models.count()));
            });
    connect(m_MeshFile,   &BrowsePathWidget::pathChanged, m_RenderWidget, &RenderWidget::loadMesh);
    connect(m_OutputPath, &BrowsePathWidget::pathChanged, m_RenderWidget, &RenderWidget::setCapturePath);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // buttons
    connect(m_btnResetCamera,   &QPushButton::clicked, m_RenderWidget, &RenderWidget::resetCameraPosition);
    connect(m_btnClipViewPlane, &QPushButton::clicked, m_RenderWidget, &RenderWidget::enableClipPlane);
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupIOControllers()
{
    QWidget* wgIOControllers = new QWidget;
    m_MainTab->insertTab(0, wgIOControllers, "Data IO");
    m_MainTab->setCurrentIndex(0);
    ////////////////////////////////////////////////////////////////////////////////
    m_InputPath = new BrowsePathWidget("Browse");
    QGroupBox* grInput = new QGroupBox;
    grInput->setTitle("Model Path");
    grInput->setLayout(m_InputPath->getLayout());
    ////////////////////////////////////////////////////////////////////////////////
    m_chkRenderAsSequence = new QCheckBox("Render as sequence");
    m_cbModels            = new EnhancedComboBox;
    m_btnReloadModel      = new QPushButton("Reload");
    QHBoxLayout* layoutModels = new QHBoxLayout;
    layoutModels->addLayout(m_cbModels->getLayout(), 20);
    layoutModels->addStretch(1);
    layoutModels->addWidget(m_btnReloadModel, 5);
    ////////////////////////////////////////////////////////////////////////////////
    QFrame* line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    m_lblModelCount = new QLabel;
    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* layoutModelsCtr = new QVBoxLayout;
    layoutModelsCtr->addWidget(m_chkRenderAsSequence);
    layoutModelsCtr->addSpacing(1);
    layoutModelsCtr->addLayout(layoutModels);
    layoutModelsCtr->addSpacing(5);
    layoutModelsCtr->addWidget(line1);
    layoutModelsCtr->addSpacing(5);
    layoutModelsCtr->addWidget(m_lblModelCount);
    QGroupBox* grModels = new QGroupBox;
    grModels->setTitle("Models");
    grModels->setLayout(layoutModelsCtr);
    ////////////////////////////////////////////////////////////////////////////////
    m_MeshFile = new BrowsePathWidget("Browse", false);
    QVBoxLayout* layoutMeshPath = new QVBoxLayout;
    layoutMeshPath->addLayout(m_MeshFile->getLayout());
    QGroupBox* grpMeshPath = new QGroupBox;
    grpMeshPath->setTitle("Mesh");
    grpMeshPath->setLayout(layoutMeshPath);
    ////////////////////////////////////////////////////////////////////////////////

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
    QFrame* line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* layoutIOControllers = new QVBoxLayout;
    layoutIOControllers->addWidget(grInput);
    layoutIOControllers->addWidget(grModels);
    layoutIOControllers->addWidget(grpMeshPath);
    layoutIOControllers->addSpacing(5);
    layoutIOControllers->addWidget(line2);
    layoutIOControllers->addSpacing(5);
    layoutIOControllers->addWidget(grpOutput);
    layoutIOControllers->addStretch();
    wgIOControllers->setLayout(layoutIOControllers);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupHairMaterialControllers()
{
    m_msHairMaterial = new MaterialSelector;
    m_msHairMaterial->setCustomMaterial(CUSTOM_HAIR_MATERIAL);
    m_msHairMaterial->setDefaultCustomMaterial(true);
    m_msHairMaterial->getComboBox()->setEnabled(false);
    QGridLayout* layoutMaterial = new QGridLayout;
    layoutMaterial->addLayout(m_msHairMaterial->getLayout(), 0, 0, 1, 2);
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grMaterial = new QGroupBox("Hair Material");
    grMaterial->setLayout(layoutMaterial);
    m_LayoutRenderControllers->addWidget(grMaterial);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupMeshMaterialControllers()
{
    m_msMeshMaterial = new MaterialSelector;
    m_msMeshMaterial->setCustomMaterial(CUSTOM_MESH_MATERIAL);
    m_msMeshMaterial->setDefaultCustomMaterial(true);
    QGridLayout* layoutMaterial = new QGridLayout;
    layoutMaterial->addLayout(m_msMeshMaterial->getLayout(), 0, 0, 1, 2);
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grMaterial = new QGroupBox("Mesh Material");
    grMaterial->setLayout(layoutMaterial);
    m_LayoutRenderControllers->addWidget(grMaterial);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupHairRenderModeControllers()
{
    ////////////////////////////////////////////////////////////////////////////////
    QRadioButton* rdbLineRender             = new QRadioButton("Line Render");
    QRadioButton* rdbVertexParticle         = new QRadioButton("Vertex Particle");
    QRadioButton* rdbLineWithVertexParticle = new QRadioButton("Line With Vertex Particle");
    rdbLineRender->setChecked(true);
    ////////////////////////////////////////////////////////////////////////////////
    QGridLayout* layoutHairRenderMode = new QGridLayout;
    layoutHairRenderMode->addWidget(rdbLineRender,             0, 0, 1, 1);
    layoutHairRenderMode->addWidget(rdbVertexParticle,         0, 1, 1, 1);
    layoutHairRenderMode->addWidget(rdbLineWithVertexParticle, 1, 0, 1, 2);
    ////////////////////////////////////////////////////////////////////////////////
    m_smHairRenderMode = new QSignalMapper(this);
    connect(rdbLineRender,             SIGNAL(clicked()), m_smHairRenderMode, SLOT(map()));
    connect(rdbVertexParticle,         SIGNAL(clicked()), m_smHairRenderMode, SLOT(map()));
    connect(rdbLineWithVertexParticle, SIGNAL(clicked()), m_smHairRenderMode, SLOT(map()));

    m_smHairRenderMode->setMapping(rdbLineRender,             static_cast<int>(HairRenderMode::LineRender));
    m_smHairRenderMode->setMapping(rdbVertexParticle,         static_cast<int>(HairRenderMode::VertexParticle));
    m_smHairRenderMode->setMapping(rdbLineWithVertexParticle, static_cast<int>(HairRenderMode::LineWithVertexParticle));
    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* layoutHairRenderCtrls = new QVBoxLayout;
    layoutHairRenderCtrls->addLayout(layoutHairRenderMode);
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grHairRenderMode = new QGroupBox;
    grHairRenderMode->setTitle("Hair Render Modes");
    grHairRenderMode->setLayout(layoutHairRenderCtrls);
    m_LayoutRenderControllers->addWidget(grHairRenderMode);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupHairColorModeControllers()
{
    ////////////////////////////////////////////////////////////////////////////////
    QRadioButton* rdbColorRandom  = new QRadioButton("Random");
    QRadioButton* rdbColorUniform = new QRadioButton("Uniform Material");
    QRadioButton* rdbColorObjIdx  = new QRadioButton("Object Index");
    QRadioButton* rdbColorVelMag  = new QRadioButton("Velocity Magnitude");
    rdbColorRandom->setChecked(true);
    ////////////////////////////////////////////////////////////////////////////////
    QGridLayout* layoutColorMode = new QGridLayout;
    layoutColorMode->addWidget(rdbColorRandom,  0, 0, 1, 1);
    layoutColorMode->addWidget(rdbColorUniform, 0, 1, 1, 1);
    layoutColorMode->addWidget(rdbColorObjIdx,  1, 0, 1, 1);
    layoutColorMode->addWidget(rdbColorVelMag,  1, 1, 1, 1);
    ////////////////////////////////////////////////////////////////////////////////
    m_smHairColorMode = new QSignalMapper(this);
    connect(rdbColorRandom,  SIGNAL(clicked()), m_smHairColorMode, SLOT(map()));
    connect(rdbColorUniform, SIGNAL(clicked()), m_smHairColorMode, SLOT(map()));
    connect(rdbColorObjIdx,  SIGNAL(clicked()), m_smHairColorMode, SLOT(map()));
    connect(rdbColorVelMag,  SIGNAL(clicked()), m_smHairColorMode, SLOT(map()));

    m_smHairColorMode->setMapping(rdbColorRandom,  static_cast<int>(HairColorMode::Random));
    m_smHairColorMode->setMapping(rdbColorUniform, static_cast<int>(HairColorMode::UniformMaterial));
    m_smHairColorMode->setMapping(rdbColorObjIdx,  static_cast<int>(HairColorMode::ObjectIndex));
    m_smHairColorMode->setMapping(rdbColorVelMag,  static_cast<int>(HairColorMode::VelocityMagnitude));
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
    grColorMode->setTitle("Hair Color");
    grColorMode->setLayout(layoutColorCtrls);
    m_LayoutRenderControllers->addWidget(grColorMode);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupButtons()
{
    ////////////////////////////////////////////////////////////////////////////////
    m_btnResetCamera   = new QPushButton("Reset Camera");
    m_btnEditClipPlane = new QPushButton("Edit Clip Plane");
    m_btnClipViewPlane = new QPushButton("Clip View");
    m_btnClipViewPlane->setCheckable(true);
    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* layoutButtons = new QVBoxLayout;
    layoutButtons->addWidget(m_btnResetCamera);
    layoutButtons->addWidget(m_btnClipViewPlane);
    layoutButtons->addWidget(m_btnEditClipPlane);
    m_MainLayout->addStretch();
    m_MainLayout->addLayout(layoutButtons);
}
