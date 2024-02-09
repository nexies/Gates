#ifndef FRAMENAMEBAR_H
#define FRAMENAMEBAR_H

#include <QWidget>

namespace Gates{

class Frame;

class NameBar: public QWidget
{

public:
    NameBar(Frame * frame);

protected:

    void paintEvent(QPaintEvent * /*event*/){
//            QPainter painter(this);
//            painter.setOpacity(0.5);
//            QRect selfRect = this->rect().adjusted(0, 0, -1, -1);
//            painter.fillRect(selfRect, Qt::red);
    }

    QPoint startMovePos;
    void mousePressEvent(QMouseEvent * event);

    void mouseMoveEvent(QMouseEvent * event);


public:
    Gates::Frame * frame;
    QPoint startDragPos;
};


}
#endif // FRAMENAMEBAR_H
