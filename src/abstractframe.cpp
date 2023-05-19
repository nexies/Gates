#include "abstractframe.h"

#include <iostream>

Gates::AbstractFrame::AbstractFrame(QWidget * parent): QWidget(parent) {
    this->animationPolicy = NotAnimated;
    this->iconPlacementPolicy = GridPlacement;
    this->sortingPolicy = NoSorting;

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);

//    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AcceptDrops);
    setAttribute(Qt::WA_MouseNoMask);
    setAttribute(Qt::WA_PaintOnScreen);
}

Gates::AbstractFrame::AbstractFrame(QString p_filename,  QWidget * parent): AbstractFrame(parent)
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

