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

#include <QStringList>
#include <QDir>
#include <QWidget>
#include <QtWidgets>

#include <QtAppHelpers/MaterialSelector.h>
#include <QtAppHelpers/EnhancedComboBox.h>
#include <QtAppHelpers/EnhancedSlider.h>
#include <QtAppHelpers/PointLightEditor.h>

#include "RenderWidget.h"
#include "DataReader.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Controller : public QWidget
{
    Q_OBJECT
    friend class MainWindow;
public:
    explicit Controller(RenderWidget* renderWidget, DataReader* dataReader, QWidget* parent = nullptr) :
        QWidget(parent), m_RenderWidget(renderWidget), m_DataReader(dataReader)
    {
        __BNN_REQUIRE(m_RenderWidget != nullptr);
        __BNN_REQUIRE(m_DataReader != nullptr);
        setupGUI();
        connectWidgets();
    }

private:
    void        setupGUI();
    void        connectWidgets();
    QStringList getTextureFolders(const QString& texType, const QString& texPath);
    QStringList getTextureFiles(const QString& texType, const QString& texPath);
    void        loadTextures();

    ////////////////////////////////////////////////////////////////////////////////
    // main objects
    RenderWidget* m_RenderWidget = nullptr;
    DataReader*   m_DataReader   = nullptr;
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
    // frame controller
    void setupFrameControllers(QBoxLayout* layoutCtr);
    EnhancedSlider* m_sldFrameStride;
    EnhancedSlider* m_sldFrameDelay;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material
    void setupMaterialControllers(QBoxLayout* layoutCtr);
    MaterialSelector* m_msParticleMaterial;
    MaterialSelector* m_msMeshMaterial;
    QComboBox*        m_cbMeshMaterialID;
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
    QCheckBox*   m_chkUseAniKernel;
    QCheckBox*   m_chkRenderBox;
    ColorPicker* m_pkrBoxColor;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // buttons
    void setupButtons(QBoxLayout* layoutCtr);
    QPushButton* m_btnPause;
    QPushButton* m_btnNextFrame;
    QPushButton* m_btnReset;
    QPushButton* m_btnReverse;
    QPushButton* m_btnRepeatPlay;
    QPushButton* m_btnClipViewPlane;
    QPushButton* m_btnResetCamera;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // light
    PointLightEditor* m_LightEditor;
    ////////////////////////////////////////////////////////////////////////////////
};