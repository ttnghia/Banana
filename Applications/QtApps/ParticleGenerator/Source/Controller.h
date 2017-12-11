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
#include <QtAppHelpers/OpenGLController.h>

#include "Common.h"
#include "RenderWidget.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Controller : public OpenGLController
{
    Q_OBJECT
    friend class MainWindow;
public:
    explicit Controller(RenderWidget* renderWidget, QWidget* parent = nullptr) :
        OpenGLController(static_cast<OpenGLWidget*>(renderWidget), parent), m_RenderWidget(renderWidget)
    {
        setupGUI();
        connectWidgets();
    }

signals:
    void transformationChanged(const Vec3f& translation, const Vec4f& rotation, float uniformScale);

private:
    void setupGUI();
    void connectWidgets();

    ////////////////////////////////////////////////////////////////////////////////
    // main objects
    RenderWidget* m_RenderWidget = nullptr;
    ////////////////////////////////////////////////////////////////////////////////

    void setupColorControllers();
    void setupGridResolutionControllers();
    void setupParticleSizeControllers();
    void setupParticleDisplayControllers();
    void setupObjectTransformationControllers();
    void setupSDFObjectControllers();
    void setupButtons();

    ////////////////////////////////////////////////////////////////////////////////
    MaterialSelector* m_msNegativeParticleMaterial;
    MaterialSelector* m_msPositiveParticleMaterial;
    EnhancedComboBox* m_cbResolution;
    QSlider*          m_slNegativeParticleSize;
    QSlider*          m_slPositiveParticleSize;
    QCheckBox*        m_chkHideNegativeParticles;
    QCheckBox*        m_chkHidePositiveParticles;
    QLineEdit*        m_txtTranslationX;
    QLineEdit*        m_txtTranslationY;
    QLineEdit*        m_txtTranslationZ;
    QLineEdit*        m_txtRotationAxisX;
    QLineEdit*        m_txtRotationAxisY;
    QLineEdit*        m_txtRotationAxisZ;
    QLineEdit*        m_txtRotationAngle;
    QLineEdit*        m_txtUniformScale;
    QPushButton*      m_btnApplyTransform;
    EnhancedComboBox* m_cbSDFObject;

    QPushButton* m_btnEditClipPlane;
    QPushButton* m_btnEnableClipPlane;
};