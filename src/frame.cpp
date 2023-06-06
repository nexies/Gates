#include "frame.h"
#include <QToolButton>
#include <QPainterPath>
#include <QTextStream>

using namespace Gates;

QTextStream & operator << (QTextStream & stream, const QPoint & point){
    stream << "(" << point.x() << ", " << point.y() << ")";
    return stream;
}

QTextStream & operator << (QTextStream & stream, const QRect & rect){
    stream << "(" << rect.x() << ", " << rect.y();
    stream << ", " << rect.width() << ", " << rect.height() << ")";
    return stream;
}

QTextStream qout(stdout);

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

    openStateRect = this->geometry();
    closedStateRect = this->_nameBar->geometry();

    qout << "INIT: " << openStateRect << " " << closedStateRect << endl;
}


void Frame::animateClose()
{
    if(closed)
        return;

    int duration = 200;

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    connect(animation, SIGNAL(finished()), this, SLOT(finishedAnimating()));

    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve(QEasingCurve::Type::InOutCubic)); //InOutCubic

    animation->setStartValue(this->geometry());
    animation->setEndValue(closedStateRect);
    this->animated = true;
    animation->start();
    this->closed = true;
}

void Frame::animateOpen()
{
    if(!closed)
        return;

    int duration = 200;

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    connect(animation, SIGNAL(finished()), this, SLOT(finishedAnimating()));

    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve(QEasingCurve::Type::InOutCubic)); //InOutCubic

    animation->setStartValue(this->geometry());
    animation->setEndValue(openStateRect);
    this->animated = true;
    animation->start();
    this->closed = false;
}

void Frame::changeOpenClosedStates()
{
    switch(this->dockedState){

    case 0: // NotDocked
        this->closedStateRect = this->_nameBar->geometry().translated(this->pos());

        if(!this->closed)
            this->openStateRect = QRect(this->pos().x(), this->pos().y(), totalWidth, totalHeight);
        else{
            this->openStateRect = QRect(this->pos().x() - totalWidth + this->width(),this->pos().y() - totalHeight + this->height(), totalWidth, totalHeight);
        }
        qout << openStateRect << " " << closedStateRect << endl;
        break;
    case 1: // DockedTop
        this->closedStateRect = QRect(this->pos().x(), this->pos().y(), this->_nameBar->width(), this->_nameBar->height());
        this->openStateRect   = QRect(this->pos().x(), this->pos().y(), totalWidth, totalHeight);
    default:
        break;
    }
}

void Frame::setDockedState(DockedState state)
{
    this->dockedState = state;
    this->repaint();
}

void Frame::setOpenState(bool openState, bool animated)
{
    if(animated)
        openState ? animateOpen() : animateClose();
    else
        this->setGeometry(openState ? openStateRect : closedStateRect);
}

void Frame::hideAnimation()
{
    if(closed)
        this->animateOpen();
    else
        this->animateClose();
}


void Frame::paintEvent(QPaintEvent *)
{
    QRect selfRect = this->rect().adjusted(0, 0, -1, -1);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;

    switch(dockedState){
    case 1: // DockedTop
        path.addPolygon(QRectF(selfRect.adjusted(0, 0, 0, -30)));
        path.addRoundedRect(selfRect, 15, 15, Qt::AbsoluteSize);
        path.setFillRule(Qt::WindingFill);
        break;

    default:
    case 0: // NotDocked
        path.addRoundedRect(selfRect, 15, 15, Qt::AbsoluteSize);
        break;

    }

    painter.setPen(QPen(Qt::black, 1));
    painter.setOpacity(0.6);
    painter.fillPath(path, Qt::black);
}
void Frame::mousePressEvent(QMouseEvent *event)
{

}

void Frame::moveAction(QMouseEvent *event, QPoint startMovePos)
{
    if(this->animated)
        return;
    if(!this->underMouse())
        return;

    this->setOpenState(true, false);
    QPoint whereTo = event->globalPos() - startMovePos;

    if(whereTo.y() < 15){
        whereTo.ry() = 0;
        this->setDockedState(DockedTop);
    }else
        this->setDockedState(NotDocked);
    if(whereTo.x() < 0)
        whereTo.rx() = 0;
    if(whereTo.x() + this->width() >= 1920)
        whereTo.rx() = 1920 - this->width() + 1;

    this->move(whereTo);
    qout << this->geometry() << endl;

    this->changeOpenClosedStates();
}

Gates::Frame::NameBar::NameBar(Frame *frame) : QWidget(nullptr)
{
    this->frame = frame;

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_PaintOnScreen);
    this->setAttribute(Qt::WA_TranslucentBackground);

    this->setFixedHeight(60);
//    this->setFixedWidth(700);


//    QPushButton * hideButton = new QPushButton("Hide");
//    hideButton->setFixedSize(30, 30);
//    QObject::connect(hideButton, SIGNAL(pressed()), this->frame, SLOT(hideAnimation()));

//    QPushButton * menuButton = new QPushButton("Menu");
//    menuButton->setFixedSize(30, 30);
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

//    layout->addWidget(hideButton, 0, Qt::AlignLeft);
    layout->addWidget(titleLabel);
//    layout->addWidget(menuButton, 0, Qt::AlignRight | Qt::AlignBaseline);

    this->setLayout(layout);
    this->setMask(this->visibleRegion()/* - this->childrenRegion()*/);
}
