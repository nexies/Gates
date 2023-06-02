#ifndef ICONVIEW_H
#define ICONVIEW_H

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

class IconView: public QWidget
{
    Q_OBJECT

    enum IconPlacementPolicy
    {
        FreePlacement,
        GridPlacement,
        ListPlacement,
        TilePlacement
    };

    enum SortingPolicy
    {
        NoSorting,
        SortByName,
        SortBySize,
        SortByNewest,
    };

    bool invertedSort = false;

//    enum AnimationPolicy
//    {
//        NotAnimated  = 0b00000,
//        Animated     = 0b10000,
//        RollingUp    = 0b10001,
//        RollingDown  = 0b10010,
//        RollingLeft  = 0b10011,
//        RollingRight = 0b10100,
//    };

public:

    IconView(QWidget * parent = nullptr);
    IconView(QString p_filename, QWidget * parent = nullptr);
    virtual ~IconView() {}

private:  // file managment

    QList<IconItem *> iconList;
    QGridLayout * layout;
    QDir directory;

public:   // look

    int gridHeight;
    int gridWidth;
//    AnimationPolicy animationPolicy;
    IconPlacementPolicy iconPlacementPolicy;
    SortingPolicy sortingPolicy;

public:

    void placeIcons();

public:

    bool addIcon(IconItem * p_icon);
    bool removeIcon(IconItem * p_icon);

    bool setSortingPolicy(SortingPolicy policy);
    bool setIconPlacementPolicy(IconPlacementPolicy);
};
bool sortIconsByName(const IconItem & left, const IconItem & right);
bool sortIconsByNameInverted(const IconItem & left, const IconItem & right);

bool sortIconsBySize(const IconItem & left, const IconItem & right);
bool sortIconsBySizeInverted(const IconItem & left, const IconItem & right);
}


#endif // ICONVIEW_H
