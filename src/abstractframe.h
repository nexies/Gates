#ifndef ABSTRACTFRAME_H
#define ABSTRACTFRAME_H


#include <QWidget>
#include <QString>
#include <QList>
#include <QDir>
#include <QPalette>
#include <QPropertyAnimation>
#include <QLayout>
#include <QGraphicsEffect>


#include "icon.h"

namespace Gates{

class AbstractFrame: public QWidget
{
    Q_OBJECT

    enum IconPlacementPolicy
    {
        FreePlacement = 0,
        GridPlacement = 1,
        ListPlacement = 2,
    };

    enum SortingPolicy
    {
        NoSorting    = 0,
        SortByName   = 1,
        SortBySize   = 2,
        SortByNewest = 3
    };


    enum AnimationPolicy
    {
        NotAnimated  = 0b00000,
        Animated     = 0b10000,
        RollingUp    = 0b10001,
        RollingDown  = 0b10010,
        RollingLeft  = 0b10011,
        RollingRight = 0b10100,
    };

public:

    AbstractFrame(QWidget * parent = nullptr);
    AbstractFrame(QString p_filename, QWidget * parent = nullptr);
    virtual ~AbstractFrame() {}

private:  // file managment

    QList<IconItem *> iconList;
    QGridLayout * layout;
    QDir directory;

public:   // look

    int gridHeight;
    int gridWidth;
    double opacity;
    AnimationPolicy animationPolicy;
    IconPlacementPolicy iconPlacementPolicy;
    SortingPolicy sortingPolicy;

public:

    void placeIcons();

public:

    bool addIcon(IconItem * p_icon);
    bool removeIcon(IconItem * p_icon);

};

}

#endif // ABSTRACTFRAME_H
