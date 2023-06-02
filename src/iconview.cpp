#include "iconview.h"
#include <QtAlgorithms>

#include <iostream>

Gates::IconView::IconView(QWidget * parent): QWidget(parent) {
    this->iconPlacementPolicy = GridPlacement;
    this->sortingPolicy = NoSorting;

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AcceptDrops);
    setAttribute(Qt::WA_PaintOnScreen);
}

Gates::IconView::IconView(QString p_filename,  QWidget * parent): IconView(parent)
{
    this->layout = new QGridLayout;
    this->setLayout(this->layout);
    this->layout->setHorizontalSpacing(1);
    this->layout->setVerticalSpacing(1);

    this->directory.setPath(p_filename);
    if(!directory.exists()) return;

    QStringList filesList = directory.entryList();

    int count = 0;
    for(int i = 2; i < filesList.size(); i++){
        IconItem * newIcon = new IconItem(directory.path() + "/" + filesList[i]);
        this->iconList.append(newIcon);
        this->layout->addWidget(newIcon, count/6, count % 6);
        count += 1;
    }

}


bool Gates::IconView::addIcon(IconItem * p_icon)
{
    if(!p_icon->isValid())
        return false;

    this->iconList.append(p_icon);
    this->layout->addWidget(p_icon);
    return true;
}

bool Gates::IconView::removeIcon(IconItem * p_icon)
{
    this->iconList.removeOne(p_icon);
    this->layout->removeWidget(p_icon);
    return true;
}

bool Gates::IconView::setSortingPolicy(SortingPolicy policy)
{
    this->sortingPolicy = policy;
    if(policy == SortByName){
        if(this->invertedSort)
            qSort(this->iconList.begin(), this->iconList.end(), sortIconsByNameInverted);
        else
            qSort(this->iconList.begin(), this->iconList.end(), sortIconsByName);
    }
    else if(policy == SortBySize){
        if(this->invertedSort)
            qSort(this->iconList.begin(), this->iconList.end(), sortIconsBySizeInverted);
        else
            qSort(this->iconList.begin(), this->iconList.end(), sortIconsBySizeInverted);
    }
}


bool Gates::sortIconsByName(const IconItem &left, const IconItem &right)
{
    return left.fileNameWithExtention().toLower() < right.fileNameWithExtention().toLower();
}

bool Gates::sortIconsByNameInverted(const IconItem &left, const IconItem &right)
{
    return left.fileNameWithExtention().toLower() > right.fileNameWithExtention().toLower();
}

bool Gates::sortIconsBySize(const IconItem &left, const IconItem &right)
{
    return left.file.size() < right.file.size();
}


bool Gates::sortIconsBySizeInverted(const IconItem &left, const IconItem &right)
{
    return left.file.size() > right.file.size();
}
