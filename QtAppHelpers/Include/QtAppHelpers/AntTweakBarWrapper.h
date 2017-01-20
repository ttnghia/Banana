#ifndef __ANTTWEAKBAR_WRAPPER_H__
#define __ANTTWEAKBAR_WRAPPER_H__

#include <string>
#include <QWidget>
#include <QtGui>
#include <AntTweakBar.h>
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class AntTweakBarWrapper
{
public:
    AntTweakBarWrapper() : antTweakBar(nullptr)
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

    TwBar* antTweakBar;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#endif // __ANTTWEAKBAR_WRAPPER_H__
