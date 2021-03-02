#include "myled.h"

#include <QBrush>
#include <QColor>
#include <QGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>

MyLed::MyLed(QWidget* parent)
    : QWidget(parent)
    , ledColor(Qt::yellow)
    , lightColor(QColor(0xE0, 0xE0, 0xE0))
    , shadowColor(QColor(0x70, 0x70, 0x70))
    , ringShadowDarkColor(QColor(0x50, 0x50, 0x50, 0xFF))
    , ringShadowMedColor(QColor(0x50, 0x50, 0x50, 0x20))
    , ringShadowLightColor(QColor(0xEE, 0xEE, 0xEE, 0x00))
    , topReflexUpColor(QColor(0xFF, 0xFF, 0xFF, 0xA0))
    , topReflexDownColor(QColor(0xFF, 0xFF, 0xFF, 0x00))
    , bottomReflexCenterColor(QColor(0xFF, 0xFF, 0xFF, 0x00))
    , bottomReflexSideColor(QColor(0xFF, 0xFF, 0xFF, 0x70))
{
    //setMinimumSize(40, 40);
}

void MyLed::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    height = size().height();
    width = size().width();
    minDim = (height > width) ? width : height;
    half = minDim / 2.0;
    centerX = width / 2.0;
    centerY = height / 2.0;

    outerBorderWidth = minDim / 13.0;
    innerBorderWidth = minDim / 15.0;
    outerBorderRadius = half - outerBorderWidth;
    innerBorderRadius = half - (outerBorderWidth + innerBorderWidth);

    topReflexY = centerY - (half - outerBorderWidth - innerBorderWidth) / 2.0;
    bottomReflexY = centerY + (half - outerBorderWidth - innerBorderWidth) / 2.0;
    topReflexHeight = half / 5.0;
    topReflexWidth = half / 3.0;
    bottomReflexHeight = half / 5.0;
    bottomReflexWidth = half / 3.0;

    drawingRect.setTop((height - minDim) / 2.0);
    drawingRect.setLeft((width - minDim) / 2.0);
    drawingRect.setHeight(minDim);
    drawingRect.setWidth(minDim);
}

void MyLed::drawLed(QColor color)
{
    if (!isEnabled())
        color = Qt::gray;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    QPen pen;
    pen.setStyle(Qt::NoPen);
    painter.setPen(pen);
    /////////////////
    QRadialGradient outerRingGradient(QPointF(centerX, centerY - outerBorderRadius - (outerBorderWidth / 2)), minDim - (outerBorderWidth / 2));
    outerRingGradient.setColorAt(0, lightColor);
    outerRingGradient.setColorAt(1, shadowColor);
    QBrush outerRingBrush(outerRingGradient);
    painter.setBrush(outerRingBrush);
    painter.drawEllipse(drawingRect);
    /////////////////
    QRadialGradient innerRingGradient(QPointF(centerX, centerY + innerBorderRadius + (innerBorderWidth / 2)), minDim - (innerBorderWidth / 2));
    innerRingGradient.setColorAt(0, lightColor);
    innerRingGradient.setColorAt(1, shadowColor);
    QBrush innerRingBrush(innerRingGradient);
    painter.setBrush(innerRingBrush);
    painter.drawEllipse(QPointF(centerX, centerY), outerBorderRadius, outerBorderRadius);
    /////////////////
    QColor dark(color.darker(120));
    QRadialGradient glassGradient(QPointF(centerX, centerY), innerBorderRadius);
    glassGradient.setColorAt(0, color);
    glassGradient.setColorAt(1, dark);
    QBrush glassBrush(glassGradient);
    painter.setBrush(glassBrush);
    painter.drawEllipse(QPointF(centerX, centerY), innerBorderRadius, innerBorderRadius);
    /////////////////
    QRadialGradient shadowGradient(QPointF(centerX, centerY), innerBorderRadius);
    shadowGradient.setColorAt(0, ringShadowLightColor);
    shadowGradient.setColorAt(0.85, ringShadowMedColor);
    shadowGradient.setColorAt(1, ringShadowDarkColor);
    QBrush shadowBrush(shadowGradient);
    painter.setBrush(shadowBrush);
    painter.drawEllipse(QPointF(centerX, centerY), innerBorderRadius, innerBorderRadius);
    /////////////////
    QLinearGradient topTeflexGradient(QPointF(centerX, (innerBorderWidth + outerBorderWidth)), QPoint(centerX, centerY));
    topTeflexGradient.setColorAt(0, topReflexUpColor);
    topTeflexGradient.setColorAt(1, topReflexDownColor);
    QBrush topReflexbrush(topTeflexGradient);
    painter.setBrush(topReflexbrush);
    painter.drawEllipse(QPointF(centerX, topReflexY), topReflexWidth, topReflexHeight);
    QRadialGradient bottomReflexGradient(QPointF(centerX, bottomReflexY + (bottomReflexHeight / 2)), bottomReflexWidth);
    bottomReflexGradient.setColorAt(0, bottomReflexSideColor);
    bottomReflexGradient.setColorAt(1, bottomReflexCenterColor);
    QBrush bottomReflexBrush(bottomReflexGradient);
    painter.setBrush(bottomReflexBrush);
    painter.drawEllipse(QPointF(centerX, bottomReflexY), bottomReflexWidth, bottomReflexHeight);
    /////////////////
}

QColor MyLed::getLedColor() const
{
    return ledColor;
}

void MyLed::setLedColor(const QColor& value)
{
    ledColor = value;
    update();
}

void MyLed::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    drawLed(ledColor);
}

void MyLed::mousePressEvent(QMouseEvent* event)
{
    static int count = 0;
    if (event->button() == Qt::LeftButton) {
        switch (count) {
        case 0:
            ledColor = Qt::red;
            count++;
            break;
        case 1:
            ledColor = QColor(255, 127, 0);
            count++;
            break;
        default:
            ledColor = Qt::green;
            count = 0;
            break;
        }
        repaint();
    }
    QWidget::mousePressEvent(event);
}
