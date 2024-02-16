#include "namebar.h"
#include "frame.h"


void Gates::NameBar::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        startMovePos = QWidget::mapTo(this->frame, event->pos());

        //                std::cout << "Set startDragPos: (" << startMovePos.x() << ", " << startMovePos.y() << ")" << std::endl;

        if(event->type() == QMouseEvent::Type::MouseButtonPress){
        }
        if(event->type() == QMouseEvent::Type::MouseButtonDblClick)
            this->frame->hideAnimation();
    }else if(event->button() == Qt::RightButton)
    {
        std::cout << "Right button pressed" << std::endl;
    }
    else if(event->button() == Qt::MiddleButton)
    {
        std::cout << "Middle button pressed" << std::endl;
    }

}

void Gates::NameBar::mouseMoveEvent(QMouseEvent *event){
    if(this->frame->animated)
        return;
    this->frame->moveAction(event, startMovePos);
}

void Gates::NameBar::setTitle(const QString &str)
{
    this->titleLabel->setText(str);
}

Gates::NameBar::NameBar(Frame *frame) : QWidget(nullptr)
{
    this->frame = frame;

    this->setFixedHeight(40);

    titleLabel = new QLabel();
    titleLabel->setFont(QFont("Helvetica", 12));
    setTitle(frame->title());
    QPalette sample_palette;
    sample_palette.setColor(QPalette::WindowText, Qt::white);
    titleLabel->setPalette(sample_palette);
    QSizePolicy titleSizePolicy;
    titleSizePolicy.setHorizontalPolicy(QSizePolicy::Maximum);
    titleLabel->setSizePolicy(titleSizePolicy);

    QHBoxLayout * layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(titleLabel);

    this->setLayout(layout);
    this->setMask(this->visibleRegion());
}
