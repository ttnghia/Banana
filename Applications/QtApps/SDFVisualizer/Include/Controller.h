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

#include <QtAppHelpers/BrowsePathWidget.h>
#include <QtAppHelpers/MaterialSelector.h>
#include <QtAppHelpers/EnhancedComboBox.h>
#include <QtAppHelpers/EnhancedSlider.h>
#include <QtAppHelpers/BrowsePathWidget.h>
#include <QtAppHelpers/PointLightEditor.h>

#include "Common.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Controller : public QWidget
{
    Q_OBJECT
    friend class MainWindow;
public:
    explicit Controller(QWidget* parent) : QWidget(parent)
    {
        setupGUI();
    }

//    void disableParameters(bool disable);

public slots:
    void loadTextures();

private:
    void setupGUI();
    void setupTextureControllers(QBoxLayout* ctrLayout);
    void setupColorControllers(QBoxLayout* ctrLayout);
    void setupGridResolutionControllers(QBoxLayout* ctrLayout);
    void setupParticleSizeControllers(QBoxLayout* ctrLayout);
    void setupParticleDisplayControllers(QBoxLayout* ctrLayout);
    void setupSDFObjectControllers(QBoxLayout* ctrLayout);
    void setupButtons(QBoxLayout* ctrLayout);

    ////////////////////////////////////////////////////////////////////////////////
    EnhancedComboBox* m_cbSkyTexture;
    MaterialSelector* m_msNegativeParticleMaterial;
    MaterialSelector* m_msPositiveParticleMaterial;
    EnhancedComboBox* m_cbResolution;
    QSlider*          m_slNegativeParticleSize;
    QSlider*          m_slPositiveParticleSize;
    QCheckBox*        m_chkHideNegativeParticles;
    QCheckBox*        m_chkHidePositiveParticles;
    EnhancedComboBox* m_cbSDFObject;

    QPushButton* m_btnEditClipPlane;
    QPushButton* m_btnEnableClipPlane;

    PointLightEditor* m_LightEditor;
};