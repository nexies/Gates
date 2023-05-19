#ifndef ICON_H
#define ICON_H

#include <iostream>

#include <QWidget>
//#include <QPushButton>
//#include <QString>
#include <QIcon>
#include <QFile>
//#include <QFileInfo>
//#include <QFileIconProvider>
//#include <QApplication>
//#include <QDrag>
//#include <QMimeData>
#include <QProcess>
#include <QMouseEvent>
//#include <QStyle>
//#include <QAbstractButton>
//#include <QPainter>



#include <cstdlib>
#include "shellqt.h"

namespace Gates{

class AbstractFrame;

class IconItem: public QWidget
{

    Q_OBJECT

    enum State
    {
        Idle        = 0b00000,
        Highlighted = 0b00001,
        Selected    = 0b00010,
        Dragged     = 0b00100,
        Remamed     = 0b01000,
        Unactive    = 0b10000
    };

public:

    IconItem(QWidget * parent = nullptr);
    IconItem(QString p_filePath, QWidget * parent= nullptr);
    IconItem(QFile p_file, QWidget * parent = nullptr);


public:

    QIcon icon;
    QFile file;
    AbstractFrame * parentFrame;

    State state = Idle;
    QPoint dragStartPosition;

public:
    bool isValid();
    QString fileName();
    QString fullFilePath();
    QString fileExtention();

private:
    void getIcon();
    void execute();
    void contextMenu() {std::cout << "Context menu will be launched" << std::endl; }

    void customStyle();
    void paint(State state);
    void repaint();

protected:
    void paintEvent(QPaintEvent * /* event */);

    void mousePressEvent(QMouseEvent * /*event*/);
    void mouseMoveEvent(QMouseEvent * /*event*/);
    void enterEvent(QEvent * /*event*/);
    void leaveEvent(QEvent * /*event*/);

//    void mouseDoubleClickEvent(QMouseEvent * e);

public slots:
    void unselect() {if(state &(Selected|Remamed)) state = State::Idle; }

signals:
    void signal_clicked(IconItem * self);
    void signal_doubleClicked(IconItem * self);
    void signal_rightClicked(IconItem * self);

    void signal_deleted(IconItem * self);

};

}



#endif // ICON_H
