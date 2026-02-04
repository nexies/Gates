#pragma once

#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QtCore>

class RoundedRect : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT

public:

    enum Corner
    {
        TopLeftCorner = 0x01,
        TopRightCorner = 0x02,
        BottomLeftCorner = 0x04,
        BottomRightCorner = 0x08,

        TopCorners = TopLeftCorner | TopRightCorner,
        BottomCorners = BottomRightCorner | BottomLeftCorner,

        LeftCorners = TopLeftCorner | BottomLeftCorner,
        RightCorners = TopRightCorner | BottomRightCorner,
        AllCorners = TopLeftCorner | TopRightCorner | BottomLeftCorner |BottomRightCorner,
        NoCorners = 0
    };

    Q_DECLARE_FLAGS(Corners, Corner)
    Q_FLAG(Corner)

    Q_PROPERTY(bool antialising READ antialising WRITE setAntialsing)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(int radius READ radius WRITE setRadius)

    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
    Q_PROPERTY(float borderOpacityFactor READ borderOpacityFactor WRITE borderOpacityFactor)

    Q_PROPERTY(Corners roundedCorners READ roundedCorners WRITE setRoundedCorners)

    explicit RoundedRect(QQuickItem * parent = nullptr);
    void paint(QPainter * painter) override;

    bool antialising() const;
    void setAntialsing(bool newAntialising);

    QColor color() const;
    void setColor(const QColor &newColor);

    int radius() const;
    void setRadius(int newRadius);

    QColor borderColor() const;
    void setBorderColor(const QColor &newBorderColor);

    Corners roundedCorners() const;
    void setRoundedCorners(Corners newRoundedCorners);

    float borderOpacityFactor() const;
    void borderOpacityFactor(float newBorderOpacityFactor);

    int borderWidth() const;
    void setBorderWidth(int newBorderWidth);

private:
    bool m_antialising { false };
    QColor m_color { Qt::black };

    int m_radius { 0 };
    QColor m_borderColor { Qt::black };
    float m_borderOpacityFactor { 2 };

    Corners m_roundedCorners { AllCorners };
    int m_borderWidth { 0 };
};
