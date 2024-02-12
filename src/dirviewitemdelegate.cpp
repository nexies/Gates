#include "dirviewitemdelegate.h"

#include <QPainter>
#include <QLinearGradient>
#include <QPainterPath>

void DirViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = option.rect.adjusted(0, 0, -1, -1);
    QRect iconRect = QRect(0, 0, 25, 25);
    QRect nameRect = rect.adjusted(30, 3, -60, 0);

    painter->setPen(Qt::white);

    if(option.state & QStyle::State_HasFocus)
    {
        QPainterPath path;
        path.addRoundedRect(rect, 3, 3, Qt::AbsoluteSize);
        painter->setOpacity(0.6);
        painter->fillPath(path, Qt::white);
    }
    else if(option.state & QStyle::State_MouseOver)
    {
        QPainterPath path;
        path.addRoundedRect(rect, 3, 3, Qt::AbsoluteSize);
        painter->setOpacity(0.4);
        painter->fillPath(path, Qt::white);
    }


    painter->setOpacity(1);
    painter->setPen(Qt::white);
    painter->drawText(nameRect, "text text");

//    return QStyledItemDelegate::paint(painter, option, index);
}

QSize DirViewItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const
{
    return QSize(option.rect.width(), 25);
}
