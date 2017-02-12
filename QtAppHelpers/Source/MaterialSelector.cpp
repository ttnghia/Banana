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

#include <QtAppHelpers/MaterialSelector.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MaterialSelector::MaterialSelector(const Material::MaterialData& material /*= Material::MT_Emerald*/, bool defaultCustomMaterial /*= false*/,
                                   int comboBoxSpan /*= 3*/, QWidget *parent /*= nullptr*/)
    : QWidget(parent),
    m_CurrentMaterial(material),
    m_ComboBox(nullptr),
    m_GroupBox(nullptr),
    m_Layout(nullptr)
{
    m_ComboBox = new QComboBox;

    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, floatToQColor(m_CurrentMaterial.diffuse));
    m_MaterialColorPicker = new MaterialColorPicker;
    connect(m_MaterialColorPicker, &MaterialColorPicker::materialChanged, this, [&](const Material::MaterialData& material)
    {
        m_CustomMaterial = material;
        if(m_ComboBox->currentIndex() != m_ComboBox->count() - 1)
        {
            m_ComboBox->setCurrentIndex(m_ComboBox->count() - 1);
        }
        else
        {
            emit materialChanged(material);
        }
    });

    m_Layout = new QGridLayout;
    m_Layout->addWidget(m_ComboBox, 0, 0, 1, comboBoxSpan);
    m_Layout->addWidget(m_MaterialColorPicker, 0, comboBoxSpan, 1, 1);

    ////////////////////////////////////////////////////////////////////////////////
    m_Materials = Material::getBuildInMaterials();

    for(const Material::MaterialData& material : m_Materials)
    {
        m_ComboBox->addItem(QString::fromStdString(material.name));
        QColor color(floatToQColor(material.diffuse));
        m_ComboBox->setItemData(m_ComboBox->count() - 1, color, Qt::DecorationRole);
    }
    connect(m_ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setMaterial(int)));

    ////////////////////////////////////////////////////////////////////////////////
    m_CustomMaterial.ambient   = glm::vec4(0.2 * 0.2);
    m_CustomMaterial.diffuse   = glm::vec4(0.40, 0.65, 0.96, 1.00);
    m_CustomMaterial.specular  = glm::vec4(1);
    m_CustomMaterial.shininess = 200;

    m_ComboBox->addItem(QString("Custom Material"));
    QColor color(floatToQColor(m_CustomMaterial.diffuse));
    m_ComboBox->setItemData(m_ComboBox->count() - 1, color, Qt::DecorationRole);

    m_MaterialColorPicker->setMaterial(m_CustomMaterial);
    m_MaterialColorPicker->setWidgetColor(m_Materials[m_ComboBox->currentIndex()]);

    if(defaultCustomMaterial)
    {
        setMaterial(m_ComboBox->count() - 1);
        m_ComboBox->setCurrentIndex(m_ComboBox->count() - 1);
    }

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MaterialSelector::~MaterialSelector()
{
    delete m_ComboBox;
    delete m_GroupBox;
    delete m_Layout;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialSelector::setDefaultCustomMaterial(bool defaultCustomMaterial)
{
    m_ComboBox->setCurrentIndex(defaultCustomMaterial ? m_ComboBox->count() - 1 : 0);
    setMaterial(m_ComboBox->currentIndex());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QComboBox* MaterialSelector::getComboBox()
{
    return m_ComboBox;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QLayout * MaterialSelector::getLayout()
{
    return m_Layout;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QGroupBox * MaterialSelector::getGroupBox(QString title)
{
    if(m_GroupBox == nullptr)
    {
        m_GroupBox = new QGroupBox(title);
        m_GroupBox->setLayout(m_Layout);
    }

    return m_GroupBox;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int MaterialSelector::getNumMaterials()
{
    return m_ComboBox->count();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialSelector::setEnabled(bool enabled)
{
    m_ComboBox->setEnabled(enabled);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialSelector::setMaterial(int materialID)
{
    m_CurrentMaterial = (materialID == m_ComboBox->count() - 1) ? m_CustomMaterial : m_Materials[materialID];
    m_MaterialColorPicker->setWidgetColor(m_CurrentMaterial);

    emit materialChanged(m_CurrentMaterial);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialSelector::setMaterial(const Material::MaterialData& material)
{
    size_t mIndex = m_Materials.size();
    for(size_t i = 0; i < m_Materials.size(); ++i)
    {
        if(material.name == m_Materials[i].name)
        {
            mIndex = i;
            break;
        }
    }

    m_ComboBox->setCurrentIndex(mIndex);
    setMaterial(mIndex);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialSelector::setCustomMaterial(const Material::MaterialData & material)
{
    m_CurrentMaterial = material;
    if(m_ComboBox->currentIndex() == m_ComboBox->count() - 1)
    {
        m_MaterialColorPicker->setWidgetColor(m_CurrentMaterial);
    }
}
