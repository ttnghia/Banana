//------------------------------------------------------------------------------------------
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 9/28/2016
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

#include <QMouseEvent>
#include <qmath.h>
#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
    setupGUI();
    connect(&renderer->FPSTimer, &AvgTimer::avgTimeChanged, this,
            &MainWindow::setFPS);
}

//------------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    renderer->dump();
    delete renderer;
}


//------------------------------------------------------------------------------------------
void MainWindow::setupGUI()
{
    renderer = new FluidRenderer(this);
    setCentralWidget(renderer);

    lblStatusMethod = new QLabel(this);
    lblStatusMethod->setMargin(5);
    statusBar()->addPermanentWidget(lblStatusMethod);
    statusBar()->setMinimumHeight(30);
    statusBar()->setSizeGripEnabled(false);
    setMethodStatus();

    lblStatusFPS = new QLabel(this);
    lblStatusFPS->setMargin(5);
    statusBar()->addPermanentWidget(lblStatusFPS);
    statusBar()->setMinimumHeight(30);
    statusBar()->setSizeGripEnabled(false);
}

//------------------------------------------------------------------------------------------
void MainWindow::setMethodStatus()
{
    if(renderer->getMethod() == 0)
    {
        lblStatusMethod->setText("Old Method ");
    }
    else
    {
        lblStatusMethod->setText("New Method");
    }

}

//------------------------------------------------------------------------------------------
    void MainWindow::keyPressEvent(QKeyEvent * e)
    {
        switch(e->key())
        {
        case Qt::Key_Escape:
            exit(EXIT_SUCCESS);

        case Qt::Key_Space:
            renderer->setMethod(1 - renderer->getMethod());
            setMethodStatus();
            break;

        default:
            renderer->keyPressEvent(e);
        }
    }

//------------------------------------------------------------------------------------------
    void MainWindow::keyReleaseEvent(QKeyEvent * _event)
    {
        renderer->keyReleaseEvent(_event);
    }

//------------------------------------------------------------------------------------------
    void MainWindow::setFPS(double avgTime)
    {
        lblStatusFPS->setText(QString("Frame time: %1ms (~ %2 FPS)")
                              .arg(avgTime).arg(1000.0 / avgTime));
    }
