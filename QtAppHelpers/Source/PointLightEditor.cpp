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

#include <QtAppHelpers/PointLightEditor.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
PointLightEditor::PointLightEditor(PointLights* lights /*= nullptr*/, QWidget *parent /*= nullptr*/)
    : QWidget(parent), m_Lights(lights)
{
    QVBoxLayout* mainLayout = new QVBoxLayout;
    QGridLayout* lightLayouts[MAX_POINT_LIGHT];


    for(int i = 0; i < MAX_POINT_LIGHT; ++i)
    {
        lightLayouts[i] = new QGridLayout;
        m_CheckBoxes[i] = new QCheckBox(QString("Enable Light %1").arg(i));
        m_CheckBoxes[i]->setEnabled((i == 1));
        m_CheckBoxes[i]->setChecked((i == 0));

        lightLayouts[i]->addWidget(m_CheckBoxes[i], 0, 0, 1, 6);
        lightLayouts[i]->addWidget(new QLabel("Ambient: "), 1, 0, 1, 2);
        lightLayouts[i]->addWidget(new QLabel("Diffuse: "), 2, 0, 1, 2);
        lightLayouts[i]->addWidget(new QLabel("Specular: "), 3, 0, 1, 2);
        lightLayouts[i]->addWidget(new QLabel("Position: "), 4, 0, 1, 2);

        for(int j = 0; j < 3; ++j)
        {
            m_LightAmbients[i][j]  = new QLineEdit;
            m_LightDiffuses[i][j]  = new QLineEdit;
            m_LightSpeculars[i][j] = new QLineEdit;
            m_LightPositions[i][j] = new QLineEdit;

            m_LightAmbients[i][j]->setText("0.2");
            m_LightDiffuses[i][j]->setText("1.0");
            m_LightSpeculars[i][j]->setText("1.0");
            m_LightPositions[i][j]->setText(j == 1 ? "100.0" : "0");

            m_LightAmbients[i][j]->setEnabled((i == 0));
            m_LightDiffuses[i][j]->setEnabled((i == 0));
            m_LightSpeculars[i][j]->setEnabled((i == 0));
            m_LightPositions[i][j]->setEnabled((i == 0));

            lightLayouts[i]->addWidget(m_LightAmbients[i][j], 1, j + 2, 1, 1);
            lightLayouts[i]->addWidget(m_LightDiffuses[i][j], 2, j + 2, 1, 1);
            lightLayouts[i]->addWidget(m_LightSpeculars[i][j], 3, j + 2, 1, 1);
            lightLayouts[i]->addWidget(m_LightPositions[i][j], 4, j + 2, 1, 1);

            m_ColorSelectors[i][j] = new ColorPicker;
            m_ColorSelectors[i][j]->setColor(j == 0 ?
                                             floatToQColor(0.2, 0.2, 0.2) : floatToQColor(1.0, 1.0, 1.0));
            m_ColorSelectors[i][j]->setFixedWidth(50);
            m_ColorSelectors[i][j]->setEnabled((i == 0));
            lightLayouts[i]->addWidget(m_ColorSelectors[i][j], j + 1, 5, 1, 1);

        }


        QGroupBox* grLight = new QGroupBox(QString("Light %1").arg(i), this);
        grLight->setLayout(lightLayouts[i]);
        mainLayout->addWidget(grLight);
    }

    mainLayout->addStretch();

    QPushButton* btnApply = new QPushButton("Apply Lights");
    connect(btnApply, &QPushButton::clicked, this, &PointLightEditor::applyLights);
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(btnApply, 2);
    btnLayout->addStretch(1);

    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);
    connectComponents();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLightEditor::setLights(PointLights * lights)
{
    m_Lights = lights;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLightEditor::connectComponents()
{
    for(int i = 0; i < MAX_POINT_LIGHT; ++i)
    {
        connect(m_CheckBoxes[i], &QCheckBox::toggled, this,
                [&, i](bool checked)
        {
            for(int j = 0; j < 3; ++j)
            {
                m_LightAmbients[i][j]->setEnabled(checked);
                m_LightDiffuses[i][j]->setEnabled(checked);
                m_LightSpeculars[i][j]->setEnabled(checked);
                m_LightPositions[i][j]->setEnabled(checked);

                m_ColorSelectors[i][j]->setEnabled(checked);
            }
        });

        ////////////////////////////////////////////////////////////////////////////////
        connect(m_ColorSelectors[i][0], &ColorPicker::colorChanged, this,
                [&, i](float r, float g, float b)
        {
            m_LightAmbients[i][0]->setText(QString("%1").arg(r, 8, 'g', 6));
            m_LightAmbients[i][1]->setText(QString("%1").arg(g, 8, 'g', 6));
            m_LightAmbients[i][2]->setText(QString("%1").arg(b, 8, 'g', 6));
        });

        for(int j = 0; j < 3; ++j)
        {
            connect(m_LightAmbients[i][j], &QLineEdit::textChanged, this,
                    [&, i]()
            {
                m_ColorSelectors[i][0]->setColor(floatToQColor(m_LightAmbients[i][0]->text().toFloat(),
                                                 m_LightAmbients[i][1]->text().toFloat(),
                                                 m_LightAmbients[i][2]->text().toFloat()));
            });
        }


        ////////////////////////////////////////////////////////////////////////////////
        connect(m_ColorSelectors[i][1], &ColorPicker::colorChanged, this,
                [&, i](float r, float g, float b)
        {
            m_LightDiffuses[i][0]->setText(QString("%1").arg(r, 8, 'g', 6));
            m_LightDiffuses[i][1]->setText(QString("%1").arg(g, 8, 'g', 6));
            m_LightDiffuses[i][2]->setText(QString("%1").arg(b, 8, 'g', 6));
        });

        for(int j = 0; j < 3; ++j)
        {
            connect(m_LightDiffuses[i][j], &QLineEdit::textChanged, this,
                    [&, i]()
            {
                m_ColorSelectors[i][1]->setColor(floatToQColor(m_LightDiffuses[i][0]->text().toFloat(),
                                                 m_LightDiffuses[i][1]->text().toFloat(),
                                                 m_LightDiffuses[i][2]->text().toFloat()));
            });
        }

        ////////////////////////////////////////////////////////////////////////////////
        connect(m_ColorSelectors[i][2], &ColorPicker::colorChanged, this,
                [&, i](float r, float g, float b)
        {
            m_LightSpeculars[i][0]->setText(QString("%1").arg(r, 8, 'g', 6));
            m_LightSpeculars[i][1]->setText(QString("%1").arg(g, 8, 'g', 6));
            m_LightSpeculars[i][2]->setText(QString("%1").arg(b, 8, 'g', 6));
        });

        for(int j = 0; j < 3; ++j)
        {
            connect(m_LightSpeculars[i][j], &QLineEdit::textChanged, this,
                    [&, i]()
            {
                m_ColorSelectors[i][2]->setColor(floatToQColor(m_LightSpeculars[i][0]->text().toFloat(),
                                                 m_LightSpeculars[i][1]->text().toFloat(),
                                                 m_LightSpeculars[i][2]->text().toFloat()));
            });
        }

        ////////////////////////////////////////////////////////////////////////////////
        // only allow light i+1, i+2, .... to be enabled when light i was enabled
        if(i < MAX_POINT_LIGHT - 1)
        {
            connect(m_CheckBoxes[i], &QCheckBox::toggled, this,
                    [&, i](bool checked)
            {
                m_CheckBoxes[i + 1]->setEnabled(checked);
                if(!checked)
                {
                    m_CheckBoxes[i + 1]->setChecked(false);
                }
            });
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLightEditor::applyLights()
{
    assert(m_Lights != nullptr);

    ////////////////////////////////////////////////////////////////////////////////
    // update the number of active lights
    int numPointLights = 0;
    for(int i = 0; i < MAX_POINT_LIGHT; ++i)
    {
        if(!m_CheckBoxes[i]->isChecked())
        {
            numPointLights = i;
            break;
        }
    }

    if(m_Lights->getNumLights() != numPointLights)
    {
        m_Lights->setNumLights(numPointLights);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // update light data
    for(int i = 0; i < numPointLights; ++i)
    {
        PointLights::PointLightData light;

        light.setAmbient(glm::vec4(m_LightAmbients[i][0]->text().toFloat(),
                         m_LightAmbients[i][1]->text().toFloat(),
                         m_LightAmbients[i][2]->text().toFloat(),
                         1.0));
        light.setDiffuse(glm::vec4(m_LightDiffuses[i][0]->text().toFloat(),
                         m_LightDiffuses[i][1]->text().toFloat(),
                         m_LightDiffuses[i][2]->text().toFloat(),
                         1.0));
        light.setSpecular(glm::vec4(m_LightSpeculars[i][0]->text().toFloat(),
                          m_LightSpeculars[i][1]->text().toFloat(),
                          m_LightSpeculars[i][2]->text().toFloat(),
                          1.0));

        light.setPosition(glm::vec4(m_LightPositions[i][0]->text().toFloat(),
                          m_LightPositions[i][1]->text().toFloat(),
                          m_LightPositions[i][2]->text().toFloat(),
                          1.0));

        m_Lights->setLight(light, i);
    }

    ////////////////////////////////////////////////////////////////////////////////
    emit lightsChanged(m_Lights);
}