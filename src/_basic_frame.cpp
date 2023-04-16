#include "_basic_frame.h"


BasicFrame::BasicFrame(QWidget *p_parent) : QWidget(p_parent, Qt::Window | Qt::FramelessWindowHint)
{
    this->__init();
}

void BasicFrame::__init()
{
    this->setOpacity(0.8);
    this->setColor(Qt::darkBlue);
    this->setMouseTracking(true);
    this->Layout = new QGridLayout;
    this->setLayout(Layout);
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

void BasicFrame::animateOpen()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve(QEasingCurve::Type::InOutCubic)); //InOutCubic
    animation->setStartValue(QRect(100, 0, 400, 30));
    animation->setEndValue(QRect(100, 0, 400, 300));

    animation->start();
    this->minimised = false;
}

void BasicFrame::animateClose()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve(QEasingCurve::Type::InOutCubic)); //InOutCubic
    animation->setStartValue(QRect(100, 0, 400, 300));
    animation->setEndValue(QRect(100, 0, 400, 30));

    animation->start();
    this->minimised = true;
}

#include <QThread>

void BasicFrame::animateCycle()
{
    while(true){
        this->animateOpen();
//        QThread::sleep(2);
//        this->animateClose();
//        QThread::sleep(2);
    }
}



#include <QToolButton>

void BasicFrame::add_icon()
{
    Icon * add = new Icon;
    this->Layout->addWidget(add);
    add->setWindowOpacity(1);

}


void BasicFrame::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
        this->close();
    if(event->button() == Qt::LeftButton){
        if(minimised)
            animateOpen();
        else
            animateClose();
    }
}

