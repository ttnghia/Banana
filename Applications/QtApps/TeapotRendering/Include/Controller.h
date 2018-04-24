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

#pragma once

#include <QWidget>
#include <QtWidgets>

#include <QtAppHelpers/MaterialSelector.h>
#include <QtAppHelpers/EnhancedComboBox.h>
#include <QtAppHelpers/EnhancedSlider.h>
#include <QtAppHelpers/PointLightEditor.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
using namespace Banana;
class Controller : public QWidget
{
    Q_OBJECT
    friend class MainWindow;
public:
    explicit Controller(QWidget* parent) :
        QWidget(parent)
    {
        setupGUI();
    }

public slots:
    void loadTextures();

private:
    void setupGUI();
    void setupTextureControllers(QBoxLayout* ctrLayout);
    void setupMaterialControllers(QBoxLayout* ctrLayout);
    void setupButtons(QBoxLayout* ctrLayout);

    ////////////////////////////////////////////////////////////////////////////////
    EnhancedComboBox* m_cbSkyTexture;
    EnhancedComboBox* m_cbFloorTexture;
    EnhancedSlider*   m_sldFloorSize;
    EnhancedSlider*   m_sldFloorTexScale;
    EnhancedSlider*   m_sldFloorExposure;

    MaterialSelector* m_msMeshMaterial;
    QComboBox*        m_cbMeshMaterialID;
    PointLightEditor* m_LightEditor;

    QPushButton* m_btnResetCamera;
    QPushButton* m_btnExportImage;
    QPushButton* m_btnReloadTextures;
};