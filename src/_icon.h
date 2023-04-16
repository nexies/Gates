#ifndef _ICON_H
#define _ICON_H

#include <QWidget>
#include <QPushButton>
#include <QString>
#include <QIcon>

#include <cstdlib>


class Icon: public QPushButton
{

    Q_OBJECT

public:
    Icon();

public:

    QString Filepath;

    void mousePressEvent(QMouseEvent *e);

//this->keyPressEvent(QKeyEvent *) override()
//    QKeyEvent a;

};

#endif // _ICON_H
