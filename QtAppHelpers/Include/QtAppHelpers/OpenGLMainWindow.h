//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <QtWidgets>
#include <QtGui>

#include <QtAppHelpers/OpenGLWidget.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OpenGLMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    OpenGLMainWindow(QWidget * parent)
        : QMainWindow(parent), glWidget(nullptr)
    {
        qApp->installEventFilter(this);
        //    connect(glWidget, &OpenGLWidget::frameRateChanged, this, &OpenGLMainWindow::updateFrameRate);

        lblStatusFPS = new QLabel(this);
        lblStatusFPS->setMargin(5);
        statusBar()->addPermanentWidget(lblStatusFPS);
        statusBar()->setMinimumHeight(30);
        statusBar()->setSizeGripEnabled(false);
    }

    ~OpenGLMainWindow()
    {
        delete glWidget;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    bool eventFilter(QObject* obj, QEvent* event)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

            processKeyPressEvent(keyEvent);
        }
        else if(event->type() == QEvent::KeyRelease)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

            processKeyReleaseEvent(keyEvent);
        }

        return QObject::eventFilter(obj, event);
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void processKeyPressEvent(QKeyEvent* event)
    {
        switch(event->key())
        {
            case Qt::Key_Escape:
                exit(EXIT_SUCCESS);

            default:
                if(glWidget != nullptr)
                {
                    glWidget->keyPressEvent(event);
                }
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void processKeyReleaseEvent(QKeyEvent* event)
    {
        (void)event;

    }

    public slots:
    void updateFrameRate(double avgFrameTime);

protected:
    virtual void instantiateOpenGLWidget() = 0;

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void updateFrameRate(double avgFrameTime)
    {
        lblStatusFPS->setText(QString("Frame time: %1ms (~ %2 FPS)")
                              .arg(avgFrameTime).arg(1000.0 / avgFrameTime));
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setupOpenglWidget(OpenGLWidget* _glWidget)
    {
        if(glWidget != nullptr)
        {
            delete glWidget;
        }

        glWidget = _glWidget;
        setCentralWidget(glWidget);
        //    connect(glWidget, &OpenGLWidget::frameRateChanged, this,
        //            &OpenGLMainWindow::updateFrameRate);
        connect(&glWidget->FPSTimer, &AvgTimer::avgTimeChanged, this,
                &OpenGLMainWindow::updateFrameRate);
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    OpenGLWidget* glWidget;
    QLabel* lblStatusFPS;
};
