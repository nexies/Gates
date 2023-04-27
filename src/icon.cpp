#include "icon.h"
#include <windows.h>
#include <iostream>


using namespace std;
using namespace Gates;


IconItem::IconItem(QWidget * parent) : QWidget(parent/*Qt::FramelessWindowHint*/)
{
//    this->setGeometry(800, 800, 100, 70);
//    this->setIcon(QIcon("app.ico"));
//    this->setIconSize(QSize(50, 50));
    this->setWindowOpacity(1);
    this->setFixedSize(80, 80);
    this->paintEngine();
//    this->setWindowOpacity(0.1);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

//    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
//    setAttribute(Qt::WA_PaintOnScreen);
}

IconItem::IconItem(QString p_filePath, QWidget * parent) : IconItem(parent)
{
    this->file.setFileName(p_filePath);
    this->getFileIcon();
//    this->setGeometry(800, 800, 100, 70);
//    this->setIcon(this->icon);
//    this->setIconSize((QSize(50, 50)));
//    this->setText(fileName());
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
    int i = name.lastIndexOf("/");
    return name.mid(i+1);
}

QString IconItem::fileFullPath()
{
    return this->file.fileName();
}

void IconItem::getFileIcon()
{
//    QFileInfo info(file.fileName());
//    QFileIconProvider ip;
//    this->icon=ip.icon(info);
//    QList<QSize> sizes = icon.availableSizes();
    this->icon = ExtractIcons(this->file.fileName());
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
    QRect pixmapRect = mainRect.adjusted(-6, -6, -6, -6).translated(3, 3);
//    QRect textRect;
//    QSize size = this->icon.actualSize(pixmapRect.size());

    QPixmap pixmap = this->icon.pixmap(QSize(32,32)).scaledToHeight(128, Qt::SmoothTransformation);

    QStyleOptionButton option;
//    option.initFrom(this);
//    option.icon = this->icon;

    QRect rect = this->rect().adjusted(0, 0, -1, -1);
    painter.setPen(Qt::blue);
    painter.setOpacity(0.3);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect, Qt::blue);
    painter.setOpacity(1);
    painter.drawPixmap(pixmapRect, pixmap, QRect(QPoint(0, 0), pixmap.size()));

//    style()->drawControl(QStyle::CE_CheckBox, &option, &painter,
//                         this);
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



