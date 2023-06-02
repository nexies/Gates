#include "icon.h"
#include <windows.h>
#include <iostream>


using namespace std;
using namespace Gates;


IconItem::IconItem(QWidget * parent) : QWidget(parent)
{
    this->setFixedSize(95, 85);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

//    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_PaintOnScreen);
}

IconItem::IconItem(QString p_filePath, QWidget * parent) : IconItem(parent)
{
    this->file.setFileName(p_filePath);
    this->getIcon();
}

bool IconItem::isValid()
{
    if(!this->file.exists())
        return false;
    return true;
}

QString IconItem::fileName()
{
    QString name = this->file.fileName();
    int lastSeparator = name.lastIndexOf("/");
    int len = name.lastIndexOf(".") - lastSeparator;
    return name.mid(lastSeparator + 1, len-1);
}

QString IconItem::fileNameWithExtention()
{
    QString name = this->file.fileName();
    int lastSeparator = name.lastIndexOf("/");
    return name.mid(lastSeparator + 1);
}

QString IconItem::fullFilePath()
{
    return this->file.fileName();
}

void IconItem::getIcon()
{
    this->icon = extractIcons(this->fullFilePath());
}

void IconItem::execute()
{
    Execute(this->file.fileName());
}


#include <QStyleOptionButton>
#include <QPainter>
#include <QPainterPath>

void IconItem::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);// = new QPainter(this);

    QRect mainRect = this->rect().adjusted(0, 0, -1, -1);
//    QSize mainSize = mainRect.size();
    QSize iconSize = this->icon.actualSize(this->size() / 10 * 7);

    QRect iconRect(mainRect.left()+(mainRect.width()/2)-(iconSize.width()/2),
                   mainRect.top()+4, iconSize.width(), iconSize.height());
    QRect textRect(mainRect.left()+4, mainRect.top()+iconSize.height()+4,
                  mainRect.width()-8, mainRect.height()-iconSize.height()-4);
//    QBrush txtBrush;

    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    QColor highlightColor;

    QBrush highlightBrush(Qt::white);
    painter.setPen(highlightColor);
    QPainterPath path;
    path.addRoundRect(mainRect, 15,15);


    if(state & (Idle | Dragged))
        painter.setOpacity(0);
    else if(state & Highlighted)
        painter.setOpacity(0.2);
    else if(state & Selected)
        painter.setOpacity(0.4);

    painter.fillPath(path, highlightBrush);


    if(state == Dragged)
        painter.setOpacity(0.5);
    else
        painter.setOpacity(1.0);

    this->icon.paint(&painter, iconRect);

    painter.setPen(Qt::black);
    painter.drawText(textRect.adjusted(0, 1, 1, 0),
                      Qt::AlignTop|Qt::AlignHCenter|Qt::TextWordWrap,
                      this->fileName());
    painter.setPen(Qt::white);
    painter.drawText(textRect,
                      Qt::AlignTop|Qt::AlignHCenter|Qt::TextWordWrap,
                     this->fileName());
    this->setMask(this->rect());
}

void IconItem::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        this->contextMenu();
        emit signal_rightClicked(this);
    }
    else if(event->type() == QMouseEvent::Type::MouseButtonDblClick){
        this->state = Highlighted;
        this->execute();
        emit signal_doubleClicked(this);
    }
    else if(event->type() == QMouseEvent::Type::MouseButtonPress){
        this->state = State::Selected;
        emit signal_clicked(this);
        this->dragStartPosition = event->pos();
        this->mouseMoveEvent(event);
    }
    update();
}

void IconItem::mouseMoveEvent(QMouseEvent *event)
{
    if(!(event->button() & Qt::LeftButton))
        return;

    QDrag * drag = new QDrag(this);
    QMimeData * mimeData = new QMimeData();

    QPixmap pixmap(this->size());
    this->state = Dragged;
    this->update();
    this->render(&pixmap, QPoint(), QRegion(this->rect()));



    mimeData->setText(this->fullFilePath());
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);

    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);

    QPoint endSpot = QCursor::pos();
    this->setGeometry(endSpot.x(), endSpot.y(), this->height(), this->width());

    this->state = Idle;

//    if((event->pos() - dragStartPosition).manhattanLength() <
//            QApplication::startDragDistance())
//        return;
//    if(this)
//    if(event->pos())


//    QDrag *drag = new QDrag(this);
//    QMimeData *mimeData = new QMimeData;

//    mimeData->setData(mimeType, data);
//    drag->setMimeData(mimeData);

        //    Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);
}

void IconItem::enterEvent(QEvent *)
{
    if(this->state == Idle) this->state = Highlighted;
    this->update();
}

void IconItem::leaveEvent(QEvent *)
{
    if(this->state == Highlighted) this->state = Idle;
    this->update();
}



