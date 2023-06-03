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





class Frame: public QWidget
{

    Q_OBJECT

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

    public:
        Gates::Frame * frame;
    };

public:
    Frame () {}
    Frame(QString frameTitle, QWidget * p_parent = nullptr);
    Frame(QString frameTitle, QString p_filename, QWidget * p_parent = nullptr);
    ~Frame() {}

private:

    QString    _title = "New Frame";
    IconView * _iconView;
    NameBar *  _nameBar;


public: // looks
    double opacity;
    QColor color;       // something from that
    QPalette palette;   // something from that

public:

    bool visible = true;

    QString title() {return this->_title; }
    void setTitle(QString new_title) {this->_title = new_title; }

private:


public: // for test
    void animateOpen();
    void animateClose();

    QRect globalGeometry;

    void add_icon(QString p_fileName);


protected: // events
//    void keyPressEvent(QKeyEvent *event);

    void paintEvent(QPaintEvent * /*event*/);
    void mousePressEvent(QMouseEvent * event);
//    void mouseDoubleClickEvent(QMouseEvent *event);


public slots:
    void setColor(QColor p_Color) {};
    void setOpacity(double p_Opacity) {};
    void setPalette(QPalette p_Palette) {};

    void hideAnimation();

};

}

#endif // FRAME_H
