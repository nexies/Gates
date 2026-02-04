#include "RoundedRect.h"
#include <QPainterPath>
#include <QPainter>

RoundedRect::RoundedRect(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{

}

void RoundedRect::paint(QPainter *painter)
{
    QPainterPath path;

    int h = height();
    int w = width();
    int r = radius();
    int d = r * 2;
    int b = borderWidth();

    path.setFillRule(Qt::WindingFill);
    path.addRect(r, 0, w - d, h);
    path.addRect(0, r, w, h - d);

    if(roundedCorners () & TopLeftCorner)
        path.addEllipse(0, 0, d, d);
    else
        path.addRect(0, 0, r, r);

    if(roundedCorners() & TopRightCorner)
        path.addEllipse(w - d, 0, d, d);
    else
        path.addRect(w - r, 0, r, r);

    if(roundedCorners() & BottomRightCorner)
        path.addEllipse(w - d, h - d, d, d);
    else
        path.addRect(w - r, h - r, r, r);

    if(roundedCorners() & BottomLeftCorner)
        path.addEllipse(0, h - d, d, d);
    else
        path.addRect(0, h - r, r, r);


    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setOpacity(this->opacity());
    painter->fillPath(path, this->color());

    painter->setOpacity(this->opacity() * this->borderOpacityFactor());


    if(borderWidth() > 0) {
        painter->setPen(QPen(this->borderColor(), b, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));

        QTransform transform;
        transform.scale(1- b/qreal(w), 1- b/qreal(h));
        transform.translate(b/2, b/2);
        painter->setTransform(transform);
        painter->drawPath(path.simplified());
    }

    painter->restore();
}

bool RoundedRect::antialising() const
{
    return m_antialising;
}

void RoundedRect::setAntialsing(bool newAntialising)
{
    m_antialising = newAntialising;
}

QColor RoundedRect::color() const
{
    return m_color;
}

void RoundedRect::setColor(const QColor &newColor)
{
    m_color = newColor;
}

int RoundedRect::radius() const
{
    return m_radius;
}

void RoundedRect::setRadius(int newRadius)
{
    if(newRadius < 0)
        newRadius = 0;
    m_radius = newRadius;
}

QColor RoundedRect::borderColor() const
{
    return m_borderColor;
}

void RoundedRect::setBorderColor(const QColor &newBorderColor)
{
    m_borderColor = newBorderColor;
}

RoundedRect::Corners RoundedRect::roundedCorners() const
{
    return m_roundedCorners;
}

void RoundedRect::setRoundedCorners(Corners newRoundedCorners)
{
    m_roundedCorners = newRoundedCorners;
}

float RoundedRect::borderOpacityFactor() const
{
    return m_borderOpacityFactor;
}

void RoundedRect::borderOpacityFactor(float newBorderOpacityFactor)
{
    m_borderOpacityFactor = newBorderOpacityFactor;
}

int RoundedRect::borderWidth() const
{
    return m_borderWidth;
}

void RoundedRect::setBorderWidth(int newBorderWidth)
{
    if(newBorderWidth < 0)
        newBorderWidth = 0;
    m_borderWidth = newBorderWidth;
}
