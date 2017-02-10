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

#pragma once

#include <OpenGLHelpers/Material.h>
#include <QtAppHelpers/MaterialEditor.h>
#include <QtWidgets>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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

private:
    QComboBox*                          m_ComboBox;
    QGroupBox*                          m_GroupBox;
    QGridLayout*                        m_Layout;
    MaterialColorPicker*                m_MaterialColorPicker;
    Material::MaterialData              m_CurrentMaterial;
    Material::MaterialData              m_CustomMaterial;
    std::vector<Material::MaterialData> m_Materials;
};
