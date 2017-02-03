//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 2/2/2017
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <QtAppHelpers/MaterialSelector.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MaterialSelector::MaterialSelector(const Material::MaterialData& material /*= Material::MT_Emerald*/,
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
    m_ColorWidget = new QWidget;
    m_ColorWidget->setAutoFillBackground(true);
    m_ColorWidget->setPalette(palette);

    m_Layout = new QGridLayout;
    m_Layout->addWidget(m_ComboBox, 0, 0, 1, comboBoxSpan);
    m_Layout->addWidget(m_ColorWidget, 0, comboBoxSpan, 1, 1);

    connect(m_ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setMaterial(int)));
    connect(m_ComboBox, SIGNAL(highlighted(int)), this, SLOT(setWidgetColor(int)));

    ////////////////////////////////////////////////////////////////////////////////
    m_Materials = Material::getBuildInMaterials();

    for(const Material::MaterialData& material : m_Materials)
    {
        m_ComboBox->addItem(QString::fromStdString(material.name));
    }
    m_ComboBox->addItem(QString("Custom Material"));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MaterialSelector::~MaterialSelector()
{
    delete m_ComboBox;
    delete m_GroupBox;
    delete m_Layout;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QComboBox* MaterialSelector::getComboBox()
{
    return m_ComboBox;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QLayout * MaterialSelector::getLayout()
{
    return m_Layout;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QGroupBox * MaterialSelector::getGroupBox(QString title)
{
    if(m_GroupBox == nullptr)
    {
        m_GroupBox = new QGroupBox(title);
        m_GroupBox->setLayout(m_Layout);
    }

    return m_GroupBox;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialSelector::setEnabled(bool enabled)
{
    m_ComboBox->setEnabled(enabled);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialSelector::setMaterial(int materialID)
{
    // the last material is custom material
    if(materialID == m_ComboBox->count() - 1)
        return;

    m_CurrentMaterial = m_Materials[materialID];
    setWidgetColor(m_CurrentMaterial);

    emit materialChanged(m_CurrentMaterial);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialSelector::setMaterial(const Material::MaterialData& material)
{
    size_t mIndex = m_Materials.size();
    for(size_t i = 0; i < m_Materials.size() - 1; ++i)
    {
        if(material.name == m_Materials[i].name)
        {
            mIndex = i;
            break;
        }
    }

    if(mIndex != m_Materials.size())
    {
        m_ComboBox->setCurrentIndex(mIndex);
        setMaterial(mIndex);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialSelector::setWidgetColor(int materialID)
{
    // the last material is custom material
    if(materialID == m_ComboBox->count() - 1)
        return;

    const Material::MaterialData& material = m_Materials[materialID];
    setWidgetColor(material);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialSelector::resetWidgetColor()
{
    setWidgetColor(m_CurrentMaterial);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MaterialSelector::setWidgetColor(const Material::MaterialData& material)
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, floatToQColor(material.diffuse));
    m_ColorWidget->setAutoFillBackground(true);
    m_ColorWidget->setPalette(palette);
}
