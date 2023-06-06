#ifndef FRAME_H
#define FRAME_H

#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include "iconview.h"
#include "icon.h"
#include <QApplication>
#include <QDrag>


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
        void mousePressEvent(QMouseEvent * event){
            if(event->button() == Qt::LeftButton){
                startMovePos = QWidget::mapTo(this->frame, event->pos());

//                std::cout << "Set startDragPos: (" << startMovePos.x() << ", " << startMovePos.y() << ")" << std::endl;

                if(event->type() == QMouseEvent::Type::MouseButtonPress){
                }
                if(event->type() == QMouseEvent::Type::MouseButtonDblClick)
                    this->frame->hideAnimation();
            }else if(event->button() == Qt::RightButton)
            {
                std::cout << "Right button pressed" << std::endl;
            }
            else if(event->button() == Qt::MiddleButton)
            {
                std::cout << "Middle button pressed" << std::endl;
            }

        }

        void mouseMoveEvent(QMouseEvent * event){
            if(this->frame->animated)
                return;
            this->frame->moveAction(event, startMovePos);
        }


    public:
        Gates::Frame * frame;
        QPoint startDragPos;
    };

public:
    Frame () {}
    Frame(QString frameTitle, QWidget * p_parent = nullptr);
    Frame(QString frameTitle, QString p_filename, QWidget * p_parent = nullptr);
    ~Frame() {std::cout << "Frame " << _title.toStdString() << " destroied" << std::endl; }

private:

    QString    _title = "New Frame";
    IconView * _iconView;
    NameBar *  _nameBar;


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

    QString title() {return this->_title; }
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
protected: // events
//    void keyPressEvent(QKeyEvent *event);

    void paintEvent(QPaintEvent * /*event*/);
    void mousePressEvent(QMouseEvent * event);
    void enterEvent(QEvent * /*event*/) {this->underCursor = true;
                                        std::cout << "in under cursor" << std::endl;
                                        if(dockedState != NotDocked) setOpenState(true);}
    void moveAction(QMouseEvent * event, QPoint startMovePos);
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

};

}

#endif // FRAME_H
