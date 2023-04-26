#include "abstractframe.h"

#include <iostream>

Gates::AbstractFrame::AbstractFrame(QWidget * parent): QWidget(parent,  Qt::FramelessWindowHint) {
    this->animationPolicy = NotAnimated;
    this->iconPlacementPolicy = GridPlacement;
    this->sortingPolicy = NoSorting;
}

Gates::AbstractFrame::AbstractFrame(QString p_filename,  QWidget * parent): AbstractFrame(parent)
{
    this->layout = new QGridLayout;
    this->setLayout(this->layout);

    this->directory.setPath(p_filename);
    std::cout << directory.exists() << std::endl;
    QStringList filesList = directory.entryList();

    int count = 0;
    for(int i = 0; i < filesList.size(); i++){
        IconItem * newIcon = new IconItem(directory.path() + "/" + filesList[i]);
        this->iconList.append(newIcon);
        this->layout->addWidget(newIcon, count/10, count % 10);
        count += 1;
    }

}


bool Gates::AbstractFrame::addIcon(IconItem * p_icon)
{
    if(!p_icon->isValid())
        return false;

    this->iconList.append(p_icon);
    this->layout->addWidget(p_icon);
    return true;
}

bool Gates::AbstractFrame::removeIcon(IconItem * p_icon)
{
    this->iconList.removeOne(p_icon);
    this->layout->removeWidget(p_icon);
    return true;
}

