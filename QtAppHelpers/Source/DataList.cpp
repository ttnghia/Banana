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

#include <QtAppHelpers/DataList.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
DataList::DataList(QWidget *parent, bool bAddEmptyItem /*= true*/, bool bAddOrderText /*= true*/, QString indexSeparator /*= QString("::")*/) :
    QWidget(parent), m_ListWidget(nullptr), m_bAddEmptyItem(bAddEmptyItem), m_bAddOrderText(bAddOrderText), m_IndexSeparator(indexSeparator)
{
    setWindowTitle("Data List");
    setFocusPolicy(Qt::StrongFocus);
    setupGUI();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
DataList::~DataList()
{
    m_DataList.clear();
    m_ListWidget->clear();
    delete m_ListWidget;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataList::loadListFromFile(const QString& listFile)
{

    QFile textFile(listFile);
    if(!textFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error: Cannot open file" << listFile << "for reading.";
        return;
    }

    QTextStream in(&textFile);
    while(!in.atEnd())
    {
        addItem(in.readLine());
    }

    setListCurrentIndex(0);
    m_lblStatus->setText(QString("Loaded list from %1").arg(listFile));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataList::setListCurrentIndex(int index)
{
    m_ListWidget->setCurrentRow(index);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataList::addItem(QString dataStr)
{
    m_DataList.push_back(dataStr);

    if(m_bAddOrderText)
    {
        m_ListWidget->addItem(QString("%1%2     %3").arg(m_ListWidget->count()).arg(m_IndexSeparator).arg(dataStr));
    }
    else
    {
        m_ListWidget->addItem(dataStr);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataList::removeIndex(int index)
{
    m_DataList.removeAt(index);
    m_ListWidget->takeItem(index);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataList::clear()
{
    m_DataList.clear();
    m_ListWidget->clear();

    if(m_bAddEmptyItem)
        m_ListWidget->addItem("Nothing");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int DataList::getListSize()
{
    return m_DataList.count();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QSize DataList::sizeHint() const
{
    return QSize(800, 600);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool DataList::eventFilter(QObject* obj, QEvent* e)
{
    if(obj == m_ListWidget && e->type() == QEvent::KeyPress)
    {
        QKeyEvent* key = static_cast<QKeyEvent*>(e);

        switch(key->key())
        {
            case Qt::Key_0:
            case Qt::Key_1:
            case Qt::Key_2:
            case Qt::Key_3:
            case Qt::Key_4:
            case Qt::Key_5:
            case Qt::Key_6:
            case Qt::Key_7:
            case Qt::Key_8:
            case Qt::Key_9:
                m_ListWidget->setCurrentRow((int)(key->key() - Qt::Key_0) + 1);
                break;
            default:
                ;
        } // end switch

        return true;
    }
    else
    {
        return false;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataList::setupGUI()
{
    m_ListWidget = new QListWidget;

    if(m_bAddEmptyItem)
        m_ListWidget->addItem("Nothing");

    m_ListWidget->installEventFilter(this);

    m_lblStatus = new QLabel(this);
    m_lblStatus->setMargin(5);
    m_lblStatus->setAlignment(Qt::AlignLeft);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_ListWidget);
    layout->addWidget(m_lblStatus);
    setLayout(layout);

    connect(m_ListWidget, &QListWidget::currentRowChanged, this, [&](int currentRow)
    {
        if(!m_bAddEmptyItem || (m_bAddEmptyItem && currentRow != 0))
        {
            emit currentRowChanged(currentRow);
            emit currentTextChanged(m_DataList[m_bAddEmptyItem ? currentRow - 1 : currentRow]);
        }
    });

}
