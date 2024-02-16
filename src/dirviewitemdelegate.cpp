#include "dirviewitemdelegate.h"

#include <QPainter>
#include <QLinearGradient>
#include <QPainterPath>
#include <QDebug>
#include <QFileSystemModel>

void DirViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = option.rect.adjusted(0, 0, -1, -1);
    QRect iconRect = QRect(option.rect.topLeft().x(), option.rect.topLeft().y(), 25, 25).adjusted(2, 2, -2, -2);
    QRect nameRect = rect.adjusted(30, 3, -60, 0);

    const QFileSystemModel * model = static_cast<const QFileSystemModel *>(index.model());
    if(model == nullptr)
        return QStyledItemDelegate::paint(painter, option, index);

    QString filename = model->fileName(index);
    QIcon   icon     = model->fileIcon(index);
    QFileInfo info   = model->fileInfo(index);

    painter->setPen(Qt::white);
    painter->setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect, 5, 5, Qt::AbsoluteSize);

    if(option.state & QStyle::State_HasFocus)
    {
        painter->setOpacity(0.6);
        painter->fillPath(path, Qt::white);
    }
    else if(option.state & QStyle::State_MouseOver)
    {
        painter->setOpacity(0.4);
        painter->fillPath(path, Qt::white);
    }

    painter->setOpacity(1);

    painter->drawPixmap(iconRect, icon.pixmap(iconRect.size()));

    painter->setPen(Qt::white);
    painter->drawText(nameRect, filename);

//    qDebug() << Q_FUNC_INFO << ": painter called";

//    return QStyledItemDelegate::paint(painter, option, index);
}

QSize DirViewItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const
{
    return QSize(option.rect.width(), 25);
}

void DirViewItemDelegate::setSelectionOpacity(double value)
{
    if(value <= 0)
        _selectionOpacity = 0;
    else if(value >= 1)
        _selectionOpacity = 1;
    else
        _selectionOpacity = value;
}

double DirViewItemDelegate::selectionOpacity() const
{
    return _selectionOpacity;
}
