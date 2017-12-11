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

#include <QtAppHelpers/OpenGLWidget.h>
#include <QtAppHelpers/PointLightEditor.h>
#include <QtAppHelpers/EnhancedComboBox.h>
#include <QtAppHelpers/EnhancedSlider.h>
#include <QtWidgets>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OpenGLController : public QWidget
{
    Q_OBJECT
    friend class OpenGLMainWindow;
public:
    explicit OpenGLController(OpenGLWidget* renderWidget, QWidget* parent, Int width = 350,
                              bool bShowBackgroundControllers = true, bool bShowFloorControllers = true, bool bShowBoxControllers = true) :
        QWidget(parent), m_GLWidget(renderWidget),
        m_bShowBackgroundControllers(bShowBackgroundControllers), m_bShowFloorControllers(bShowFloorControllers), m_bShowBoxControllers(bShowBoxControllers)
    {
        __BNN_REQUIRE(m_GLWidget != nullptr);
        setupBasicGUI(width);
        connectBasicWidgets();
        if(!m_bShowBackgroundControllers) { m_grBackgroundCtrl->setVisible(false); }
        if(!m_bShowFloorControllers) { m_grFloorCtrl->setVisible(false); }
        if(!m_bShowBoxControllers) { m_grBoxCtrl->setVisible(false); }
    }

public slots:
    void showBackgroundControllers(bool bShow) { m_grBackgroundCtrl->setVisible(bShow); }
    void showFloorControllers(bool bShow) { m_grFloorCtrl->setVisible(bShow); }
    void showBoxControllers(bool bShow) { m_grBoxCtrl->setVisible(bShow); }
    void reloadTextures() { loadSkyBoxTextures(); loadFloorTextures(); }
    void loadSkyBoxTextures();
    void loadFloorTextures();

protected:
    void setupBasicGUI(Int width);
    void connectBasicWidgets();

    ////////////////////////////////////////////////////////////////////////////////
    // background
    void setupBackgroundControllers();
    bool              m_bShowBackgroundControllers = true;
    EnhancedComboBox* m_cbSkyTexture               = new EnhancedComboBox;
    QSignalMapper*    m_smBackgroundMode           = new QSignalMapper;
    EnhancedSlider*   m_sldCheckerboardScale       = new EnhancedSlider;
    ColorPicker*      m_pkrCheckerColor1           = new ColorPicker;
    ColorPicker*      m_pkrCheckerColor2           = new ColorPicker;
    EnhancedSlider*   m_sldGridScale               = new EnhancedSlider;
    ColorPicker*      m_pkrGridBackgroundColor     = new ColorPicker;
    ColorPicker*      m_pkrGridLineColor           = new ColorPicker;
    QGroupBox*        m_grBackgroundCtrl           = new QGroupBox("Background");
    ////////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////////
    // floor
    void setupFloorControllers();
    bool              m_bShowFloorControllers = true;
    EnhancedComboBox* m_cbFloorTexture        = new EnhancedComboBox;
    EnhancedSlider*   m_sldFloorSize          = new EnhancedSlider;
    EnhancedSlider*   m_sldFloorExposure      = new EnhancedSlider;
    ColorPicker*      m_pkrBackgroundColor    = new ColorPicker;
    QGroupBox*        m_grFloorCtrl           = new QGroupBox("Floor Texture");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // box
    void setupBoxControllers();
    bool         m_bShowBoxControllers = true;
    QCheckBox*   m_chkRenderBox        = new QCheckBox("Render box");
    ColorPicker* m_pkrBoxColor         = new ColorPicker;
    QGroupBox*   m_grBoxCtrl           = new QGroupBox("Box");
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    OpenGLWidget*     m_GLWidget                = nullptr;
    QTabWidget*       m_MainTab                 = new QTabWidget;
    QVBoxLayout*      m_MainLayout              = new QVBoxLayout;
    PointLightEditor* m_LightEditor             = nullptr;
    QWidget*          m_RenderControllers       = new QWidget;
    QVBoxLayout*      m_LayoutRenderControllers = new QVBoxLayout;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana
