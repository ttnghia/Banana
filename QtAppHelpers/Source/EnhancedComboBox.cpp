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

#include <QtAppHelpers/EnhancedComboBox.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
EnhancedComboBox::EnhancedComboBox(int comboBoxSpan /*= 3*/,
                                   QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    m_ComboBox(nullptr),
    m_GroupBox(nullptr),
    m_Layout(nullptr),
    m_btnPrev(nullptr),
    m_btnNext(nullptr),
    m_enabledCycling(true)
{
    m_ComboBox = new QComboBox;

    m_btnPrev = new QToolButton;
    m_btnPrev->setArrowType(Qt::LeftArrow);
    m_btnNext = new QToolButton;
    m_btnNext->setArrowType(Qt::RightArrow);

    connect(m_btnPrev, SIGNAL(clicked()), this, SLOT(prevItem()));
    connect(m_btnNext, SIGNAL(clicked()), this, SLOT(nextItem()));

    m_Layout = new QGridLayout;
    m_Layout->addWidget(m_ComboBox, 0, 0, 1, comboBoxSpan);
    m_Layout->addWidget(m_btnPrev, 0, comboBoxSpan, 1, 1);
    m_Layout->addWidget(m_btnNext, 0, comboBoxSpan + 1, 1, 1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
EnhancedComboBox::~EnhancedComboBox()
{
    delete m_ComboBox;
    delete m_GroupBox;
    delete m_Layout;
    delete m_btnPrev;
    delete m_btnNext;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QComboBox* EnhancedComboBox::getComboBox()
{
    return m_ComboBox;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void EnhancedComboBox::enableCycling()
{
    m_enabledCycling = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void EnhancedComboBox::disableCycling()
{
    m_enabledCycling = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QLayout* EnhancedComboBox::getLayout()
{
    return m_Layout;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QLayout* EnhancedComboBox::getLayoutWithLabel(QString label, int comboStretch /*= 5*/)
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(new QLabel(label), 1);
    layout->addLayout(m_Layout, comboStretch);

    return layout;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QGroupBox* EnhancedComboBox::getGroupBox(QString title)
{
    if(m_GroupBox == nullptr)
    {
        m_GroupBox = new QGroupBox(title);
        m_GroupBox->setLayout(m_Layout);
    }

    return m_GroupBox;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void EnhancedComboBox::setCurrentIndex(int index)
{
    m_ComboBox->setCurrentIndex(index);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int EnhancedComboBox::getCurrentIndex() const
{
    return m_ComboBox->currentIndex();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void EnhancedComboBox::setEnabled(bool enabled)
{
    m_ComboBox->setEnabled(enabled);
    m_btnPrev->setEnabled(enabled);
    m_btnNext->setEnabled(enabled);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void EnhancedComboBox::prevItem()
{
    int currentIndex = m_ComboBox->currentIndex();

    if(currentIndex > 0)
    {
        m_ComboBox->setCurrentIndex(currentIndex - 1);
    }
    else if(m_enabledCycling)
    {
        m_ComboBox->setCurrentIndex(m_ComboBox->count() - 1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void EnhancedComboBox::nextItem()
{
    int currentIndex = m_ComboBox->currentIndex();

    if(currentIndex < m_ComboBox->count() - 1)
    {
        m_ComboBox->setCurrentIndex(currentIndex + 1);
    }
    else if(m_enabledCycling)
    {
        m_ComboBox->setCurrentIndex(0);
    }
}