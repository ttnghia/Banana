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
    setupMaterialControllers(controlLayout);
    controlLayout->addStretch();
    setupButtons(btnLayout);

    ////////////////////////////////////////////////////////////////////////////////
    m_LightEditor = new PointLightEditor(nullptr, this);
//    m_LightEditor->setMaxLight(1);
    QWidget* mainControls = new QWidget;
    mainControls->setLayout(controlLayout);

    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->setTabPosition(QTabWidget::South);
    tabWidget->setTabShape(QTabWidget::Triangular);
    tabWidget->addTab(mainControls,  "Main Controls");
    tabWidget->addTab(m_LightEditor, "Lights");

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);

    ////////////////////////////////////////////////////////////////////////////////
    //    setLayout(controlLayout);
    setFixedWidth(320);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::loadTextures()
{
    ////////////////////////////////////////////////////////////////////////////////
    // sky textures
    int currentSkyTexID = m_cbSkyTexture->getComboBox()->currentIndex();
    m_cbSkyTexture->getComboBox()->clear();
    m_cbSkyTexture->getComboBox()->addItem("None");
    QStringList skyTexFolders = getTextureFiles("Sky");

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

    m_cbFloorTexture->setCurrentIndex(currentFloorTexID > 0 ? currentFloorTexID : 0);
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
    m_sldFloorSize->setValue(10);
    QHBoxLayout* floorSizeLayout = new QHBoxLayout;
    floorSizeLayout->addWidget(new QLabel("Size:"), 1);
    floorSizeLayout->addLayout(m_sldFloorSize->getLayout(), 5);

    m_sldFloorTexScale = new EnhancedSlider;
    m_sldFloorTexScale->setRange(1, 100);
    m_sldFloorTexScale->setValue(5);
    QHBoxLayout* floorTexScaleLayout = new QHBoxLayout;
    floorTexScaleLayout->addWidget(new QLabel("TexScale:"), 1);
    floorTexScaleLayout->addLayout(m_sldFloorTexScale->getLayout(), 5);

    m_sldFloorExposure = new EnhancedSlider;
    m_sldFloorExposure->setRange(0, 100);
    m_sldFloorExposure->setValue(50);
    QHBoxLayout* floorExposureLayout = new QHBoxLayout;
    floorExposureLayout->addWidget(new QLabel("Exposure:"), 1);
    floorExposureLayout->addLayout(m_sldFloorExposure->getLayout(), 5);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QVBoxLayout* floorLayout = new QVBoxLayout;
    floorLayout->addLayout(m_cbFloorTexture->getLayout());
    floorLayout->addSpacing(10);
    floorLayout->addWidget(line);
    floorLayout->addLayout(floorSizeLayout);
    floorLayout->addLayout(floorTexScaleLayout);
    floorLayout->addLayout(floorExposureLayout);

    QGroupBox* floorGroup = new QGroupBox;
    floorGroup->setTitle("Floor Texture");
    floorGroup->setLayout(floorLayout);
    ctrLayout->addWidget(floorGroup);

    ////////////////////////////////////////////////////////////////////////////////
    loadTextures();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupMaterialControllers(QBoxLayout* ctrLayout)
{
    m_msMeshMaterial = new MaterialSelector;
    m_msMeshMaterial->setCustomMaterial(DEFAULT_GLASS_MATERIAL);
    m_msMeshMaterial->setDefaultCustomMaterial(true);

    m_cbMeshMaterialID = new QComboBox;

    for(int i = 0; i < MAX_NUM_MESHES; ++i)
    {
        m_cbMeshMaterialID->addItem(QString("%1").arg(i));
    }

    QGridLayout* meshMaterialSelectorLayout = new QGridLayout;
    meshMaterialSelectorLayout->addWidget(m_cbMeshMaterialID, 0, 0, 1, 1);
    meshMaterialSelectorLayout->addLayout(m_msMeshMaterial->getLayout(), 0, 1, 1, 4);

    QGridLayout* meshMaterialLayout = new QGridLayout;
    meshMaterialLayout->addWidget(new QLabel("Mesh: "), 0, 0, Qt::AlignRight);
    meshMaterialLayout->addLayout(meshMaterialSelectorLayout, 0, 1, 1, 5);

    QGroupBox* grMeshMaterial = new QGroupBox("Material");
    grMeshMaterial->setLayout(meshMaterialLayout);
    ctrLayout->addWidget(grMeshMaterial);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupButtons(QBoxLayout* ctrLayout)
{
    m_btnResetCamera = new QPushButton("Reset Camera");
    ctrLayout->addWidget(m_btnResetCamera);

    ////////////////////////////////////////////////////////////////////////////////
    m_btnExportImage = new QPushButton("Export Image");
    ctrLayout->addWidget(m_btnExportImage);

    ////////////////////////////////////////////////////////////////////////////////
    m_btnReloadTextures = new QPushButton("Reload Textures");
    ctrLayout->addWidget(m_btnReloadTextures);
}
