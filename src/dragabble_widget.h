#ifndef DRAGABBLE_WIDGET_H
#define DRAGABBLE_WIDGET_H

#include <QWidget>

class DragabbleWidget : public QWidget
{

    Q_OBJECT

public:
    DragabbleWidget(QWidget * parent = nullptr) : QWidget(parent, Qt::Window | Qt::FramelessWindowHint) {
        setWindowOpacity(0.5);
    }
};

#endif // DRAGABBLE_WIDGET_H
