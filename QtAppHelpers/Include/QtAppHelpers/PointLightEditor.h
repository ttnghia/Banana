//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <OpenGLHelpers/Light.h>
#include <QtAppHelpers/ColorSelector.h>

#include <QtWidgets>
#include <vector>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define MAX_POINT_LIGHT 4

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PointLightEditor : public QWidget
{
    Q_OBJECT

public:
    PointLightEditor(QWidget *parent = nullptr);
    ~PointLightEditor();

    void setLight(int lightID, const PointLight::PointLightData& light);
    void addLight(const PointLight::PointLightData& light);

signals:
    void lightsChanged(std::vector<PointLight::PointLightData> lights);

private:
    void connectComponents();
    void applyLights();


    QCheckBox*     m_CheckBoxes[MAX_POINT_LIGHT];
    QLineEdit*     m_LightAmbients[MAX_POINT_LIGHT][3];
    QLineEdit*     m_LightDiffuses[MAX_POINT_LIGHT][3];
    QLineEdit*     m_LightSpeculars[MAX_POINT_LIGHT][3];
    QLineEdit*     m_LightPositions[MAX_POINT_LIGHT][3];
    ColorSelector* m_ColorSelectors[MAX_POINT_LIGHT][3];

    std::vector<PointLight::PointLightData> m_Lights;
};
