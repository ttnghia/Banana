//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/21/2017
//            `)  ('                                                    Author: Nghia Truong
//         ,  /::::\  ,
//         |'.\::::/.'|
//        _|  ';::;'  |_
//       (::)   ||   (::)                       _.
//        "|    ||    |"                      _(:)
//         '.   ||   .'                       /::\
//           '._||_.'                         \::/
//            /::::\                         /:::\
//            \::::/                        _\:::/
//             /::::\_.._  _.._  _.._  _.._/::::\
//             \::::/::::\/::::\/::::\/::::\::::/
//               `""`\::::/\::::/\::::/\::::/`""`
//                    `""`  `""`  `""`  `""`
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <string>
#include <QWidget>
#include <QtGui>
#include <AntTweakBar.h>
#include <QtAppHelpers/DeviceToLogical.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class AntTweakBarWrapper
{
public:
    AntTweakBarWrapper() : m_AntTweakBar(nullptr)
    {}

    int TwMousePressQt(QMouseEvent* e);
    int TwMouseReleaseQt(QMouseEvent* e);
    int TwMouseMotionQt(QMouseEvent* e);
    int TwMousePressQt(QWidget* qw, QMouseEvent* e);
    int TwMouseReleaseQt(QWidget* qw, QMouseEvent* e);
    int TwMouseMotionQt(QWidget* qw, QMouseEvent* e);

    int TwKeyPressQt(QKeyEvent* e);

    void initializeAntTweakBar();
    void resizeAntTweakBarWindow(int width, int height);
    void renderAntTweakBar();
    void shutDownAntTweakBar();

protected:
    virtual void setupTweakBar()
    {}

    TwMouseButtonID Qt2TwMouseButtonId(QMouseEvent* e);

    ////////////////////////////////////////////////////////////////////////////////
    TwBar* m_AntTweakBar;
};
