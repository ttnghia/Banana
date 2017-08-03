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

#include "PaintArea.h"

#include <QPainter>
#include <QMouseEvent>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PaintArea class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
PaintArea::PaintArea(QWidget* parent) :
    QWidget(parent),
    m_Image(400, 400, QImage::Format_RGB32),
    m_BrushColor(Qt::blue),
    m_BackgroundColor(Qt::white),
    m_BrushSize(3),
    m_LastMousePos(-1, -1)
{
    setAttribute(Qt::WA_StaticContents);
    setAttribute(Qt::WA_NoBackground);
    m_Image.fill(m_BackgroundColor);
    setMinimumWidth(1);
    setMinimumHeight(1);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QSize PaintArea::sizeHint() const
{
    return m_Image.size();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintArea::clear()
{
    m_Image.fill(m_BackgroundColor);
    update();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintArea::resize(int width, int height)
{
    if(width == 0 || height == 0)
        return;

    m_Image = m_Image.scaled(width, height, Qt::IgnoreAspectRatio);
    update();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PaintArea::openImage(const QString& fileName)
{
    QImage image;
    if(!image.load(fileName))
        return false;

    setImage(image);
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool PaintArea::saveImage(const QString& fileName, const char* fileFormat)
{
    return m_Image.save(fileName, fileFormat);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintArea::setImage(const QImage& image)
{
    m_Image = image.convertToFormat(QImage::Format_RGB32);
    update();
    updateGeometry();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintArea::setBackground(const QColor& color)
{
    m_BackgroundColor = color;
    m_Image.fill(m_BackgroundColor);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintArea::setBrushColor(const QColor& color)
{
    m_BrushColor = color;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintArea::setBrushSize(int size)
{
    m_BrushSize = size;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintArea::setBrushStyle(BrushStyle brushStyle)
{
    m_BrushStyle = brushStyle;
}

void PaintArea::setBrushLineStyle(PaintArea::LineStyle lineStyle)
{
    m_LineStyle = lineStyle;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintArea::paintEvent(QPaintEvent* /* event */)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), m_Image);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintArea::mousePressEvent(QMouseEvent* event)
{
    if(event->button() & Qt::LeftButton)
    {
        QPainter painter(&m_Image);
        setupPainter(painter);

        if(!m_PendingPath.isEmpty())
        {
            const QRectF    boundingRect = m_PendingPath.boundingRect();
            QLinearGradient gradient(boundingRect.topRight(), boundingRect.bottomLeft());
            gradient.setColorAt(0.0, QColor(m_BrushColor.red(), m_BrushColor.green(), m_BrushColor.blue(), 63));
            gradient.setColorAt(1.0, QColor(m_BrushColor.red(), m_BrushColor.green(), m_BrushColor.blue(), 191));
            painter.setBrush(gradient);
            painter.translate(event->pos() - boundingRect.center());
            painter.drawPath(m_PendingPath);

            m_PendingPath = QPainterPath();
#ifndef QT_NO_CURSOR
            unsetCursor();
#endif
            update();
        }
        else
        {
            const QRect rect = mousePress(m_BrushStyle, painter,
                                          event->pos());
            update(rect);

            m_LastMousePos = event->pos();
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintArea::mouseMoveEvent(QMouseEvent* event)
{
    if((event->buttons() & Qt::LeftButton) && m_LastMousePos != QPoint(-1, -1))
    {
        QPainter painter(&m_Image);
        setupPainter(painter);
        const QRect rect = mouseMove(m_BrushStyle, painter, m_LastMousePos, event->pos());
        update(rect);
        m_LastMousePos = event->pos();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintArea::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton && m_LastMousePos != QPoint(-1, -1))
    {
        QPainter painter(&m_Image);
        setupPainter(painter);
        QRect rect = mouseRelease(m_BrushStyle, painter, event->pos());
        update(rect);
        m_LastMousePos = QPoint(-1, -1);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintArea::setupPainter(QPainter& painter)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(m_BrushColor, m_BrushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QRect PaintArea::mousePress(PaintArea::BrushStyle brush, QPainter& painter, const QPoint& pos)
{
    return mouseMove(brush, painter, pos, pos);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QRect PaintArea::mouseRelease(PaintArea::BrushStyle /* brush */, QPainter& /* painter */, const QPoint& /* pos */)
{
    return QRect(0, 0, 0, 0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
QRect PaintArea::mouseMove(PaintArea::BrushStyle brush, QPainter& painter, const QPoint& oldPos, const QPoint& newPos)
{
    painter.save();

    int    rad          = painter.pen().width() / 2;
    QRect  boundingRect = QRect(oldPos, newPos).normalized().adjusted(-rad, -rad, +rad, +rad);
    QColor color        = painter.pen().color();
    int    thickness    = painter.pen().width();

    if(brush == PaintArea::BrushStyle::PencilBrush)
    {
        painter.drawLine(oldPos, newPos);
    }
    else if(brush == PaintArea::BrushStyle::AirBrush)
    {
        int numSteps = 2 + (newPos - oldPos).manhattanLength() / 2;

        painter.setBrush(QBrush(color, Qt::Dense6Pattern));
        painter.setPen(Qt::NoPen);

        for(int i = 0; i < numSteps; ++i)
        {
            int x = oldPos.x() + i * (newPos.x() - oldPos.x()) / (numSteps - 1);
            int y = oldPos.y() + i * (newPos.y() - oldPos.y()) / (numSteps - 1);

            painter.drawEllipse(x - (thickness / 2), y - (thickness / 2), thickness, thickness);
        }
    }

    painter.restore();
    return boundingRect;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Controller class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintAreaController::enableOpenImage(bool bEnableOpen)
{
    m_btnOpenImg->setVisible(bEnableOpen);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintAreaController::enableSaveImage(bool bEnableOpen)
{
    m_btnSaveImg->setVisible(bEnableOpen);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintAreaController::enableBrushStyleSelection(bool bEnableStyle)
{
    m_grBrushStyle->setVisible(bEnableStyle);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintAreaController::connectPaintArea()
{
    Q_ASSERT(m_PaintArea != nullptr);

    connect(m_pkrBrushColor,             &ColorPicker::colorRGBChanged, m_PaintArea, &PaintArea::setBrushColor);
    connect(m_sldBrushSize->getSlider(), &QSlider::valueChanged,        m_PaintArea, &PaintArea::setBrushSize);
    connect(m_rdPencilBrush,             &QRadioButton::toggled,        m_PaintArea, [&](bool checked)
            {
                if(checked)
                    m_PaintArea->setBrushStyle(PaintArea::BrushStyle::PencilBrush);
            });
    connect(m_rdAirBrush, &QRadioButton::toggled, m_PaintArea, [&](bool checked)
            {
                if(checked)
                    m_PaintArea->setBrushStyle(PaintArea::BrushStyle::AirBrush);
            });
    connect(m_btnClearImg, &QPushButton::clicked, m_PaintArea, &PaintArea::clear);
    connect(m_btnOpenImg,  &QPushButton::clicked, [&]
    {
        const QString fileName = QFileDialog::getOpenFileName(this, QString("Open File"), QDir::homePath());
        if(!fileName.isEmpty())
        {
            if(!m_PaintArea->openImage(fileName))
            {
                QMessageBox::information(this, QString("PaintArea"), QString("Cannot load image %1.").arg(fileName));
                return;
            }
            m_PaintArea->adjustSize();
        }
    });

    connect(m_btnSaveImg, &QPushButton::clicked, [&]
    {
        const QString initialPath = QDir::homePath() + "/untitled.png";

        const QString fileName = QFileDialog::getSaveFileName(this, QString("Save As"), initialPath);
        if(fileName.isEmpty())
        {
            return false;
        }
        else
        {
            return m_PaintArea->saveImage(fileName, "png");
        }
    });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintAreaController::setupGUI()
{
    QVBoxLayout* controlLayout = new QVBoxLayout;

    ////////////////////////////////////////////////////////////////////////////////
    // brush color
    m_pkrBrushColor = new ColorPicker(this, QSize(22, 22));
    m_pkrBrushColor->setColor(m_PaintArea != nullptr ? m_PaintArea->getBrushColor() : Qt::gray);

    controlLayout->addWidget(m_pkrBrushColor->getGroupBox("Brush Color:"));

    ////////////////////////////////////////////////////////////////////////////////
    // brush size
    m_sldBrushSize = new EnhancedSlider;
    m_sldBrushSize->setRange(1, 100);
    m_sldBrushSize->setValue(m_PaintArea != nullptr ? m_PaintArea->getBrushSize() : 30);
    controlLayout->addWidget(m_sldBrushSize->getGroupBox("Brush Size"));

    ////////////////////////////////////////////////////////////////////////////////
    // brush style
    m_rdPencilBrush = new QRadioButton("Pencil");;
    m_rdPencilBrush->setChecked(true);
    m_rdAirBrush = new QRadioButton("Air Brush");
    QHBoxLayout* layoutBrushStyle = new QHBoxLayout;
    layoutBrushStyle->addWidget(m_rdPencilBrush);
    layoutBrushStyle->addWidget(m_rdAirBrush);
    m_grBrushStyle = new QGroupBox("Brush Style");
    m_grBrushStyle->setLayout(layoutBrushStyle);
    controlLayout->addWidget(m_grBrushStyle);

    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* btnLayout = new QVBoxLayout;
    setupButtons(btnLayout);

    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(controlLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);
    setLayout(mainLayout);

    ////////////////////////////////////////1////////////////////////////////////////
    setFixedWidth(200);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PaintAreaController::setupButtons(QBoxLayout* ctrLayout)
{
    m_btnClearImg = new QPushButton("Clear");
    ctrLayout->addWidget(m_btnClearImg);

    ////////////////////////////////////////////////////////////////////////////////
    m_btnOpenImg = new QPushButton("Open");
    ctrLayout->addWidget(m_btnOpenImg);

    ////////////////////////////////////////////////////////////////////////////////
    m_btnSaveImg = new QPushButton("Save");
    ctrLayout->addWidget(m_btnSaveImg);
}
