//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
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

#include <QtAppHelpers/DataList.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
DataList::DataList(QWidget* parent, bool bAddEmptyItem /*= true*/, bool bAddOrderText /*= true*/, QString indexSeparator /*= QString("::")*/) :
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
    m_ListFile = listFile;
    QFile textFile(listFile);
    if(!textFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Cannot open file '" + listFile + "' for reading.");
        return;
    }

    clear();
    QTextStream in(&textFile);
    while(!in.atEnd()) {
        addItem(in.readLine());
    }

    setListCurrentIndex(0);
    m_lblStatus->setText(QString("Loaded list from %1").arg(listFile)); \
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

    if(m_bAddOrderText) {
        m_ListWidget->addItem(QString("%1%2     %3").arg(m_ListWidget->count()).arg(m_IndexSeparator).arg(dataStr));
    } else {
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

    if(m_bAddEmptyItem) {
        m_ListWidget->addItem("Nothing");
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int DataList::getListSize() const
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
    if(obj == m_ListWidget && e->type() == QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(e);

        switch(key->key()) {
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
                m_ListWidget->setCurrentRow((int)(key->key() - Qt::Key_0));
                break;
            default:
                ;
        }   // end switch

        return true;
    } else {
        return false;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DataList::setupGUI()
{
    m_ListWidget = new QListWidget;

    if(m_bAddEmptyItem) {
        m_ListWidget->addItem("Nothing");
    }

    m_ListWidget->installEventFilter(this);

    ////////////////////////////////////////////////////////////////////////////////
    m_lblStatus = new QLabel(this);
    m_lblStatus->setMargin(5);
    m_lblStatus->setAlignment(Qt::AlignLeft);

    QPushButton* btnReload = new QPushButton("Reload");
    connect(btnReload, &QPushButton::clicked, [&]
        {
            loadListFromFile(m_ListFile);
        });

    QHBoxLayout* statusLayout = new QHBoxLayout;
    statusLayout->addWidget(m_lblStatus, 4);
    statusLayout->addWidget(btnReload,   1);

    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_ListWidget);
    layout->addLayout(statusLayout);
    setLayout(layout);

    connect(m_ListWidget, &QListWidget::currentRowChanged, [&](int currentRow)
            {
                if(currentRow < 0) {
                    return;
                }

                if(!m_bAddEmptyItem || (m_bAddEmptyItem && currentRow != 0)) {
                    emit currentRowChanged(currentRow);
                    emit currentTextChanged(m_DataList[m_bAddEmptyItem ? currentRow - 1 : currentRow]);
                }
            });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana