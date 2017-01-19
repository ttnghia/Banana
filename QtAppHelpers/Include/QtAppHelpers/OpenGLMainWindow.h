//------------------------------------------------------------------------------------------
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 09/07/2016
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
//------------------------------------------------------------------------------------------
#ifndef OPENGL_MAINWINDOW_H
#define OPENGL_MAINWINDOW_H

#include <QtWidgets>
#include <QtGui>

#include <QtAppHelpers/OpenGLWidget.h>

//------------------------------------------------------------------------------------------
class OpenGLMainWindow: public QMainWindow
{
    Q_OBJECT

public:
    OpenGLMainWindow(QWidget* parent = 0);

    ~OpenGLMainWindow()
    {
        if(glWidget != nullptr)
        {
            delete glWidget;
        }
    }

    bool eventFilter(QObject* obj, QEvent* event);
    virtual void processKeyPressEvent(QKeyEvent* event);
    virtual void processKeyReleaseEvent(QKeyEvent* event);

public slots:
    void updateFrameRate(double avgFrameTime);

protected:
    virtual void instantiateOpenGLWidget() = 0;
    void setupOpenglWidget(OpenGLWidget* _glWidget);

    OpenGLWidget* glWidget;
    QLabel* lblStatusFPS;
};

#endif // OPENGL_MAINWINDOW_H
