#include "frame.h"
#include <QToolButton>
#include <QPainterPath>

using namespace Gates;

Frame::Frame(QString frameTitle, QWidget *p_parent) : QWidget(p_parent)
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_PaintOnScreen);
    this->setAttribute(Qt::WA_TranslucentBackground);

    this->_title = frameTitle;
    this->_nameBar = new NameBar(this);
    this->_iconView = new IconView(this);

    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget(_iconView);
    layout->addWidget(_nameBar);
    this->setLayout(layout);
}

Frame::Frame(QString frameTitle, QString p_filename, QWidget *p_parent) : QWidget(p_parent)
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_PaintOnScreen);
    this->setAttribute(Qt::WA_TranslucentBackground);

    this->_title = frameTitle;
    this->_nameBar = new NameBar(this);
    this->_iconView = new IconView(p_filename, this);

    QVBoxLayout * layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    QSizePolicy iconViewPolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    _iconView->setSizePolicy(iconViewPolicy);

    layout->addWidget(_iconView, Qt::AlignTop);
    layout->addWidget(_nameBar, Qt::AlignBottom);
    this->setLayout(layout);
    this->setGeometry(300, 300, 400, 400);
}


void Frame::animateClose()
{
    int duration = 200;

    QRect FrameStart    = this->rect().translated(mapToGlobal(QPoint(0, 0)));
    QRect FrameEnd      = FrameStart.adjusted(0, 0, 0, -_iconView->height());

    globalGeometry = FrameStart;

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");

    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve(QEasingCurve::Type::InOutCubic)); //InOutCubic

    animation->setStartValue(FrameStart);
    animation->setEndValue(FrameEnd);
    animation->start();
}
void Frame::animateOpen()
{
    int duration = 200;

    QRect FrameStart    = this->rect().translated(mapToGlobal(QPoint(0, 0)));
    QRect FrameEnd      = globalGeometry;

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");

    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve(QEasingCurve::Type::InOutCubic)); //InOutCubic

    animation->setStartValue(FrameStart);
    animation->setEndValue(FrameEnd);
    animation->start();
}
void Frame::hideAnimation()
{
    static bool open = true;

    if(open){
        this->animateClose();
        open = false;
    }else{
//        this->setGeometry(0, 0, 500, 300);
        this->animateOpen();
        open = true;
    }
    return;
}


void Frame::paintEvent(QPaintEvent *)
{
    QRect selfRect = this->rect().adjusted(0, 0, -1, -1);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(selfRect, 15, 15, Qt::AbsoluteSize);

//    painter.setPen(QPen(Qt::black, 8));
//    painter.setOpacity(0.6);
//    painter.drawPath(path);

    painter.setPen(QPen(Qt::black, 1));
    painter.setOpacity(0.6);
    painter.fillPath(path, Qt::black);


//    painter.setOpacity(0.3);
//    painter.fillRect(selfRect, Qt::blue);
//    painter.setBrush(Qt::darkCyan);
//    painter.drawRect(selfRect);
}
void Frame::mousePressEvent(QMouseEvent *event)
{

}


Gates::Frame::NameBar::NameBar(Frame *frame) : QWidget(nullptr)
{
    this->frame = frame;

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_PaintOnScreen);
    this->setAttribute(Qt::WA_TranslucentBackground);

    this->setFixedHeight(60);
    this->setFixedWidth(700);


    QPushButton * hideButton = new QPushButton("Hide");
    hideButton->setFixedSize(30, 30);
    QObject::connect(hideButton, SIGNAL(pressed()), this->frame, SLOT(hideAnimation()));

    QPushButton * menuButton = new QPushButton("Menu");
    menuButton->setFixedSize(30, 30);
//    QObject::connect(menuButton, SIGNAL(pressed()), this, SLOT(menuButtonPressed()));

    QLabel      * titleLabel = new QLabel(frame->title());
    titleLabel->setFont(QFont("Helvetica", 12));
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
