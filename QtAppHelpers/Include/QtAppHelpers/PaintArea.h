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

#pragma once

#include <QtWidgets>

#include <QtAppHelpers/ColorPicker.h>
#include <QtAppHelpers/EnhancedSlider.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PaintArea class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PaintArea : public QWidget
{
    Q_OBJECT

public:
    enum class BrushStyle
    {
        PencilBrush = 0,
        AirBrush    = 1
    };

    // todo: unimplemented
    enum class LineStyle
    {
        SolidLine      = 0,
        DashLine       = 1,
        DotLine        = 2,
        DashDotLine    = 3,
        DashDotDotLine = 4
    };

    PaintArea(QWidget* parent = nullptr);

    QSize sizeHint() const override;
    QImage getImage() const { return m_Image; }
    QColor getBackgroundColor() const { return m_BackgroundColor; }
    QColor getBrushColor() const { return m_BrushColor; }
    BrushStyle getBrushStyle() const { return m_BrushStyle; }
    LineStyle getLineStyle() const { return m_LineStyle; }
    int getBrushSize() const { return m_BrushSize; }

signals:
    void paintingStarted();
    void paintingFinished();

public slots:
    void clear();
    void resize(int width, int height);
    bool openImage(const QString& fileName);
    bool saveImage(const QString& fileName, const char* fileFormat);
    void setImage(const QImage& image);
    void setBackground(const QColor& color);
    void setBrushColor(const QColor& color);
    void setBrushSize(int size);
    void setBrushStyle(PaintArea::BrushStyle brushStyle);
    void setBrushLineStyle(PaintArea::LineStyle lineStyle);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void  setupPainter(QPainter& painter);
    QRect mousePress(BrushStyle m_BrushStyle, QPainter& painter, const QPoint& pos);
    QRect mouseRelease(BrushStyle, QPainter&, const QPoint&);
    QRect mouseMove(BrushStyle m_BrushStyle, QPainter& painter, const QPoint& oldPos, const QPoint& newPos);

    ////////////////////////////////////////////////////////////////////////////////
    BrushStyle m_BrushStyle      = PaintArea::BrushStyle::PencilBrush;
    LineStyle  m_LineStyle       = PaintArea::LineStyle::SolidLine;
    QImage     m_Image           = QImage(400, 400, QImage::Format_RGB32);
    QColor     m_BackgroundColor = Qt::white;
    QColor     m_BrushColor      = Qt::blue;
    int        m_BrushSize       = 3;
    QPoint     m_LastMousePos    = QPoint(-1, -1);
    bool       m_bTimeout        = false;
    int        m_Timeout         = 1000;

    QPainterPath m_PendingPath;
    QTimer       m_Timer;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Controller class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PaintAreaController : public QWidget
{
    Q_OBJECT

public:
    explicit PaintAreaController(QWidget* parent = nullptr, PaintArea* paintArea = nullptr) : QWidget(parent), m_PaintArea(paintArea) { setupGUI(); }

    void enableOpenImage(bool bEnableOpen);
    void enableSaveImage(bool bEnableOpen);
    void enableBrushStyleSelection(bool bEnableStyle);
    void connectPaintArea();

    QPushButton* m_btnClearImg;
    QPushButton* m_btnOpenImg;
    QPushButton* m_btnSaveImg;
private:
    void setupGUI();
    void setupButtons(QBoxLayout* ctrLayout);

    ////////////////////////////////////////////////////////////////////////////////
    PaintArea*      m_PaintArea;
    ColorPicker*    m_pkrBrushColor;
    EnhancedSlider* m_sldBrushSize;
    QGroupBox*      m_grBrushStyle;
    QRadioButton*   m_rdPencilBrush;
    QRadioButton*   m_rdAirBrush;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana