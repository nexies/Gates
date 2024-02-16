#ifndef FRAME_H
#define FRAME_H

#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QApplication>
#include <QDrag>

#include "namebar.h"
#include "iconview.h"
#include "icon.h"
#include "dirview.h"
#include "resizable.h"

namespace Gates{




class Frame: public QWidget
{

    Q_OBJECT

    enum DockedState
    {
        NotDocked = 0,
        DockedTop = 1,
        DockedBottom = 2
    };



public:
    Frame (QWidget * parent);
    Frame(QString frameTitle, QWidget * p_parent = nullptr);
    Frame(QString frameTitle, QString p_filename, QWidget * p_parent = nullptr);
    ~Frame() {std::cout << "Frame " << _title.toStdString() << " destroied" << std::endl; }

private:

    QString    _title = "New Frame";
    IconView * _iconView;
    NameBar *  _nameBar;
    DirView * _dirView;
    QVBoxLayout * layout;
    Resizable * res;

public: // looks
    double opacity;
    QColor color;       // something from that
    QPalette palette;   // something from that

    QRect openStateRect;
    QRect closedStateRect;

public: // instantaneous properties
    bool visible = true;
    bool animated = false;
    bool underCursor = false;
    bool closed = false;
    int dockedState = NotDocked;
    int totalHeight;
    int totalWidth;

    const QString title() {return this->_title; }
    void setTitle(QString new_title) {this->_title = new_title; }



public: // for test
    void animateOpen();
    void animateClose();
    void changeOpenClosedStates();
    void setDockedState(DockedState state);
    void setOpenState(bool openState, bool animated = true);

    void add_icon(QString p_fileName);

    void setGeometry(int ax, int ay, int aw, int ah){
        this->setGeometry(QRect(ax, ay, aw, ah));
    }
    void setGeometry(QRect rect){
        this->QWidget::setGeometry(rect);
        this->totalHeight = rect.height();
        this->totalWidth = rect.width();
        this->changeOpenClosedStates();
    }

    void moveAction(QMouseEvent * event, QPoint startMovePos);
protected: // events
//    void keyPressEvent(QKeyEvent *event);

    void paintEvent(QPaintEvent * /*event*/);
    void mousePressEvent(QMouseEvent * event);
    void enterEvent(QEvent * /*event*/) {this->underCursor = true;
                                        std::cout << "is under cursor" << std::endl;
                                        if(dockedState != NotDocked) setOpenState(true);}
    void leaveEvent(QEvent * /*event*/) {this->underCursor = false;
                                        std::cout << "is not under cursor" << std::endl;
                                        if(dockedState != NotDocked) setOpenState(false);}
//    void mouseReleaseEvent(QMouseEvent * event) {changeOpenClosedStates(); }
//    void mouseDoubleClickEvent(QMouseEvent *event);


public slots:
    void setColor(QColor p_Color) {};
    void setOpacity(double p_Opacity) {};
    void setPalette(QPalette p_Palette) {};

    void finishedAnimating () {animated = false;}
    void hideAnimation();

    void onGeometryUpdated();

};

}

#endif // FRAME_H
