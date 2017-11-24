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

#pragma once

#include <QWidget>
#include <QtWidgets>

#include <QtAppHelpers/BrowsePathWidget.h>
#include <QtAppHelpers/MaterialSelector.h>
#include <QtAppHelpers/EnhancedComboBox.h>
#include <QtAppHelpers/EnhancedSlider.h>
#include <QtAppHelpers/BrowsePathWidget.h>
#include <QtAppHelpers/PointLightEditor.h>

#include "Common.h"
#include "RenderWidget.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Controller : public QWidget
{
    Q_OBJECT
    friend class MainWindow;
public:
    explicit Controller(RenderWidget* renderWidget, QWidget* parent = nullptr) : QWidget(parent), m_RenderWidget(renderWidget)
    {
        __BNN_ASSERT(m_RenderWidget != nullptr);
        setupGUI();
        connectWidgets();
    }

private:
    void setupGUI();
    void connectWidgets();

    QStringList getTextureFolders(const QString& texType, const QString& texPath);
    QStringList getTextureFiles(const QString& texType, const QString& texPath);
    void        loadTextures();

    ////////////////////////////////////////////////////////////////////////////////
    // main objects
    RenderWidget* m_RenderWidget = nullptr;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // background
    void setupBackgroundControllers(QBoxLayout* layoutCtr);
    QSignalMapper*    m_smBackgroundMode;
    EnhancedComboBox* m_cbSkyTexture;
    EnhancedSlider*   m_sldCheckerboardScale;
    ColorPicker*      m_pkrCheckerColor1;
    ColorPicker*      m_pkrCheckerColor2;
    EnhancedSlider*   m_sldGridScale;
    ColorPicker*      m_pkrGridBackgroundColor;
    ColorPicker*      m_pkrGridLineColor;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // floor
    void setupFloorControllers(QBoxLayout* layoutCtr);
    EnhancedComboBox* m_cbFloorTexture;
    EnhancedSlider*   m_sldFloorSize;
    EnhancedSlider*   m_sldFloorExposure;
    ColorPicker*      m_pkrBackgroundColor;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material
    void setupMaterialControllers(QBoxLayout* layoutCtr);
    MaterialSelector* m_msParticleMaterial;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // color mode
    void setupColorModeControllers(QBoxLayout* layoutCtr);
    QSignalMapper* m_smParticleColorMode;
    ColorPicker*   m_pkrColorDataMin;
    ColorPicker*   m_pkrColorDataMax;
    QPushButton*   m_btnRndColor;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // misc
    void setupMiscControllers(QBoxLayout* layoutCtr);
    QCheckBox*   m_chkRenderBox;
    ColorPicker* m_pkrBoxColor;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // simulation controllers
    void        setupSimulationControllers(QBoxLayout* layoutCtr);
    QStringList getSceneFiles();
    QComboBox*        m_cbSimulationScene;
    QCheckBox*        m_chkEnableOutput;
    BrowsePathWidget* m_OutputPath;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // buttons
    void setupButtons(QBoxLayout* layoutCtr);
    QPushButton* m_btnStartStopSimulation;
    QPushButton* m_btnResetCamera;
    QPushButton* m_btnClipViewPlane;
    QPushButton* m_btnEditClipPlane;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // light
    PointLightEditor* m_LightEditor;
    ////////////////////////////////////////////////////////////////////////////////
};