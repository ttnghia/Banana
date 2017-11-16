//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
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

#include <QtAppHelpers/MaterialEditor.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MaterialEditor class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MaterialEditor::MaterialEditor(const Material::MaterialData& material /*= Material::MT_Emerald*/, QWidget* parent /*= nullptr*/) : QDialog(parent)
{
    setModal(true);
    setupGUI();

    setMaterial(material);
    setFixedSize(250, 180);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialEditor::setMaterial(const Material::MaterialData& material)
{
    m_CurrentMaterial = material;
    m_AmbientColorPicker->setColor(floatToQColor(material.ambient));
    m_DiffuseColorPicker->setColor(floatToQColor(material.diffuse));
    m_SpecularColorPicker->setColor(floatToQColor(material.specular));
    m_txtShininess->setText(QString::number(material.shininess));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialEditor::setupGUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    QGridLayout* materialLayout = new QGridLayout;
    QGroupBox*   materialGrp    = new QGroupBox;
    materialGrp->setLayout(materialLayout);
    mainLayout->addWidget(materialGrp);

    ////////////////////////////////////////////////////////////////////////////////
    materialLayout->addWidget(new QLabel("Ambient:"),   0, 0, 1, 1);
    m_AmbientColorPicker = new ColorPicker;
    materialLayout->addWidget(m_AmbientColorPicker,     0, 1, 1, 2);

    materialLayout->addWidget(new QLabel("Diffuse:"),   1, 0, 1, 1);
    m_DiffuseColorPicker = new ColorPicker;
    materialLayout->addWidget(m_DiffuseColorPicker,     1, 1, 1, 2);

    materialLayout->addWidget(new QLabel("Specular:"),  2, 0, 1, 1);
    m_SpecularColorPicker = new ColorPicker;
    materialLayout->addWidget(m_SpecularColorPicker,    2, 1, 1, 2);

    materialLayout->addWidget(new QLabel("Shininess:"), 3, 0, 1, 1);
    m_txtShininess = new QLineEdit("100");
    materialLayout->addWidget(m_txtShininess,           3, 1, 1, 2);


    ////////////////////////////////////////////////////////////////////////////////
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::accepted, this, [&]()
            {
                m_CurrentMaterial.ambient   = Vec4f(QColorToFloat(m_AmbientColorPicker->getQColor()), 1);
                m_CurrentMaterial.diffuse   = Vec4f(QColorToFloat(m_DiffuseColorPicker->getQColor()), 1);
                m_CurrentMaterial.specular  = Vec4f(QColorToFloat(m_SpecularColorPicker->getQColor()), 1);
                m_CurrentMaterial.shininess = m_txtShininess->text().toFloat();

                emit materialChanged(m_CurrentMaterial);
            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MaterialColorPicker class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MaterialColorPicker::MaterialColorPicker(QWidget* parent /*= nullptr*/) : QWidget(parent), m_MaterialEditor(new MaterialEditor)

{
    setMouseTracking(true);
    setAutoFillBackground(true);
    setMaterial(m_MaterialEditor->getMaterial());
    connect(m_MaterialEditor, &MaterialEditor::materialChanged, this, [&](const Material::MaterialData& material)
            {
                setMaterial(material);
                emit materialChanged(material);
            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialColorPicker::setMaterial(const Material::MaterialData& material)
{
    m_MaterialEditor->setMaterial(material);
    setWidgetColor(material);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialColorPicker::setWidgetColor(const Material::MaterialData& material)
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, floatToQColor(material.diffuse));
    setPalette(palette);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialColorPicker::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.drawRoundedRect(1, 1, width() - 1, height() - 1, 1, 1);

    QWidget::paintEvent(e);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana