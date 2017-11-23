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
    ////////////////////////////////////////////////////////////////////////////////
    setArthurStyle();
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle("Particle Visualizer");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::instantiateOpenGLWidget()
{
    if(m_GLWidget != nullptr) {
        delete m_GLWidget;
    }
    m_RenderWidget = new RenderWidget(this, m_DataReader->getVizData());
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
            m_CapturePath->browse();
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
            m_DataReader->setDataPath(m_InputPath->getCurrentPath());
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
void MainWindow::updateWindowTitle(const QString& filePath)
{
    setWindowTitle(QString("Particle Visualizer: ") + filePath);
}

void MainWindow::updateStatusCurrentFrame(int currentFrame)
{
    m_lblStatusCurrentFrame->setText(QString("Current frame: %1").arg(currentFrame));
}

void MainWindow::updateStatusNumParticlesAndMeshes()
{
//    m_lblStatusNumParticles->setText(QString("Particles: %1 | Mesh(es): %2")
//                                         .arg(QString::fromStdString(NumberHelpers::formatWithCommas(m_nParticles)))
//                                         .arg(QString::fromStdString(NumberHelpers::formatWithCommas(m_nMeshes))));
    m_lblStatusNumParticles->setText(QString("Particles: %1").arg(QString::fromStdString(NumberHelpers::formatWithCommas(m_nParticles))));
}

void MainWindow::updateNumFrames(int numFrames)
{
    m_sldFrame->setRange(0, numFrames - 1);
    m_lblStatusNumFrames->setText(QString("Total frame: %1").arg(numFrames));
}

void MainWindow::updateStatusReadInfo(double readTime, size_t bytes)
{
    m_lblStatusReadInfo->setText(QString("Load data: %1 (ms) | %2 (MBs)").arg(readTime).arg(static_cast<double>(bytes) / 1048576.0));
}

void MainWindow::updateStatusMemoryUsage()
{
    m_lblStatusMemoryUsage->setText(QString("Memory usage: %1 (MBs)").arg(QString::fromStdString(NumberHelpers::formatWithCommas(getCurrentRSS() / 1048576.0))));
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

    ////////////////////////////////////////////////////////////////////////////////
    QHBoxLayout* inputOutputLayout = new QHBoxLayout;
    setupDataWidgets(inputOutputLayout);

    ////////////////////////////////////////////////////////////////////////////////
    // setup layouts
    QVBoxLayout* renderLayout = new QVBoxLayout;
    renderLayout->addWidget(m_GLWidget, 1);
    renderLayout->addWidget(m_sldFrame);
    renderLayout->addLayout(inputOutputLayout);

    m_Controller = new Controller(m_RenderWidget, m_DataReader, this);

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
    m_chkCaptureFrame = new QCheckBox("Export to Images");
    m_CapturePath     = new BrowsePathWidget(QIcon(":/Icons/save.png"));
    m_CapturePath->setPath(getCapturePath());
    QHBoxLayout* exportImageLayout = new QHBoxLayout;
    exportImageLayout->addWidget(m_chkCaptureFrame);
    exportImageLayout->addLayout(m_CapturePath->getLayout());

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
    statusBar()->addPermanentWidget(m_lblStatusReadInfo, 1);

    m_lblStatusMemoryUsage = new QLabel(this);
    m_lblStatusMemoryUsage->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusMemoryUsage, 1);

    QTimer* memTimer = new QTimer(this);
    connect(memTimer, &QTimer::timeout, [&] { updateStatusMemoryUsage(); });
    memTimer->start(5000);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::connectWidgets()
{
    connect(m_InputPath,  &BrowsePathWidget::pathChanged, m_DataReader, &DataReader::setDataPath);
    connect(m_CapturePath,  &BrowsePathWidget::pathChanged, m_RenderWidget, &RenderWidget::setCapturePath);
    connect(m_chkCaptureFrame, &QCheckBox::toggled, m_RenderWidget, &RenderWidget::setCaptureFrame);

    connect(m_sldFrame,   &QSlider::sliderMoved, m_DataReader, &DataReader::readFrame);

    connect(m_DataReader, &DataReader::currentFrameChanged,  m_sldFrame, &QSlider::setValue);
    connect(m_DataReader, &DataReader::currentFrameChanged,  this,       &MainWindow::updateStatusCurrentFrame);
    connect(m_DataReader, &DataReader::frameReadInfoChanged, this,       &MainWindow::updateStatusReadInfo);
    connect(m_DataReader, &DataReader::numFramesChanged,     this,       &MainWindow::updateNumFrames);
    connect(m_DataReader, &DataReader::numParticlesChanged,              [&](UInt nParticles) { m_nParticles = nParticles; updateStatusNumParticlesAndMeshes(); });

    connect(m_DataList, &DataList::currentTextChanged, m_DataReader, &DataReader::setDataPath);
    connect(m_DataList, &DataList::currentTextChanged, m_InputPath,  &BrowsePathWidget::setPath);
    connect(m_DataList, &DataList::currentTextChanged,               [&](const QString& dataPath) { updateWindowTitle(dataPath); });

    connect(m_ClipPlaneEditor, &ClipPlaneEditor::clipPlaneChanged, m_RenderWidget, &RenderWidget::setClipPlane);
}