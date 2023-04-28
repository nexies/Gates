#ifndef ICON_H
#define ICON_H

#include <iostream>

#include <QWidget>
#include <QPushButton>
#include <QString>
#include <QIcon>
#include <QFile>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QProcess>
#include <QStyle>
#include <QMouseEvent>
#include <QAbstractButton>



#include <cstdlib>
#include "shellqt.h"

namespace Gates{

class AbstractFrame;




class IconItem: public QWidget
{

    Q_OBJECT

    enum State
    {
        Idle = 0,
        Selected = 1,
        UnderHoveringMouse = 2,
        Dragged = 3,
        Remamed = 4
    };

public:

    IconItem(QWidget * parent = nullptr);
    IconItem(QString p_filePath, QWidget * parent= nullptr);
    IconItem(QFile p_file, QWidget * parent = nullptr);

public:

    QIcon icon;
    QFile file;
    QProcess process;
    AbstractFrame * parentFrame;

    State state;
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
public:
    void paintEvent(QPaintEvent * /* event */);

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
//    void mouseDoubleClickEvent(QMouseEvent * e);

public slots:
    void unselect() {if(this->state == State::Selected) state = State::Idle; }


};

}



#endif // ICON_H
