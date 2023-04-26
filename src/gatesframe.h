#ifndef GATESFRAME_H
#define GATESFRAME_H

#include <QWidget>
#include "abstractframe.h"
#include "icon.h"


namespace Gates{

class Frame : public AbstractFrame
{

    Q_OBJECT

public:
    Frame(QString frameTitle = "New Frame", QWidget * p_parent = nullptr);
    Frame(QString p_filename, QString frameTitle = "NewFrame", QWidget * p_parent = nullptr);
    ~Frame() {}

public:

    QString frameTitle = "New Frame";

private:
    void __init();

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
