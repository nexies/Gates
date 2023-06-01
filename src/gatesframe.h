#ifndef GATESFRAME_H
#define GATESFRAME_H

#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include "abstractframe.h"
#include "icon.h"


namespace Gates{

class Frame;


class NameBar: public QWidget
{

    Q_OBJECT

public:
    NameBar(QString title, Frame * frame = nullptr);

protected:

    void paintEvent(QPaintEvent * /*event*/){
        QPainter painter(this);
        painter.setOpacity(0.5);
        QRect selfRect = this->rect().adjusted(0, 0, -1, -1);
        painter.fillRect(selfRect, Qt::darkBlue);

    }

public:
    QString title;
    Gates::Frame * frame;

public slots:
    void menuButtonPressed() { std::cout << this->title.toStdString() << ":  Menu button pressed" << std::endl;};
    void hideButtonPressed() { std::cout << this->title.toStdString() << ":  Hide button pressed" << std::endl;};
};


class Frame : public AbstractFrame
{

    Q_OBJECT


public:
    Frame(QString frameTitle = "New Frame", QWidget * p_parent = nullptr);
    Frame(QString p_filename, QString frameTitle = "NewFrame", QWidget * p_parent = nullptr);
    ~Frame() {}

public:

    QString frameTitle = "New Frame";


public:
    void setColor(QColor p_Color);
    void setOpacity(double p_Opacity);

    void disappear();
    void appear();

    bool visible = true;


public: // for test
    void animateOpen();
    void animateClose();
    void animateCycle();
    void add_icon(QString p_fileName);

//    void keyPressEvent(QKeyEvent *event);

    void mousePressEvent(QMouseEvent * event);
    //    void mouseDoubleClickEvent(QMouseEvent *event);

public slots:
//    void slot_CloseGatesFrame();
//    void slot_SortIconsBy();
//    void slot_CloseFrame() {
//        this->close();
//    }

};

}

#endif // GATESFRAME_H
