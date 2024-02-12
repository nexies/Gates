#ifndef DIRVIEWITEMDELEGATE_H
#define DIRVIEWITEMDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>


class DirViewItemDelegate : public QStyledItemDelegate
{
  public:
    DirViewItemDelegate(QObject * parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter * painter,
               const QStyleOptionViewItem &option,
               const QModelIndex & index) const;

    QSize sizeHint(const QStyleOptionViewItem & option,
                   const QModelIndex & /*index*/) const;

};

#endif // DIRVIEWITEMDELEGATE_H
