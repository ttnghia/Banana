//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/22/2017
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

#include <QtWidgets>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class EnhancedSlider : public QWidget
{
    Q_OBJECT

public:
    EnhancedSlider(int sliderSpan = 5,
                   QWidget *parent= nullptr);
    ~EnhancedSlider();

    QSlider* getSlider();
    QSpinBox* getSpinBox();
    QLayout* getLayout();
    QGroupBox* getGroupBox(QString title = QString(""));

    void setTracking(bool tracking);
    void setRange(int minVal, int maxVal);

    public slots:
    void setEnabled(bool enabled);
    void prevItem();
    void nextItem();

private:
    QSlider*     m_Slider;
    QSpinBox*    m_SpinBox;
    QGroupBox*   m_GroupBox;
    QGridLayout* m_Layout;
};
