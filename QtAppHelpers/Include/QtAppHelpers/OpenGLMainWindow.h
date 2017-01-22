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
#include <arthurstyle.h>
#include <QtAppHelpers/OpenGLWidget.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OpenGLMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    OpenGLMainWindow(QWidget* parent)
        : QMainWindow(parent), m_GlWidget(nullptr)
    {
        qApp->installEventFilter(this);

        m_LBLStatusFPS = new QLabel(this);
        m_LBLStatusFPS->setMargin(5);
        statusBar()->addPermanentWidget(m_LBLStatusFPS);
        statusBar()->setMinimumHeight(30);
        statusBar()->setSizeGripEnabled(false);
    }

    ~OpenGLMainWindow()
    {
        delete m_GlWidget;
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
                if(m_GlWidget != nullptr)
                {
                    m_GlWidget->keyPressEvent(event);
                }
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    virtual void processKeyReleaseEvent(QKeyEvent* event)
    {
        (void)event;

    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setArthurStyle()
    {
        QStyle* arthurStyle = new ArthurStyle();
        setStyle(arthurStyle);
        QList<QWidget*> widgets = findChildren<QWidget*>();

        foreach(QWidget* w, widgets)
        {
            QString className = QString(w->metaObject()->className());

            if(className == "QScrollBar" || className == "QComboBox" ||
               className == "QCheckBox")
            {
                continue;
            }

            w->setStyle(arthurStyle);
        }
    }

    public slots:
    void updateFrameRate(double avgFrameTime)
    {
        m_LBLStatusFPS->setText(QString("Frame time: %1ms (~ %2 FPS)")
                                .arg(avgFrameTime).arg(1000.0 / avgFrameTime));
    }

protected:
    virtual void instantiateOpenGLWidget() = 0;

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setupOpenglWidget(OpenGLWidget* glWidget)
    {
        if(m_GlWidget != nullptr)
        {
            delete m_GlWidget;
        }

        m_GlWidget = glWidget;
        setCentralWidget(m_GlWidget);
        connect(&m_GlWidget->m_FPSTimer, &AvgTimer::avgTimeChanged, this,
                &OpenGLMainWindow::updateFrameRate);
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    QLabel*       m_LBLStatusFPS;
    OpenGLWidget* m_GlWidget;
};