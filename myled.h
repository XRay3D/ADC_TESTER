#ifndef MYLED_H
#define MYLED_H

#include <QWidget>

class MyLed : public QWidget {
    Q_OBJECT
public:
    explicit MyLed(QWidget* parent = nullptr);

    QColor getLedColor() const;
    void setLedColor(const QColor &value);

signals:

public slots:

protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void drawLed(QColor color);

    int height;
    int width;
    int minDim;
    double half;
    double centerX;
    double centerY;
    QRect drawingRect;

    double outerBorderWidth;
    double innerBorderWidth;
    double outerBorderRadius;
    double innerBorderRadius;
    double topReflexY;
    double bottomReflexY;
    double topReflexWidth;
    double topReflexHeight;
    double bottomReflexWidth;
    double bottomReflexHeight;

    QColor ledColor;
    QColor lightColor;
    QColor shadowColor;
    QColor ringShadowDarkColor;
    QColor ringShadowMedColor;
    QColor ringShadowLightColor;
    QColor topReflexUpColor;
    QColor topReflexDownColor;
    QColor bottomReflexCenterColor;
    QColor bottomReflexSideColor;
};

#endif // MYLED_H
