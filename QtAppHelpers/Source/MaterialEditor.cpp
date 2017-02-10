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

#include <QtAppHelpers/MaterialEditor.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MaterialEditor class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MaterialEditor::MaterialEditor(const Material::MaterialData& material /*= Material::MT_Emerald*/, QWidget *parent /*= nullptr*/) : QDialog(parent)
{
    setModal(true);
    setupGUI();

    setMaterial(material);
    setFixedSize(250, 180);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MaterialEditor::~MaterialEditor()
{
    delete m_AmbientColorPicker;
    delete m_DiffuseColorPicker;
    delete m_SpecularColorPicker;
    delete m_txtShininess;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialEditor::setMaterial(const Material::MaterialData & material)
{
    m_CurrentMaterial = material;
    m_AmbientColorPicker->setColor(floatToQColor(material.ambient));
    m_DiffuseColorPicker->setColor(floatToQColor(material.diffuse));
    m_SpecularColorPicker->setColor(floatToQColor(material.specular));
    m_txtShininess->setText(QString::number(material.shininess));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const Material::MaterialData& MaterialEditor::getMaterial() const
{
    return m_CurrentMaterial;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialEditor::setupGUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    QGridLayout* materialLayout = new QGridLayout;
    QGroupBox* materialGrp = new QGroupBox;
    materialGrp->setLayout(materialLayout);
    mainLayout->addWidget(materialGrp);

    ////////////////////////////////////////////////////////////////////////////////
    materialLayout->addWidget(new QLabel("Ambient:"), 0, 0, 1, 1);
    m_AmbientColorPicker = new ColorPicker;
    materialLayout->addWidget(m_AmbientColorPicker, 0, 1, 1, 2);

    materialLayout->addWidget(new QLabel("Diffuse:"), 1, 0, 1, 1);
    m_DiffuseColorPicker = new ColorPicker;
    materialLayout->addWidget(m_DiffuseColorPicker, 1, 1, 1, 2);

    materialLayout->addWidget(new QLabel("Specular:"), 2, 0, 1, 1);
    m_SpecularColorPicker = new ColorPicker;
    materialLayout->addWidget(m_SpecularColorPicker, 2, 1, 1, 2);

    materialLayout->addWidget(new QLabel("Shininess:"), 3, 0, 1, 1);
    m_txtShininess = new QLineEdit("100");
    materialLayout->addWidget(m_txtShininess, 3, 1, 1, 2);


    ////////////////////////////////////////////////////////////////////////////////
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::accepted, this, [&]()
    {
        m_CurrentMaterial.ambient  = QColorToFloat(m_AmbientColorPicker->getColor());
        m_CurrentMaterial.diffuse  = QColorToFloat(m_DiffuseColorPicker->getColor());
        m_CurrentMaterial.specular = QColorToFloat(m_SpecularColorPicker->getColor());
        m_CurrentMaterial.shininess = m_txtShininess->text().toFloat();

        emit materialChanged(m_CurrentMaterial);
    });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// MaterialColorPicker class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MaterialColorPicker::MaterialColorPicker(QWidget *parent /*= nullptr*/) : QWidget(parent), m_MaterialEditor(new MaterialEditor)

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
MaterialColorPicker::~MaterialColorPicker()
{
    delete m_MaterialEditor;
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
void MaterialColorPicker::paintEvent(QPaintEvent * e)
{
    QPainter painter(this);
    painter.drawRoundedRect(1, 1, width() - 1, height() - 1, 1, 1);

    QWidget::paintEvent(e);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QSize MaterialColorPicker::sizeHint() const
{
    return QSize(50, 25);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialColorPicker::mousePressEvent(QMouseEvent *)
{
    m_MaterialEditor->show();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialColorPicker::enterEvent(QEvent *)
{
    QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialColorPicker::leaveEvent(QEvent *)
{
    QApplication::restoreOverrideCursor();
}
