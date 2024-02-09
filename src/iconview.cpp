#include "iconview.h"
#include <QtAlgorithms>
#include <QSizePolicy>

#include <iostream>

Gates::IconView::IconView(QWidget * parent): QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AcceptDrops);
//    setAttribute(Qt::WA_PaintOnScreen);
}

Gates::IconView::IconView(QString p_filename,  QWidget * parent): IconView(parent)
{

    this->directory.setPath(p_filename);
    if(!directory.exists()) return;

    filesList = directory.entryList().mid(2);

    for(int i = 0; i < filesList.size(); i++){
        IconItem * newIcon = new IconItem(directory.path() + "/" + filesList[i]);
        this->iconList.append(newIcon);
    }

    this->setSortingPolicy(SortByName);
//    this->setIconPlacementPolicy(GridPlacement); // DOES NOTHING YET!
    this->iconPlacementPolicy = GridPlacement;
    this->placeIcons();

}

#include <QPainter>
void Gates::IconView::paintEvent(QPaintEvent *)
{
//    QRect selfRect = this->rect().adjusted(0, 0, -1, -1);
//    QPainter painter(this);
//    painter.setOpacity(0.3);
//    painter.fillRect(selfRect, Qt::green);

}

void Gates::IconView::placeIcons()
{
    if(this->iconPlacementPolicy == GridPlacement){
        QGridLayout * gridLayout = new QGridLayout;
        gridLayout->setHorizontalSpacing(1);
        gridLayout->setVerticalSpacing(1);
        int colums = this->width()/15; // @todo: divide by the width of an IconItem. Is a static value

        int c = 0, r = 0;
        for(int icon = 0; icon < iconList.size(); icon++){
            c = icon / colums;
            r = icon % colums;

            gridLayout->addWidget(iconList[icon], c, r, Qt::AlignTop | Qt::AlignRight);
        }
//        QSpacerItem * spacer = new QSpacerItem(iconList[0]->width(), iconList[0]->height());
//        for(int i = r; i < colums; i++)
//            gridLayout->addItem(spacer, i, c);

        if(this->layout() != nullptr) delete this->layout();
        this->setLayout(gridLayout);
//        this->setSizePolicy(QSizePolicy(QSizePolicy::Ignored));
        return;

    }
}


bool Gates::IconView::addIcon(IconItem * p_icon)
{
    if(!p_icon->isValid())
        return false;

    this->iconList.append(p_icon);
    this->layout()->addWidget(p_icon);
    return true;
}

bool Gates::IconView::removeIcon(IconItem * p_icon)
{
    this->iconList.removeOne(p_icon);
    this->layout()->removeWidget(p_icon);
    return true;
}

bool Gates::IconView::setSortingPolicy(SortingPolicy policy)
{
    this->sortingPolicy = policy;
    if(policy == SortByName)
        qSort(this->iconList.begin(), this->iconList.end(), sortIconsByName);
    else if(policy == SortBySize)
        qSort(this->iconList.begin(), this->iconList.end(), sortIconsBySize);
    else if(policy == NoSorting){

    }

    if(invertedSort) iconList = reversed(iconList);

    return false;
}


bool Gates::sortIconsByName(const IconItem &left, const IconItem &right)
{
    return left.fileNameWithExtention().toLower() < right.fileNameWithExtention().toLower();
}

bool Gates::sortIconsBySize(const IconItem &left, const IconItem &right)
{
    return left.file.size() < right.file.size();
}

template <typename T>
QList<T> reversed( const QList<T> & in ) {
    QList<T> result;
    result.reserve( in.size() ); // reserve is new in Qt 4.7
    std::reverse_copy( in.begin(), in.end(), std::back_inserter( result ) );
    return result;
}
