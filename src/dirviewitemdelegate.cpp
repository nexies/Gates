#include "dirviewitemdelegate.h"

#include <QPainter>
#include <QLinearGradient>

void DirViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(option.state & QStyle::State_MouseOver){
        QRect rect = option.rect;
        QLinearGradient gradient(0, 0, rect.width(), rect.height());
        gradient.setColorAt(0, Qt::white);
        gradient.setColorAt(0.5, Qt::blue);
        gradient.setColorAt(1, Qt::green);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(gradient);
        painter->drawEllipse(option.rect);
    }
//    return QStyledItemDelegate::paint(painter, option, index);
}

QSize DirViewItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const
{
    return QSize(option.rect.width(), 55);
}
