#include "frame.h"

using namespace Gates;

Frame::Frame(QString frameTitle, QWidget *p_parent) : QWidget(p_parent)
{
    this->frameTitle = frameTitle;
}

Frame::Frame(QString p_filename, QString frameTitle, QWidget *p_parent) : QWidget(p_parent)
{
    this->iconView = new IconView(p_filename, p_parent);
    this->frameTitle = frameTitle;
}

void Frame::setOpacity(double p_Opacity)
{
    this->setWindowOpacity(p_Opacity);
    this->opacity = p_Opacity;
}

void Frame::setPalette(QPalette p_Palette)
{
    this->palette = p_Palette;
}

//void Frame::disappear()
//{
//    QGraphicsOpacityEffect * show_effect = new QGraphicsOpacityEffect(this);
//    this->setGraphicsEffect(show_effect);
//    QPropertyAnimation * animation = new QPropertyAnimation(show_effect, "opacity");
//    animation->setDuration(200);
//    animation->setStartValue(1);
//    animation->setEndValue(0);

//    animation->start();
////    this->setVisible(false);
//}

//void Frame::appear()
//{
//    QPropertyAnimation * animation = new QPropertyAnimation(this, "opacity");
//    animation->setDuration(200);
//    animation->setStartValue(0);
//    animation->setEndValue(this->opacity);

////    this->setVisible(true);
//    animation->start();
//}


void Frame::animateOpen()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve(QEasingCurve::Type::InOutCubic)); //InOutCubic
    animation->setStartValue(QRect(100, 0, 400, 30));
    animation->setEndValue(QRect(100, 0, 400, 300));

    animation->start();
//    this->minimised = false;
}

void Frame::animateClose()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve(QEasingCurve::Type::InOutCubic)); //InOutCubic
    animation->setStartValue(QRect(100, 0, 400, 300));
    animation->setEndValue(QRect(100, 0, 400, 30));

    animation->start();
//    this->minimised = true;
}

void Frame::animateCycle()
{
    QPropertyAnimation *animationo = new QPropertyAnimation(this, "geometry");
    animationo->setDuration(200);
    animationo->setEasingCurve(QEasingCurve(QEasingCurve::Type::InOutCubic)); //InOutCubic
    animationo->setStartValue(QRect(100, 0, 400, 30));
    animationo->setEndValue(QRect(100, 0, 400, 300));

    QPropertyAnimation *animationc = new QPropertyAnimation(this, "geometry");
    animationc->setDuration(200);
    animationc->setEasingCurve(QEasingCurve(QEasingCurve::Type::InOutCubic)); //InOutCubic
    animationc->setStartValue(QRect(100, 0, 400, 300));
    animationc->setEndValue(QRect(100, 0, 400, 30));


}

void Frame::mousePressEvent(QMouseEvent *event)
{
    this->animateClose();
}



#include <QToolButton>

//void Frame::add_icon(QString p_fileName)
//{
//    IconItem * add = new IconItem(p_fileName);
////    this->Layout->addWidget(add);
//    add->setWindowOpacity(1);

//}


//void Frame::mousePressEvent(QMouseEvent *event)
//{
//    if(event->button() == Qt::RightButton)
//        this->close();
//    if(event->button() == Qt::LeftButton){
////        if(minimised)
//            animateOpen();
//        else
//            animateClose();
//    }
//}


Gates::NameBar::NameBar(QString title, Frame *frame) : QWidget(nullptr)
{
    this->frame = frame;
    this->title = title;

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_PaintOnScreen);
    this->setAttribute(Qt::WA_TranslucentBackground);

    this->setFixedHeight(60);
    this->setFixedWidth(700);


    QPushButton * hideButton = new QPushButton("Hide");
    hideButton->setFixedSize(30, 30);
    QObject::connect(hideButton, SIGNAL(pressed()), this, SLOT(hideButtonPressed()));

    QPushButton * menuButton = new QPushButton("Menu");
    menuButton->setFixedSize(30, 30);
    QObject::connect(menuButton, SIGNAL(pressed()), this, SLOT(menuButtonPressed()));

    QLabel      * titleLabel = new QLabel(title);
    QPalette sample_palette;
    sample_palette.setColor(QPalette::WindowText, Qt::white);
    titleLabel->setPalette(sample_palette);
    QSizePolicy titleSizePolicy;
    titleSizePolicy.setHorizontalPolicy(QSizePolicy::Maximum);
    titleLabel->setSizePolicy(titleSizePolicy);

    QHBoxLayout * layout = new QHBoxLayout;

    layout->addWidget(hideButton, 0, Qt::AlignLeft);
    layout->addWidget(titleLabel);
    layout->addWidget(menuButton, 0, Qt::AlignRight | Qt::AlignBaseline);

    this->setLayout(layout);
}
