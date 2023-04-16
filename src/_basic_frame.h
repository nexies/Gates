#ifndef _BASIC_FRAME_H
#define _BASIC_FRAME_H


#include <QWidget>
#include <QString>
#include <QList>
#include <QPalette>
#include <QPropertyAnimation>
#include <QLayout>


#include "_icon.h"

class BasicFrame : public QWidget
{
    
    Q_OBJECT

    enum IconsSorting{
        Default = 0,
        Name = 0,
        Size = 1,
            
    };

// constructors
public: 
    BasicFrame(QWidget * p_parent = nullptr);
    BasicFrame(QString p_FilePath, QWidget * p_parent = nullptr);

// fields
public: 
    QString FrameTitle = "New Frame";
    QString FilePath;
    double Opacity;
    QColor Color;
    QPalette Palette = QPalette();
    QGridLayout * Layout;
    bool minimised = false;


// methods
private:
    void __init();

public: 
    void setColor(QColor p_Color);
    void setOpacity(double p_Opacity);


public: // for test
    void animateOpen();
    void animateClose();
    void animateCycle();
    void add_icon();

//    void keyPressEvent(QKeyEvent *event);

    void mousePressEvent(QMouseEvent * event);
    //    void mouseDoubleClickEvent(QMouseEvent *event);

public slots:
//    void slot_CloseGatesFrame();
//    void slot_SortIconsBy();
    void slot_CloseFrame() {
        this->close();
    }
      
};


#endif // _BASIC_FRAME_H
