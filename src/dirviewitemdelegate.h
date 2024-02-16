#ifndef DIRVIEWITEMDELEGATE_H
#define DIRVIEWITEMDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>


class DirViewItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    Q_PROPERTY(double selectionOpacity READ selectionOpacity WRITE setSelectionOpacity NOTIFY selectionOpacityChanged)

public:
    DirViewItemDelegate(QObject * parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter * painter,
               const QStyleOptionViewItem &option,
               const QModelIndex & index) const;

    QSize sizeHint(const QStyleOptionViewItem & option,
                   const QModelIndex & /*index*/) const;

private:
    double _selectionOpacity = 0;

protected:
    void setSelectionOpacity(double value);
public:
    double selectionOpacity() const;
signals:
    void selectionOpacityChanged(double value);

public slots:
};

#endif // DIRVIEWITEMDELEGATE_H
