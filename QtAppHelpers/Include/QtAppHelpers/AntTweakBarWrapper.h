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

#include <string>
#include <QWidget>
#include <QtGui>
#include <AntTweakBar.h>
#include <QtAppHelpers/DeviceToLogical.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
