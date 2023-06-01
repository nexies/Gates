#ifndef ICONVIEW_H
#define ICONVIEW_H


//===============//
#include <QList>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
//===============//
#include "icon.h"
#include "abstractframe.h"




namespace Gates
{

class IconView : public QWidget
{

    Q_OBJECT


    enum IconPlacementPolicy
    {
        ListPlacement,
        RowsListPlacement,
        TilesPlacement,
        GridPlacement,
        FreeGridPlacement
    };


    enum SortingPolicy
    {
        NoSorting,
        SortByName,
        SortBySize,
        SortByNewest,
        SortByFrequent
    };

    bool sortingForward = true;

public:
    IconView(AbstractFrame * parent);

private:
    QLayout * _layout;
    SortingPolicy _sortingPolicy;
    IconPlacementPolicy _iconPlacementPolicy;

public:
    void setSortingPolicy(SortingPolicy policy, bool forward = true);
    void setSortingForward(bool forward);
    void setIconPlacementPolicy(IconPlacementPolicy policy);

    SortingPolicy sortingPolicy() {return _sortingPolicy;}
    IconPlacementPolicy iconPlacementPolicy() {return _iconPlacementPolicy; }

};

};
#endif // ICONVIEW_H
