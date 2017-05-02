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

#include <QtAppHelpers/ClipPlaneEditor.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
ClipPlaneEditor::ClipPlaneEditor(QWidget* parent) : QWidget(parent), m_ClipPlane(1.0f, 0.0f, 0.0f, -0.5f)
{
    setupGUI();
    setWindowTitle("Clip Plane Editor");
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
void ClipPlaneEditor::setClipPlane(const glm::vec4& clipPlane)
{
    m_ClipPlane = clipPlane;
    for(int i = 0; i < 4; ++i)
    {
        m_lblPlanes[i]->setText(QString("%1").arg(m_ClipPlane[i]));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClipPlaneEditor::setupGUI()
{
    QLabel* lblIntro = new QLabel("Set clip plane Ax + By + Cz + D = 0");
    lblIntro->setAlignment(Qt::AlignCenter);
    QLabel* lblPlaneIntros[4];
    lblPlaneIntros[0] = new QLabel("A:");
    lblPlaneIntros[1] = new QLabel("B:");
    lblPlaneIntros[2] = new QLabel("C:");
    lblPlaneIntros[3] = new QLabel("D:");

    QHBoxLayout* planeLayouts[4];
    QVBoxLayout* sldLayout = new QVBoxLayout;

    for(int i = 0; i < 4; ++i)
    {
        m_lblPlanes[i] = new QLabel(QString("%1").arg(m_ClipPlane[i]));

        m_sldCoeffs[i] = new QSlider(Qt::Horizontal);
        m_sldCoeffs[i]->setMaximum(200);
        m_sldCoeffs[i]->setValue((m_ClipPlane[i] + 1.0) * 100.0);
        connect(m_sldCoeffs[i], &QSlider::valueChanged, this, [&, i](int value)
        {
            m_ClipPlane[i] = 2.0 * (value / 200.0) - 1.0;
            m_lblPlanes[i]->setText(QString("%1").arg(m_ClipPlane[i]));

            emit clipPlaneChanged(m_ClipPlane);
        });


        planeLayouts[i] = new QHBoxLayout;
        planeLayouts[i]->addWidget(lblPlaneIntros[i], 0);
        planeLayouts[i]->addWidget(m_sldCoeffs[i], 1);
        planeLayouts[i]->addWidget(m_lblPlanes[i], 0);
        sldLayout->addLayout(planeLayouts[i]);
    }


    QGroupBox* sldGroup = new QGroupBox;
    sldGroup->setLayout(sldLayout);

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
