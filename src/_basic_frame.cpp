#include "_basic_frame.h"


BasicFrame::BasicFrame(QWidget *p_parent) : QWidget(p_parent, Qt::Window | Qt::FramelessWindowHint)
{
    this->__init();
}

void BasicFrame::__init()
{
    this->setOpacity(0.8);
    this->setColor(Qt::darkBlue);
}

void BasicFrame::setColor(QColor p_Color)
{
    this->Color = p_Color;
    this->Palette.setColor(QPalette::Window, p_Color);
    this->setPalette(this->Palette);
}

void BasicFrame::setOpacity(double p_Opacity)
{
    this->Opacity = p_Opacity;
    this->setWindowOpacity(p_Opacity);
}

void BasicFrame::animate()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(500);
    animation->setEasingCurve(QEasingCurve(QEasingCurve::Type::InOutCubic));
    animation->setStartValue(QRect(0, 0, 100, 30));
    animation->setEndValue(QRect(250, 250, 100, 30));

    animation->start();
}
