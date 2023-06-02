#ifndef FRAME_H
#define FRAME_H

#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include "iconview.h"
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


class Frame: public QWidget
{

    Q_OBJECT


public:
    Frame(QString frameTitle = "New Frame", QWidget * p_parent = nullptr);
    Frame(QString p_filename, QString frameTitle = "NewFrame", QWidget * p_parent = nullptr);
    ~Frame() {}

private:
    QString frameTitle = "New Frame";
    IconView * iconView;
    NameBar * nameBar;


public: // looks
    double opacity;
    QColor color;       // something from that
    QPalette palette;   // something from that

public:

//    void disappear();
//    void appear();

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
    void setColor(QColor p_Color) {};
    void setOpacity(double p_Opacity);
    void setPalette(QPalette p_Palette);

};

}

#endif // FRAME_H
