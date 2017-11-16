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

#pragma once

#include <OpenGLHelpers/Material.h>
#include <QtAppHelpers/MaterialEditor.h>
#include <QtWidgets>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MaterialSelector : public QWidget
{
    Q_OBJECT

public:
    MaterialSelector(const Material::MaterialData& material = Material::MT_Emerald, bool defaultCustomMaterial = false,
                     int comboBoxSpan = 4, QWidget* parent = nullptr);

    void       setDefaultCustomMaterial(bool defaultCustomMaterial);
    QComboBox* getComboBox() const;
    QLayout*   getLayout() const;
    QGroupBox* getGroupBox(QString title = QString(""));
    int        getNumMaterials() const;

public slots:
    void setEnabled(bool enabled);
    void setMaterial(int materialID);
    void setMaterial(const Material::MaterialData& material);
    void setCustomMaterial(const Material::MaterialData& material);
    void setDebug(bool bDebug);

signals:
    void materialChanged(const Material::MaterialData& material);

private:
    QColor floatToQColor(float r, float g, float b) { return QColor(static_cast<int>(255 * r), static_cast<int>(255 * g), static_cast<int>(255 * b)); }
    QColor floatToQColor(const Vec3f& color) { return floatToQColor(color.x, color.y, color.z); }
    Vec3f  QColorToFloat(const QColor& color) { return Vec3f(color.redF(), color.greenF(), color.blueF()); }

    bool                                m_bDebug;
    QComboBox*                          m_ComboBox;
    QGroupBox*                          m_GroupBox;
    QGridLayout*                        m_Layout;
    MaterialColorPicker*                m_MaterialColorPicker;
    Material::MaterialData              m_CurrentMaterial;
    Material::MaterialData              m_CustomMaterial;
    std::vector<Material::MaterialData> m_Materials;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana