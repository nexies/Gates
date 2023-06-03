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
#include <QDrag>
#include <QMimeData>

//#include <QStyle>
//#include <QAbstractButton>
//#include <QPainter>



#include <cstdlib>
#include "shellqt.h"

namespace Gates{

class IconView;

class IconItem: public QWidget
{

    Q_OBJECT

    enum State
    {
        Idle        = 0b100000,
        Highlighted = 0b000001,
        Selected    = 0b000010,
        Dragged     = 0b000100,
        Remamed     = 0b001000,
        Unactive    = 0b010000
    };

    enum LayoutPolicy
    {
        Box,
        Tile,
        Row,
    };

public:

    IconItem(QWidget * parent = nullptr);
    IconItem(QString p_filePath, QWidget * parent= nullptr);
    IconItem(QFile p_file, QWidget * parent = nullptr);


public:

    QIcon icon;
    QFile file;
    IconView * parentFrame;

    State state = Idle;
    QPoint dragStartPosition;

public:
    bool isValid() const;
    QString fileName() const;
    QString fileNameWithExtention() const;
    QString fullFilePath() const;
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
//    void mouseMoveEvent(QMouseEvent * /*event*/);
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
