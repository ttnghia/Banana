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

#include "RenderWidget.h"
#include "Controller.h"
#include "SDFGrid.h"

#include <QtAppHelpers/OpenGLMainWindow.h>
#include <QtAppHelpers/BrowsePathWidget.h>
#include <QtAppHelpers/ClipPlaneEditor.h>
#include <QtAppHelpers/BusyBar.h>

#include <QEvent>
#include <memory>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MainWindow : public OpenGLMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);

protected:
    virtual void instantiateOpenGLWidget();
    virtual bool processKeyPressEvent(QKeyEvent* event) override;
    virtual void showEvent(QShowEvent* ev);

public slots:
    void updateStatusMemoryUsage();
    void updateStatusSDFGenerationTime(double runTime);
    void finilizeSDFGeneration();

private:
    void setupRenderWidgets();
    void setupStatusBar();
    void connectWidgets();

    ////////////////////////////////////////////////////////////////////////////////
    RenderWidget* m_RenderWidget               = nullptr;
    Controller*   m_Controller                 = nullptr;
    QLabel*       m_lblStatusMemoryUsage       = nullptr;
    QLabel*       m_lblStatusSDFGenerationTime = nullptr;
    BusyBar*      m_BusyBar;

    std::unique_ptr<SDFGrid>         m_SDFGrid         = nullptr;
    std::unique_ptr<ClipPlaneEditor> m_ClipPlaneEditor = std::make_unique<ClipPlaneEditor>();
};