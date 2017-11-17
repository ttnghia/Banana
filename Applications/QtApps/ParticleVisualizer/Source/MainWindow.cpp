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

#include <QMouseEvent>
#include <Banana/Utils/NumberHelpers.h>
#include "MainWindow.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MainWindow::MainWindow(QWidget* parent) : OpenGLMainWindow(parent)
{
    instantiateOpenGLWidget();
    setupRenderWidgets();
    setupPlayList();
    setupStatusBar();
    connectWidgets();
    setArthurStyle();

    setWindowTitle("Particle Visualizer");
    setFocusPolicy(Qt::StrongFocus);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::instantiateOpenGLWidget()
{
    if(m_GLWidget != nullptr) {
        delete m_GLWidget;
    }

    m_RenderWidget = new RenderWidget(this);
    setupOpenglWidget(m_RenderWidget);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MainWindow::processKeyPressEvent(QKeyEvent* event)
{
    switch(event->key()) {
        case Qt::Key_B:
            m_InputPath->browse();
            return true;

        case Qt::Key_O:
            m_OutputPath->browse();
            return true;

        case Qt::Key_R:
            m_Controller->m_btnReverse->click();
            return true;

        case Qt::Key_Space:
            m_Controller->m_btnPause->click();
            return true;

        case Qt::Key_N:
            m_Controller->m_btnNextFrame->click();
            return true;

        case Qt::Key_X:
            m_Controller->m_btnClipViewPlane->click();
            return true;

        case Qt::Key_F1:
            m_Controller->m_btnReset->click();
            return true;

        case Qt::Key_F5:
            loadVizData(m_InputPath->getCurrentPath());
            return true;

        case Qt::Key_F9:
            m_ClipPlaneEditor->show();
            return true;

        default:
            return OpenGLMainWindow::processKeyPressEvent(event);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::showEvent(QShowEvent* ev)
{
    QMainWindow::showEvent(ev);
    updateStatusMemoryUsage();
    m_Controller->m_pkrBackgroundColor->setColor(m_GLWidget->getClearColor().x, m_GLWidget->getClearColor().y, m_GLWidget->getClearColor().z);
    ////////////////////////////////////////////////////////////////////////////////
    if(m_DataList->getListSize() > 0) {
        QTimer::singleShot(100, this, [&]()
                           {
                               m_DataList->show();
                               m_DataList->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignBottom | Qt::AlignRight, m_DataList->size(), qApp->desktop()->availableGeometry()));
                           });
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::updateStatusCurrentFrame(int currentFrame)
{
    m_lblStatusCurrentFrame->setText(QString("Current frame: %1").arg(currentFrame));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::updateStatusNumParticlesAndMeshes()
{
    m_lblStatusNumParticles->setText(QString("Particles: %1 | Mesh(es): %2")
                                         .arg(QString::fromStdString(NumberHelpers::formatWithCommas(m_nParticles)))
                                         .arg(QString::fromStdString(NumberHelpers::formatWithCommas(m_nMeshes))));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::updateNumFrames(int numFrames)
{
    m_DataReader->setNumFrames(numFrames);
    m_sldFrame->setRange(1, numFrames);

    ////////////////////////////////////////////////////////////////////////////////
    m_lblStatusNumFrames->setText(QString("Total frame: %1").arg(numFrames));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::updateStatusReadInfo(double readTime, size_t bytes)
{
    m_lblStatusReadInfo->setText(QString("Load data time: %1 ms (%2 MB)").arg(readTime).arg(static_cast<double>(bytes) / 1048576.0));
}

void MainWindow::updateStatusMemoryUsage()
{
    m_lblStatusMemoryUsage->setText(QString("Memory usage: %1 (MBs)").arg(QString::fromStdString(NumberHelpers::formatWithCommas(getCurrentRSS() / 1048576.0))));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::loadVizData(const QString& dataPath)
{
    if(m_DataManager->setDataPath(dataPath)) {
        assert(m_RenderWidget != nullptr);
        m_DataReader->setRenderData(m_RenderWidget->getParticleDataObj());
        m_DataReader->setMeshObj(m_RenderWidget->getSimMeshObjs());
        m_DataReader->setDataPath(dataPath);

        m_RenderWidget->getParticleDataObj()->resize(0);
        m_RenderWidget->updateNumSimMeshes(0);

        m_RenderWidget->setBox(m_DataManager->getVizData().boxMin, m_DataManager->getVizData().boxMax);
        m_RenderWidget->setCamera(m_DataManager->getVizData().cameraPosition, m_DataManager->getVizData().cameraFocus);

        if(m_DataManager->getVizData().boundaryObjs.size() > 0) {
            auto renderMeshes = m_RenderWidget->getBoundaryMeshObjs();
            for(size_t i = 0; i < m_DataManager->getVizData().boundaryObjs.size(); ++i) {
                auto& mesh = m_DataManager->getVizData().boundaryObjs[i];
                renderMeshes[i]->setVertices((void*)mesh.vertices.data(), mesh.vertices.size() * sizeof(Vec3f));
                renderMeshes[i]->setVertexNormal((void*)mesh.normals.data(), mesh.normals.size() * sizeof(Vec3f));
            }
            m_RenderWidget->updateBoundaryMeshes();
            m_RenderWidget->updateNumBoundaryMeshes(m_DataManager->getVizData().boundaryObjs.size());
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::setupRenderWidgets()
{
    ////////////////////////////////////////////////////////////////////////////////
    // slider
    m_sldFrame = new QSlider(Qt::Horizontal);
    m_sldFrame->setRange(1, 1);
    m_sldFrame->setValue(1);
    m_sldFrame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
//    m_sldFrame->setTracking(false);

    ////////////////////////////////////////////////////////////////////////////////
    QHBoxLayout* inputOutputLayout = new QHBoxLayout;
    setupDataWidgets(inputOutputLayout);

    ////////////////////////////////////////////////////////////////////////////////
    // setup layouts
    QVBoxLayout* renderLayout = new QVBoxLayout;
    renderLayout->addWidget(m_GLWidget, 1);
    renderLayout->addWidget(m_sldFrame);
    renderLayout->addLayout(inputOutputLayout);

    m_Controller = new Controller(this);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addLayout(renderLayout);
    mainLayout->addWidget(m_Controller);

    QWidget* mainWidget = new QWidget(this);
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::setupPlayList()
{
    assert(m_DataList != nullptr);
    m_DataList->setWindowTitle("Data List");

    ////////////////////////////////////////////////////////////////////////////////
    const QString listFile(QDir::currentPath() + "/PlayList.txt");

    if(QFile::exists(listFile)) {
        m_DataList->loadListFromFile(listFile);
    } else {
        qDebug() << "PlayList.txt does not exist. No play list loaded";
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::setupDataWidgets(QLayout* dataLayout)
{
    ////////////////////////////////////////////////////////////////////////////////
    // input data path
    m_InputPath = new BrowsePathWidget(QIcon(":/Icons/open.png"));

    ////////////////////////////////////////////////////////////////////////////////
    // output image path
    m_chkExportFrame = new QCheckBox("Export to Images");
    m_OutputPath     = new BrowsePathWidget(QIcon(":/Icons/save.png"));

    QHBoxLayout* exportImageLayout = new QHBoxLayout;
    exportImageLayout->addWidget(m_chkExportFrame);
    exportImageLayout->addLayout(m_OutputPath->getLayout());

    QGroupBox* grExportImage = new QGroupBox("Image Output Path");
    grExportImage->setLayout(exportImageLayout);

    ////////////////////////////////////////////////////////////////////////////////
    dataLayout->addWidget(m_InputPath->getGroupBox("Input Path"));
    dataLayout->addWidget(grExportImage);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::setupStatusBar()
{
    m_lblStatusCurrentFrame = new QLabel(this);
    m_lblStatusCurrentFrame->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusCurrentFrame, 1);

    m_lblStatusNumFrames = new QLabel(this);
    m_lblStatusNumFrames->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusNumFrames, 1);

    m_lblStatusNumParticles = new QLabel(this);
    m_lblStatusNumParticles->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusNumParticles, 1);

    m_lblStatusReadInfo = new QLabel(this);
    m_lblStatusReadInfo->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusReadInfo, 2);

    m_lblStatusMemoryUsage = new QLabel(this);
    m_lblStatusMemoryUsage->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusMemoryUsage, 1);

    QTimer* memTimer = new QTimer(this);
    connect(memTimer, &QTimer::timeout, [&]
            {
                updateStatusMemoryUsage();
            });
    memTimer->start(5000);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::connectWidgets()
{
    ////////////////////////////////////////////////////////////////////////////////
    // background and floor
    connect(m_Controller->m_cbSkyTexture->getComboBox(), SIGNAL(currentIndexChanged(int)), m_RenderWidget, SLOT(setSkyBoxTexture(int)));
    connect(m_Controller->m_pkrBackgroundColor, &ColorPicker::colorChanged, [&](float r, float g, float b)
            {
                m_RenderWidget->setClearColor(Vec3f(r, g, b));
            });
    connect(m_Controller->m_cbFloorTexture->getComboBox(), SIGNAL(currentIndexChanged(int)), m_RenderWidget, SLOT(setFloorTexture(int)));
    connect(m_Controller->m_sldFloorSize->getSlider(), &QSlider::valueChanged, m_RenderWidget, &RenderWidget::setFloorSize);

    ////////////////////////////////////////////////////////////////////////////////
    // frame params
    connect(m_Controller->m_sldFrameDelay->getSlider(), &QSlider::valueChanged, m_DataReader.get(), &DataReader::setDelayTime);
    connect(m_Controller->m_sldFrameStep->getSlider(), &QSlider::valueChanged, m_DataReader.get(), &DataReader::setFrameStride);

    ////////////////////////////////////////////////////////////////////////////////
    // render modes/colors
//    connect(m_Controller->m_smParticleColorMode, SIGNAL(mapped(int)), m_RenderWidget, SLOT(setParticleColorMode(int)));
    connect(m_Controller->m_msParticleMaterial, &MaterialSelector::materialChanged, m_RenderWidget, &RenderWidget::setParticleMaterial);
//    connect(m_Controller->m_msMeshMaterial, &MaterialSelector::materialChanged,
//            [&](const Material::MaterialData& material)
//            {
//                m_RenderWidget->setMeshMaterial(material, m_Controller->m_cbMeshMaterialID->currentIndex());
//            });
//    connect(m_Controller->m_chkUseAnisotropyKernel, &QCheckBox::toggled, m_RenderWidget, &RenderWidget::enableAnisotropyKernel);
//    connect(m_Controller->m_chkUseAnisotropyKernel, &QCheckBox::toggled, m_DataReader.get(), &DataReader::enableAniKernel);

    ////////////////////////////////////////////////////////////////////////////////
    // buttons
    connect(m_Controller->m_btnReloadTextures, &QPushButton::clicked, m_RenderWidget, &RenderWidget::reloadTextures);
    connect(m_Controller->m_btnReloadTextures, &QPushButton::clicked, m_Controller, &Controller::loadTextures);
    connect(m_Controller->m_btnResetCamera, &QPushButton::clicked, m_RenderWidget, &RenderWidget::resetCameraPosition);
    connect(m_Controller->m_btnPause, &QPushButton::clicked, m_DataReader.get(), &DataReader::pause);
    connect(m_Controller->m_btnNextFrame, &QPushButton::clicked, m_DataReader.get(), &DataReader::readNextFrame);
    connect(m_Controller->m_btnReset, &QPushButton::clicked, m_DataReader.get(), &DataReader::readFirstFrame);
    connect(m_Controller->m_btnRepeatPlay, &QPushButton::clicked, m_DataReader.get(), &DataReader::enableRepeat);
    connect(m_Controller->m_btnReverse, &QPushButton::clicked, m_DataReader.get(), &DataReader::enableReverse);
    connect(m_Controller->m_btnClipViewPlane, &QPushButton::clicked, m_RenderWidget, &RenderWidget::enableClipPlane);

    ////////////////////////////////////////////////////////////////////////////////
    //  data handle
    connect(m_DataManager.get(), &DataManager::numFramesChanged, this, &MainWindow::updateNumFrames);
    connect(m_DataManager.get(), &DataManager::numFramesChanged, m_DataReader.get(), &DataReader::setNumFrames);
    connect(m_InputPath, &BrowsePathWidget::pathChanged, this, &MainWindow::loadVizData);
    connect(m_OutputPath, &BrowsePathWidget::pathChanged, m_RenderWidget, &RenderWidget::setCapturePath);
    connect(m_chkExportFrame, &QCheckBox::toggled, m_RenderWidget, &RenderWidget::enableExportFrame);

    connect(m_DataReader.get(), &DataReader::currentFrameChanged, this, &MainWindow::updateStatusCurrentFrame);
    connect(m_DataReader.get(), &DataReader::currentFrameChanged, m_sldFrame, &QSlider::setValue);
    connect(m_sldFrame,  &QSlider::valueChanged, m_DataReader.get(), &DataReader::readFrame);

    connect(m_DataReader.get(), &DataReader::numParticlesChanged, [&](UInt nParticles) { m_nParticles = nParticles; updateStatusNumParticlesAndMeshes(); });
    connect(m_DataReader.get(), &DataReader::readInfoChanged, this, &MainWindow::updateStatusReadInfo);

    connect(m_DataReader.get(), &DataReader::renderDataChanged, m_RenderWidget, &RenderWidget::updateParticleData);
    connect(m_DataReader.get(), &DataReader::numSimMeshesChanged, [&](UInt nMeshes) { m_nMeshes = nMeshes; updateStatusNumParticlesAndMeshes(); });
    connect(m_DataReader.get(), &DataReader::simMeshesChanged, m_RenderWidget, &RenderWidget::updateSimMeshes);
    connect(m_DataReader.get(), &DataReader::numSimMeshesChanged, m_RenderWidget, &RenderWidget::updateNumSimMeshes);

    connect(m_DataList.get(), &DataList::currentTextChanged, this, &MainWindow::loadVizData);
    connect(m_DataList.get(), &DataList::currentTextChanged, m_InputPath, &BrowsePathWidget::setPath);

    connect(m_ClipPlaneEditor.get(), &ClipPlaneEditor::clipPlaneChanged, m_RenderWidget, &RenderWidget::setClipPlane);

    ////////////////////////////////////////////////////////////////////////////////
    // lights
    connect(m_Controller->m_LightEditor, &PointLightEditor::lightsChanged, m_RenderWidget, &RenderWidget::updateLights);
    connect(m_RenderWidget, &RenderWidget::lightsObjChanged, m_Controller->m_LightEditor, &PointLightEditor::setLights);
    connect(m_Controller->m_chkRenderShadow, &QCheckBox::toggled, m_RenderWidget, &RenderWidget::enableShadow);
    connect(m_Controller->m_chkVisualizeShadowRegion, &QCheckBox::toggled, m_RenderWidget, &RenderWidget::visualizeShadowRegion);
    connect(m_Controller->m_sldShadowIntensity->getSlider(), &QSlider::valueChanged, m_RenderWidget, &RenderWidget::setShadowIntensity);
}