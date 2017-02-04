//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <OpenGLHelpers/Material.h>
#include <QtWidgets>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline QColor floatToQColor(const glm::vec4& color)
{
    return QColor(static_cast<int>(255 * color[0]), static_cast<int>(255 * color[1]),
                  static_cast<int>(255 * color[2]));
}

class MaterialSelector : public QWidget
{
    Q_OBJECT

public:
    MaterialSelector(const Material::MaterialData& material = Material::MT_Emerald,
                     int comboBoxSpan = 3, QWidget *parent = nullptr);
    ~MaterialSelector();

    QComboBox* getComboBox();
    QLayout* getLayout();
    QGroupBox* getGroupBox(QString title = QString(""));

    public slots:
    void setEnabled(bool enabled);
    void setMaterial(int materialID);
    void setMaterial(const Material::MaterialData& material);
    void setCustomMaterial(const Material::MaterialData& material);

signals:
    void materialChanged(const Material::MaterialData& material);

    private slots:
    void setWidgetColor(int materialID);

private:
    void setWidgetColor(const Material::MaterialData& material);

    QWidget*                            m_ColorWidget;
    QComboBox*                          m_ComboBox;
    QGroupBox*                          m_GroupBox;
    QGridLayout*                        m_Layout;
    Material::MaterialData              m_CurrentMaterial;
    Material::MaterialData              m_CustomMaterial;
    std::vector<Material::MaterialData> m_Materials;
};
