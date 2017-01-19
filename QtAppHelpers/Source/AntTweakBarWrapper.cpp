#include <QtAppHelpers/DeviceToLogical.h>
#include <QtAppHelpers/AntTweakBarWrapper.h>

//------------------------------------------------------------------------------------------
int AntTweakBarWrapper::TwMousePressQt(QMouseEvent* e)
{
    TwMouseMotion(e->x(), e->y());
    return TwMouseButton(TW_MOUSE_PRESSED, Qt2TwMouseButtonId(e));
}

//------------------------------------------------------------------------------------------
int AntTweakBarWrapper::TwMousePressQt(QWidget* qw, QMouseEvent* e)
{
    TwMouseMotion(QTLogicalToDevice(qw, e->x()), QTLogicalToDevice(qw, e->y()));
    return TwMouseButton(TW_MOUSE_PRESSED, Qt2TwMouseButtonId(e));
}

//------------------------------------------------------------------------------------------
int AntTweakBarWrapper::TwMouseReleaseQt(QMouseEvent* e)
{
    //	TwMouseMotion(e->x (), e->y ());
    return TwMouseButton(TW_MOUSE_RELEASED, Qt2TwMouseButtonId(e));
}

//------------------------------------------------------------------------------------------
int AntTweakBarWrapper::TwMouseReleaseQt(QWidget* qw, QMouseEvent* e)
{
    (void)qw;
    //	TwMouseMotion(QTLogicalToDevice(qw, e->x()), QTLogicalToDevice(qw, e->y()));
    return TwMouseButton(TW_MOUSE_RELEASED, Qt2TwMouseButtonId(e));
}

//------------------------------------------------------------------------------------------
int AntTweakBarWrapper::TwMouseMotionQt(QMouseEvent* e)
{
    return TwMouseMotion(e->x(), e->y());
}

//------------------------------------------------------------------------------------------
int AntTweakBarWrapper::TwMouseMotionQt(QWidget* qw, QMouseEvent* e)
{
    return TwMouseMotion(QTLogicalToDevice(qw, e->x()), QTLogicalToDevice(qw, e->y()));
}

//------------------------------------------------------------------------------------------
int AntTweakBarWrapper::TwKeyPressQt(QKeyEvent* e)
{
    int kmod = 0;

    if(e->modifiers() & Qt::ShiftModifier)
    {
        kmod |= TW_KMOD_SHIFT;
    }

    if(e->modifiers() & Qt::ControlModifier)
    {
        kmod |= TW_KMOD_CTRL;
    }

    if(e->modifiers() & Qt::AltModifier)
    {
        kmod |= TW_KMOD_ALT;
    }

    int key = e->key();
    int k = 0;

    if(key > 0 && key < 0x7e)
    {
        k = key;    // plain ascii codes
    }

    if(key >= Qt::Key_F1 && key <= Qt::Key_F12)
    {
        k = TW_KEY_F1 + (key - Qt::Key_F1);
    }
    else
    {
        switch(key)
        {
            case Qt::Key_Left:
                k = TW_KEY_LEFT;
                break;

            case Qt::Key_Up:
                k = TW_KEY_UP;
                break;

            case Qt::Key_Right:
                k = TW_KEY_RIGHT;
                break;

            case Qt::Key_Down:
                k = TW_KEY_DOWN;
                break;

            case Qt::Key_PageUp:
                k = TW_KEY_PAGE_UP;
                break;

            case Qt::Key_PageDown:
                k = TW_KEY_PAGE_DOWN;
                break;

            case Qt::Key_Home:
                k = TW_KEY_HOME;
                break;

            case Qt::Key_End:
                k = TW_KEY_END;
                break;

            case Qt::Key_Insert:
                k = TW_KEY_INSERT;
                break;

            case Qt::Key_Backspace:
                k = TW_KEY_BACKSPACE;
                break;

            case Qt::Key_Delete:
                k = TW_KEY_DELETE;
                break;

            case Qt::Key_Return:
                k = TW_KEY_RETURN;
                break;

            case Qt::Key_Enter:
                k = TW_KEY_RETURN;
                break;

            case Qt::Key_Escape:
                k = TW_KEY_ESCAPE;
                break;

            case Qt::Key_Tab:
                k = TW_KEY_TAB;
                break;

            case Qt::Key_Space:
                k = TW_KEY_SPACE;
                break;
        }
    }

    return TwKeyPressed(k, kmod);
}

//------------------------------------------------------------------------------------------
void AntTweakBarWrapper::initializeAntTweakBar()
{
    TwInit(TW_OPENGL_CORE, NULL);
}

//------------------------------------------------------------------------------------------
void AntTweakBarWrapper::resizeAntTweakBarWindow(int width, int height)
{
    TwWindowSize(width, height);
}

//------------------------------------------------------------------------------------------
void AntTweakBarWrapper::renderAntTweakBar()
{
    if(antTweakBar != nullptr)
    {
        TwDraw();
    }
}

//------------------------------------------------------------------------------------------
void AntTweakBarWrapper::shutDownAntTweakBar()
{
    TwTerminate();
}

//------------------------------------------------------------------------------------------
TwMouseButtonID AntTweakBarWrapper::Qt2TwMouseButtonId(QMouseEvent* e)
{
    if(e->button() && Qt::LeftButton)
    {
        return TW_MOUSE_LEFT;
    }

    if(e->button() && Qt::MidButton)
    {
        return TW_MOUSE_MIDDLE;
    }

    if(e->button() && Qt::RightButton)
    {
        return TW_MOUSE_RIGHT;
    }

    return TW_MOUSE_LEFT;
    //  assert(0);
}