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

#include <QStringList>
#include <QString>
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
        __BNN_ASSERT(m_RenderWidget != nullptr);
        __BNN_ASSERT(m_DataReader != nullptr);
        setupGUI();
        connectWidgets();
    }

public slots:
    void loadTextures();

private:
    void setupGUI();
    void connectWidgets();

    QStringList getTextureFolders(QString texType);
    QStringList getTextureFiles(QString texType);

    ////////////////////////////////////////////////////////////////////////////////
    // main objects
    RenderWidget* m_RenderWidget = nullptr;
    DataReader*   m_DataReader   = nullptr;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // background and floor
    void setupTextureControllers(QBoxLayout* layoutCtr);
    EnhancedComboBox* m_cbSkyTexture;
    EnhancedComboBox* m_cbFloorTexture;
    EnhancedSlider*   m_sldFloorSize;
    EnhancedSlider*   m_sldFloorExposure;
    ColorPicker*      m_pkrBackgroundColor;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // frame controller
    void setupFrameControllers(QBoxLayout* layoutCtr);
    EnhancedSlider* m_sldFrameStep;
    EnhancedSlider* m_sldFrameDelay;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // ani-kernel
    void setupAniKernel(QBoxLayout* layoutCtr);
    QCheckBox* m_chkUseAniKernel;
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
    QListWidget*   m_lstParticleData;
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