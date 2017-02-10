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

#include <QMouseEvent>

#include "MainWindow.h"

#include <QtAppHelpers/MaterialSelector.h>
#include <QtAppHelpers/MaterialEditor.h>
#include <QtAppHelpers/ColorPicker.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MainWindow::MainWindow(QWidget* parent) : OpenGLMainWindow(parent)
{
    //    instantiateOpenGLWidget();

    setWindowTitle("Test QtWidget");
    setFocusPolicy(Qt::StrongFocus);

    setupGUI();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::instantiateOpenGLWidget()
{
    //OpenGLWidgetTestRender* glWidget = new OpenGLWidgetTestRender(this);
    //setupOpenglWidget(glWidget);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::keyPressEvent(QKeyEvent* event)
{
    switch(event->key())
    {
        default:
            OpenGLMainWindow::keyPressEvent(event);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::setupGUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout;

    MaterialSelector* matSelector = new MaterialSelector;
    mainLayout->addWidget(matSelector->getGroupBox("Material"));

    ColorPicker* colorSelector = new ColorPicker;
    QHBoxLayout* colorSLTLayout = new QHBoxLayout;
    colorSLTLayout->addStretch();
    colorSLTLayout->addWidget(colorSelector);
    mainLayout->addLayout(colorSLTLayout);

    m_MatEditor  = new MaterialEditor;
    QPushButton* btnShowMatEditor =  new QPushButton("Material");
    QHBoxLayout* matEditorLayout = new QHBoxLayout;
    matEditorLayout->addStretch();
    matEditorLayout->addWidget(btnShowMatEditor);
    connect(btnShowMatEditor, &QPushButton::clicked, [&]()
    {
        m_MatEditor->show();
    });
    mainLayout->addLayout(matEditorLayout);


    MaterialColorPicker* matPicker = new MaterialColorPicker;
    QHBoxLayout* matPickerLayout = new QHBoxLayout;
    matPickerLayout->addStretch();
    matPickerLayout->addWidget(matPicker);
    mainLayout->addLayout(matPickerLayout);


    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grp = new QGroupBox;
    grp->setLayout(mainLayout);
    setCentralWidget(grp);
}
