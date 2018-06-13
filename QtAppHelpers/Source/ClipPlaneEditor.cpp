//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <QtAppHelpers/ClipPlaneEditor.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
ClipPlaneEditor::ClipPlaneEditor(QWidget* parent) : QWidget(parent)
{
    setupGUI();
    setWindowTitle("Clip Plane Editor");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClipPlaneEditor::setClipPlane(const Vec4f& clipPlane)
{
    m_ClipPlane = clipPlane;
    for(int i = 0; i < 4; ++i) {
        m_txtCoeffs[i]->setText(QString("%1").arg(m_ClipPlane[i]));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ClipPlaneEditor::resetPlane()
{
    setClipPlane(Vec4f(1, 0, 0, 0));
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

    for(int i = 0; i < 4; ++i) {
        m_txtCoeffs[i] = new QLineEdit(QString("%1").arg(m_ClipPlane[i]));
        m_sldCoeffs[i] = new QSlider(Qt::Horizontal);
        m_sldCoeffs[i]->setMaximum(200);
        m_sldCoeffs[i]->setValue((m_ClipPlane[i] + 1.0f) * 100.0f);

        connect(m_txtCoeffs[i], &QLineEdit::textChanged, this, [&, i](const QString& value)
                {
                    float fval     = std::stof(value.toStdString());
                    m_ClipPlane[i] = fval;
                    m_sldCoeffs[i]->setValue(static_cast<int>((fval + 1.0f) * 100.0f));
                    emit clipPlaneChanged(m_ClipPlane);
                });

        connect(m_sldCoeffs[i], &QSlider::valueChanged, this, [&, i](int value)
                {
                    m_ClipPlane[i] = value / 100.0f - 1.0f;
                    m_txtCoeffs[i]->setText(QString("%1").arg(m_ClipPlane[i]));
                    emit clipPlaneChanged(m_ClipPlane);
                });

        planeLayouts[i] = new QHBoxLayout;
        planeLayouts[i]->addWidget(lblPlaneIntros[i], 1);
        planeLayouts[i]->addWidget(m_sldCoeffs[i], 10);
        planeLayouts[i]->addWidget(m_txtCoeffs[i], 1);
        sldLayout->addLayout(planeLayouts[i]);
    }

    QGroupBox* sldGroup = new QGroupBox;
    sldGroup->setLayout(sldLayout);

    QPushButton* btnReset = new QPushButton("Reset");
    QPushButton* btnClose = new QPushButton("Close");
    connect(btnReset, &QPushButton::clicked, this, [&]() { resetPlane(); });
    connect(btnClose, &QPushButton::clicked, this, [&]() { hide(); });

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(btnReset);
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana