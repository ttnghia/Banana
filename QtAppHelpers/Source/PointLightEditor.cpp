//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 2/3/2017
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <QtAppHelpers/PointLightEditor.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
PointLightEditor::PointLightEditor(QWidget *parent)
    : QWidget(parent)
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

            m_ColorSelectors[i][j] = new ColorSelector;
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
PointLightEditor::~PointLightEditor()
{
    m_Lights.clear();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
        connect(m_ColorSelectors[i][0], &ColorSelector::colorChanged, this,
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
                m_ColorSelectors[i][0]->setColor(floatToQColor(
                    m_LightAmbients[i][0]->text().toFloat(),
                    m_LightAmbients[i][1]->text().toFloat(),
                    m_LightAmbients[i][2]->text().toFloat()));
            });
        }


        ////////////////////////////////////////////////////////////////////////////////
        connect(m_ColorSelectors[i][1], &ColorSelector::colorChanged, this,
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
                m_ColorSelectors[i][1]->setColor(floatToQColor(
                    m_LightDiffuses[i][0]->text().toFloat(),
                    m_LightDiffuses[i][1]->text().toFloat(),
                    m_LightDiffuses[i][2]->text().toFloat()));
            });
        }

        ////////////////////////////////////////////////////////////////////////////////
        connect(m_ColorSelectors[i][2], &ColorSelector::colorChanged, this,
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
                m_ColorSelectors[i][2]->setColor(floatToQColor(
                    m_LightSpeculars[i][0]->text().toFloat(),
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

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLightEditor::setLight(int lightID, const PointLight::PointLightData & light)
{
    assert(lightID < MAX_POINT_LIGHT);

    if(m_CheckBoxes[lightID]->isChecked())
    {
        for(int j = 0; j < 3; ++j)
        {
            m_LightAmbients[lightID][j]->setText(QString("%1").arg(light.ambient[j], 8, 'g', 6));
            m_LightDiffuses[lightID][j]->setText(QString("%1").arg(light.diffuse[j], 8, 'g', 6));
            m_LightSpeculars[lightID][j]->setText(QString("%1").arg(light.specular[j], 8, 'g', 6));
            m_LightPositions[lightID][j]->setText(QString("%1").arg(light.position[j], 8, 'f', 2));
        }
    }
    else
    {
        addLight(light);
    }
}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLightEditor::addLight(const PointLight::PointLightData & light)
{
    int lightID = 0;
    for(; lightID < MAX_POINT_LIGHT; ++lightID)
    {
        if(!m_CheckBoxes[lightID]->isChecked())
            break;
    }

    if(lightID < MAX_POINT_LIGHT)
    {
        m_CheckBoxes[lightID]->setChecked(true);
        setLight(lightID, light);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLightEditor::applyLights()
{
    m_Lights.resize(0);

    for(int i = 0; i < MAX_POINT_LIGHT; ++i)
    {
        if(!m_CheckBoxes[i]->isChecked())
        {
            break;
        }

        PointLight::PointLightData light;

        light.ambient = glm::vec4(m_LightAmbients[i][0]->text().toFloat(),
                                  m_LightAmbients[i][1]->text().toFloat(),
                                  m_LightAmbients[i][2]->text().toFloat(),
                                  1.0);
        light.diffuse = glm::vec4(m_LightDiffuses[i][0]->text().toFloat(),
                                  m_LightDiffuses[i][1]->text().toFloat(),
                                  m_LightDiffuses[i][2]->text().toFloat(),
                                  1.0);
        light.specular = glm::vec4(m_LightSpeculars[i][0]->text().toFloat(),
                                   m_LightSpeculars[i][1]->text().toFloat(),
                                   m_LightSpeculars[i][2]->text().toFloat(),
                                   1.0);

        light.position = glm::vec4(m_LightPositions[i][0]->text().toFloat(),
                                   m_LightPositions[i][1]->text().toFloat(),
                                   m_LightPositions[i][2]->text().toFloat(),
                                   1.0);

        m_Lights.push_back(light);
    }

    assert(m_Lights.size() > 0);

    ////////////////////////////////////////////////////////////////////////////////
    emit lightsChanged(m_Lights);
}