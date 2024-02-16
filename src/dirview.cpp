#include "dirview.h"
#include <QFileSystemModel>
#include <QModelIndex>
#include <QDir>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QPainter>
#include <QScrollBar>

DirView::DirView(QWidget * parent)
{
    this->setParent(parent);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->viewport()->setAutoFillBackground(false);
//    this->setAutoFillBackground(false);
    this->setFrameStyle(0);

    this->setMovement(DirView::Free);
    fileSystemModel = new QFileSystemModel(this);
    setModel(fileSystemModel);
    fileSystemModel->setRootPath(QDir::rootPath());

    this->setDragEnabled(true);
    connect(this, &DirView::activated,
            this, &DirView::onActivated);
    delegate = new DirViewItemDelegate(this);
    this->setItemDelegate(delegate);
    this->viewport()->setAttribute(Qt::WA_Hover);

    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

}

DirView::DirView(const QString &path, QWidget *parent)
    : DirView(parent)
{
    if(!setDirectory(path))
    {
        qDebug() << QStringLiteral("[DirView] : file '%1' is not a directory").arg(path);
    }
}

DirView::DirView(const QFile &file, QWidget *parent)
    : DirView(parent)
{
    if(!setDirectory(file))
    {
        qDebug() << QStringLiteral("[DirView] : file '%1' is not a directory").arg(file.fileName());
    }
}

bool DirView::setDirectory(const QString &path)
{
    QModelIndex index = fileSystemModel->index(path);
    if(!fileSystemModel->isDir(index))
        return false;

    this->setRootIndex(index);
    return true;
}

bool DirView::setDirectory(const QFile &file)
{
    return setDirectory(file.fileName());
}

void DirView::onActivated(const QModelIndex &index)
{
    QString path = fileSystemModel->filePath(index);
    qDebug() << path;
//    qDebug()
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}


