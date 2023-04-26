#include "icon.h"
#include <windows.h>
#include <iostream>


using namespace std;
using namespace Gates;


IconItem::IconItem(QWidget * parent) : QPushButton(parent)
{
    this->setGeometry(800, 800, 100, 70);
    this->setIcon(QIcon("app.ico"));
    this->setIconSize(QSize(50, 50));
    this->setWindowOpacity(1);
    this->setFixedSize(50, 50);
}

IconItem::IconItem(QString p_filePath, QWidget * parent) : QPushButton(parent)
{
    this->file.setFileName(p_filePath);
    this->getFileIcon();
    this->setGeometry(800, 800, 100, 70);
    this->setIcon(this->icon);
    this->setIconSize((QSize(50, 50)));
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
    QFileInfo info(file.fileName());
    QFileIconProvider ip;
    this->icon=ip.icon(info);
}

void IconItem::execute()
{
    Execute(this->file.fileName());
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




#include <Shlobj.h>

void LaunchWithShell(IconItem * icon)
{

//    QString explorer = "explorer.exe ";
//    QStringList arguments;
//    arguments << icon->file.fileName();


//    cout << icon->file.fileName().toStdString() << endl;

//    QProcess * process = new QProcess;
//    process->start(explorer, arguments);
    ShellExecuteW(NULL, L"open", icon->file.fileName().toStdWString().c_str(), NULL,NULL, SW_RESTORE);

}
