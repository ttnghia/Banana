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

#include "ClipPlaneEditor.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
ClipPlaneEditor::ClipPlaneEditor(QWidget *parent) : QWidget(parent), m_CoeffA(1.0), m_CoeffB(0.0), m_CoeffC(0.0), m_CoeffD(-0.5)
{
    setupGUI();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
ClipPlaneEditor::~ClipPlaneEditor()
{
    delete m_sldCoeffA;
    delete m_sldCoeffB;
    delete m_sldCoeffC;
    delete m_sldCoeffD;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QSize ClipPlaneEditor::sizeHint() const
{
    return QSize(600, 250);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClipPlaneEditor::keyPressEvent(QKeyEvent* e)
{
    switch(e->key())
    {
        case Qt::Key_Escape:
        case Qt::Key_Enter:
            hide();
            break;

        default:
            QWidget::keyPressEvent(e);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClipPlaneEditor::setClipPlane()
{
    emit clipPlaneChanged(QVector4D(m_CoeffA, m_CoeffB, m_CoeffC, m_CoeffD));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClipPlaneEditor::setupGUI()
{
    QLabel* lblIntro = new QLabel("Set clip plane Ax + By + Cz + D = 0");
    lblIntro->setAlignment(Qt::AlignCenter);
    QLabel* lblPlaneXIntro = new QLabel("A:");
    QLabel* lblPlaneYIntro = new QLabel("B:");
    QLabel* lblPlaneZIntro = new QLabel("C:");
    QLabel* lblPlaneDIntro = new QLabel("D:");

    QLabel* lblPlaneX = new QLabel("-1");
    QLabel* lblPlaneY = new QLabel("-1");
    QLabel* lblPlaneZ = new QLabel("-1");
    QLabel* lblPlaneD = new QLabel("-1");

    m_sldCoeffA = new QSlider(Qt::Horizontal);
    m_sldCoeffA->setMaximum(200);
    connect(m_sldCoeffA, &QSlider::valueChanged, this, [&](int value)
    {
        m_CoeffA = 2.0 * (value / 200.0) - 1.0;
        lblPlaneX->setText(QString("%1").arg(m_CoeffA));

        setClipPlane();
    });

    m_sldCoeffB = new QSlider(Qt::Horizontal);
    m_sldCoeffB->setMaximum(200);
    connect(m_sldCoeffB, &QSlider::valueChanged, [=](const int value)
    {
        m_CoeffB = 2.0 * (value / 200.0) - 1.0;
        lblPlaneY->setText(QString("%1").arg(m_CoeffB));

        setClipPlane();
    });

    m_sldCoeffC = new QSlider(Qt::Horizontal);
    m_sldCoeffC->setMaximum(200);
    connect(m_sldCoeffC, &QSlider::valueChanged, [=](const int value)
    {
        m_CoeffC = 2.0 * (value / 200.0) - 1.0;
        lblPlaneZ->setText(QString("%1").arg(m_CoeffC));

        setClipPlane();
    });

    m_sldCoeffD = new QSlider(Qt::Horizontal);
    m_sldCoeffD->setMaximum(200);
    connect(m_sldCoeffD, &QSlider::valueChanged, [=](const int value)
    {
        m_CoeffD = 2.0 * (value / 200.0) - 1.0;
        lblPlaneD->setText(QString("%1").arg(m_CoeffD));

        setClipPlane();
    });

    QHBoxLayout* planeXLayout = new QHBoxLayout;
    planeXLayout->addWidget(lblPlaneXIntro, 0);
    planeXLayout->addWidget(m_sldCoeffA, 1);
    planeXLayout->addWidget(lblPlaneX, 0);

    QHBoxLayout* planeYLayout = new QHBoxLayout;
    planeYLayout->addWidget(lblPlaneYIntro, 0);
    planeYLayout->addWidget(m_sldCoeffB, 1);
    planeYLayout->addWidget(lblPlaneY, 0);

    QHBoxLayout* planeZLayout = new QHBoxLayout;
    planeZLayout->addWidget(lblPlaneZIntro, 0);
    planeZLayout->addWidget(m_sldCoeffC, 1);
    planeZLayout->addWidget(lblPlaneZ, 0);

    QHBoxLayout* planeDLayout = new QHBoxLayout;
    planeDLayout->addWidget(lblPlaneDIntro, 0);
    planeDLayout->addWidget(m_sldCoeffD, 1);
    planeDLayout->addWidget(lblPlaneD, 0);



    QVBoxLayout* sldLayout = new QVBoxLayout;
    sldLayout->addLayout(planeXLayout);
    sldLayout->addLayout(planeYLayout);
    sldLayout->addLayout(planeZLayout);
    sldLayout->addLayout(planeDLayout);

    QGroupBox* sldGroup = new QGroupBox;
    sldGroup->setLayout(sldLayout);

    m_sldCoeffA->setValue((m_CoeffA + 1.0) * 100.0);
    m_sldCoeffB->setValue((m_CoeffB + 1.0) * 100.0);
    m_sldCoeffC->setValue((m_CoeffC + 1.0) * 100.0);
    m_sldCoeffD->setValue((-0.5 + 1.0) * 100.0);

    QPushButton* btnClose = new QPushButton("Close");
    connect(btnClose, &QPushButton::clicked, this, [&]()
    {
        hide();
    });

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addStretch(1);
    btnLayout->addWidget(btnClose);

    QGroupBox* btnGroup = new QGroupBox;
    btnGroup->setLayout(btnLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(lblIntro);
    mainLayout->addWidget(sldGroup);
    mainLayout->addWidget(btnGroup);


    setLayout(mainLayout);
}
