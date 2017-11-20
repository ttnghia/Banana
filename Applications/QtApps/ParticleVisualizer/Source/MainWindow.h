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

#include "RenderWidget.h"
#include "Controller.h"
#include "DataManager.h"
#include "DataReader.h"

#include <QtAppHelpers/OpenGLMainWindow.h>
#include <QtAppHelpers/BrowsePathWidget.h>
#include <QtAppHelpers/OpenGLWidgetTestRender.h>
#include <QtAppHelpers/DataList.h>
#include <QtAppHelpers/ClipPlaneEditor.h>

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
    void updateStatusCurrentFrame(int currentFrame);
    void updateStatusNumParticlesAndMeshes();
    void updateNumFrames(int numFrames);
    void updateStatusReadInfo(double readTime, size_t bytes);
    void updateStatusMemoryUsage();
    void loadVizData(const QString& dataPath);

private:
    void setupRenderWidgets();
    void setupPlayList();
    void setupDataWidgets(QLayout* dataLayout);
    void setupStatusBar();
    void connectWidgets();

    ////////////////////////////////////////////////////////////////////////////////
    UInt m_nParticles = 0;
    UInt m_nMeshes    = 0;

    RenderWidget*     m_RenderWidget   = nullptr;
    Controller*       m_Controller     = nullptr;
    QSlider*          m_sldFrame       = nullptr;
    QCheckBox*        m_chkExportFrame = nullptr;
    BrowsePathWidget* m_InputPath      = nullptr;
    BrowsePathWidget* m_OutputPath     = nullptr;

    QLabel* m_lblStatusNumParticles = nullptr;
    QLabel* m_lblStatusCurrentFrame = nullptr;
    QLabel* m_lblStatusNumFrames    = nullptr;
    QLabel* m_lblStatusReadInfo     = nullptr;
    QLabel* m_lblStatusMemoryUsage  = nullptr;

    ClipPlaneEditor* m_ClipPlaneEditor = new ClipPlaneEditor();
    DataReader*      m_DataReader      = new DataReader();
    DataList*        m_DataList        = new DataList(nullptr, true, true);
};