//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 1/21/2017
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

#include <QtGui>
#include <QtWidgets>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ColorSelector : public QWidget
{
    Q_OBJECT
public:
    explicit ColorSelector(QWidget *parent = 0) : QWidget(parent),
        m_CurrentColor(Qt::blue)
    {
        setMouseTracking(true);
        setAutoFillBackground(true);
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setColor(QColor color)
    {
        QPalette palette = this->palette();
        palette.setColor(QPalette::Window, color);
        this->setPalette(palette);

        m_CurrentColor = color;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    bool event(QEvent *e)
    {
        return QWidget::event(e);
    }

signals:
    void colorChanged(float r, float g, float b);

protected:
    void mousePressEvent(QMouseEvent *)
    {
        QColor color = QColorDialog::getColor(m_CurrentColor, this);
        if(color.isValid())
        {
            setColor(color);

            emit colorChanged(static_cast<float>(color.red()) / 255.0,
                              static_cast<float>(color.green()) / 255.0,
                              static_cast<float>(color.blue()) / 255.0);
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void enterEvent(QEvent *)
    {
        QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void leaveEvent(QEvent *)
    {
        QApplication::restoreOverrideCursor();
    }

private:
    QColor m_CurrentColor;
};
