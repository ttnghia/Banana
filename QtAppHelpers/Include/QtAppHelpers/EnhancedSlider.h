//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <QtWidgets>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class EnhancedSlider : public QWidget
{
    Q_OBJECT

public:
    EnhancedSlider(int sliderSpan = 5,
                   QWidget *parent= nullptr) :
        QWidget(parent),
        m_SpinBox(nullptr),
        m_Slider(nullptr),
        m_GroupBox(nullptr),
        m_Layout(nullptr)
    {
        m_Slider = new QSlider(Qt::Horizontal);
        m_Slider->setRange(0, 100);
        m_Slider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        m_SpinBox = new QSpinBox;
        m_SpinBox->setRange(0, 100);

        connect(m_Slider, &QSlider::valueChanged, m_SpinBox, &QSpinBox::setValue);
        connect(m_SpinBox, SIGNAL(valueChanged(int)), m_Slider, SLOT(setValue(int)));

        m_Layout = new QGridLayout;
        m_Layout->addWidget(m_Slider, 0, 0, 1, sliderSpan);
        m_Layout->addWidget(m_SpinBox, 0, sliderSpan, 1, 1);
    }

    ~EnhancedSlider()
    {
        delete m_Slider;
        delete m_SpinBox;
        delete m_GroupBox;
        delete m_Layout;
    }

    QSlider* getSlider()
    {
        return m_Slider;
    }

    QSpinBox* getSpinBox()
    {
        return m_SpinBox;
    }

    QLayout* getLayout()
    {
        return m_Layout;
    }

    QGroupBox* getGroupBox(QString title = QString(""))
    {
        if(m_GroupBox == nullptr)
        {
            m_GroupBox = new QGroupBox(title);
            m_GroupBox->setLayout(m_Layout);
        }

        return m_GroupBox;
    }

    void setTracking(bool tracking)
    {
        m_Slider->setTracking(tracking);
    }

    void setRange(int minVal, int maxVal)
    {
        m_Slider->setRange(minVal, maxVal);
        m_SpinBox->setRange(minVal, maxVal);
    }

    void setEnabled(bool enabled)
    {
        m_Slider->setEnabled(enabled);
        m_SpinBox->setEnabled(enabled);
    }

    public slots:
    void prevItem()
    {
        int currentVal = m_Slider->value();

        if(currentVal > m_Slider->minimum())
        {
            m_Slider->setValue(currentVal - 1);
        }
    }

    void nextItem()
    {
        int currentVal = m_Slider->value();

        if(currentVal < m_Slider->maximum())
        {
            m_Slider->setValue(currentVal + 1);
        }
    }

private:
    QSlider*     m_Slider;
    QSpinBox*    m_SpinBox;
    QGroupBox*   m_GroupBox;
    QGridLayout* m_Layout;
};
