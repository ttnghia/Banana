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
        currentColor(Qt::blue)
    {
        setMouseTracking(true);
        setAutoFillBackground(true);
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    void setColor(QColor _color)
    {
        QPalette palette = this->palette();
        palette.setColor(QPalette::Window, _color);
        this->setPalette(palette);

        currentColor = _color;
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
        QColor color = QColorDialog::getColor(currentColor, this);
        if(color.isValid())
        {
            setColor(color);

            emit colorChanged((float)color.red() / 255.0f, (float)color.green() / 255.0f,
                (float)color.blue() / 255.0f);
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
    QColor currentColor;
};
