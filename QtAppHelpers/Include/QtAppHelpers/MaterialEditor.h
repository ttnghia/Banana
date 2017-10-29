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

#include <QtAppHelpers/ColorPicker.h>
#include <OpenGLHelpers/Material.h>
#include <QtWidgets>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline QColor floatToQColor(const glm::vec4& color)
{
    return QColor(static_cast<int>(255 * color[0]), static_cast<int>(255 * color[1]), static_cast<int>(255 * color[2]));
}

inline glm::vec4 QColorToFloat(QColor color)
{
    return glm::vec4(static_cast<float>(color.red()) / 255.0, static_cast<float>(color.green()) / 255.0, static_cast<float>(color.blue()) / 255.0, 1.0);
}

class MaterialEditor : public QDialog
{
    Q_OBJECT

public:
    MaterialEditor(const Material::MaterialData& material = Material::MT_Emerald, QWidget* parent = nullptr);

    void                          setMaterial(const Material::MaterialData& material);
    const Material::MaterialData& getMaterial() const;

signals:
    void materialChanged(const Material::MaterialData material);

private:
    void setupGUI();

    ColorPicker*           m_AmbientColorPicker;
    ColorPicker*           m_DiffuseColorPicker;
    ColorPicker*           m_SpecularColorPicker;
    QLineEdit*             m_txtShininess;
    Material::MaterialData m_CurrentMaterial;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class MaterialColorPicker : public QWidget
{
    Q_OBJECT

public:
    MaterialColorPicker(QWidget* parent = nullptr);

    void  setMaterial(const Material::MaterialData& material);
    void  setWidgetColor(const Material::MaterialData& material);
    void  paintEvent(QPaintEvent* e);
    QSize sizeHint() const;

signals:
    void materialChanged(const Material::MaterialData& material);

protected:
    void mousePressEvent(QMouseEvent*);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    MaterialEditor* m_MaterialEditor;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana