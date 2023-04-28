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
    return name.mid(lastSeparator + 1, len);
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
    QPainter painter(this);
    QRect mainRect = this->rect().adjusted(0, 0, -1, -1);
    QRect pixmapRect = mainRect.adjusted(0, 0, -18, -18).translated(9, 3);
//    QRect textRect;
//    QSize size = this->icon.actualSize(pixmapRect.size());

    QPixmap pixmap = this->icon.pixmap(QSize(90, 90)).scaledToHeight(128, Qt::SmoothTransformation);
    QRect pixmapSource =  QRect(QPoint(0, 0), pixmap.size());

    QStyleOptionButton option;

    QRect rect = this->rect().adjusted(0, 0, -1, -1);
    painter.setPen(Qt::blue);
    painter.setOpacity(0.3);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect, Qt::blue);
    painter.setOpacity(1);
    this->icon.paint(&painter, pixmapRect);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform);
//    painter.setOpacity(1);
//    painter.drawPixmap(pixmapRect, pixmap, pixmapSource);
}

void IconItem::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
        this->contextMenu();
    else if(event->type() == QMouseEvent::Type::MouseButtonDblClick)
        this->execute();
    else if(event->type() == QMouseEvent::Type::MouseButtonPress){
        this->state = State::Selected;
        this->dragStartPosition = event->pos();
    }
}

void IconItem::mouseMoveEvent(QMouseEvent *event)
{
    if(!(event->button() & Qt::LeftButton))
        return;
    if((event->pos() - dragStartPosition).manhattanLength() <
            QApplication::startDragDistance())
        return;

//    QDrag *drag = new QDrag(this);
//    QMimeData *mimeData = new QMimeData;

//    mimeData->setData(mimeType, data);
//    drag->setMimeData(mimeData);

//    Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);
}



