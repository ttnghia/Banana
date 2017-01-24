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
class BrowsePathWidget : public QWidget
{
    Q_OBJECT

public:
    BrowsePathWidget(QString caption, bool folderOnly = true,
                     QWidget *parent = 0) :
        QWidget(parent),
        m_GroupBox(nullptr),
        m_bFolderOnly(folderOnly)
    {
        QPushButton* btnBrowse = new QPushButton(caption);
        setupGui(btnBrowse);
    }

    BrowsePathWidget(QIcon icon, bool folderOnly = true,
                     QWidget *parent = 0) :
        QWidget(parent),
        m_GroupBox(nullptr),
        m_bFolderOnly(folderOnly)
    {
        QToolButton* btnBrowse = new QToolButton;
        btnBrowse->setIcon(icon);
        setupGui(btnBrowse);
    }

    void setupGui(QWidget* button)
    {
        m_txtPath = new QLineEdit;
        m_txtPath->setEnabled(false);

        m_Layout = new QHBoxLayout;
        m_Layout->setSpacing(10);
        m_Layout->addWidget(m_txtPath, 1);
        m_Layout->addItem(new QSpacerItem(10, 20, QSizePolicy::Expanding,
                          QSizePolicy::Minimum));
        m_Layout->addWidget(button);

        m_Button = dynamic_cast<QAbstractButton*>(button);
        Q_ASSERT(m_Button != nullptr);
        connect(m_Button, SIGNAL(clicked()), this, SLOT(browse()));

        m_CurrentPath = QDir::homePath();
    }

    void setEnabled(bool enabled)
    {
        m_Button->setEnabled(enabled);
    }

    ~BrowsePathWidget()
    {
        delete m_txtPath;
        delete m_GroupBox;
        delete m_Layout;
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

    public slots:
    void browse()
    {
        QString selectedPath = m_bFolderOnly ?
            QFileDialog::getExistingDirectory(nullptr, QString("Select path"),
                                              m_CurrentPath,
                                              QFileDialog::ShowDirsOnly
                                              | QFileDialog::DontResolveSymlinks) :
            QFileDialog::getExistingDirectory(nullptr, QString("Select path"),
                                              m_CurrentPath,
                                              QFileDialog::DontResolveSymlinks);

        if(selectedPath.trimmed() != "")
        {
            m_CurrentPath = selectedPath;
            m_txtPath->setText(m_CurrentPath);
            emit pathChanged(selectedPath);
        }
    }

signals:
    void pathChanged(QString path);

private:
    QLineEdit*       m_txtPath;
    QGroupBox*       m_GroupBox;
    QHBoxLayout*     m_Layout;
    QAbstractButton* m_Button;
    QString          m_CurrentPath;
    bool             m_bFolderOnly;
};
