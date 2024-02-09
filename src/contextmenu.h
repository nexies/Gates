#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QWidget>
#include <QMenu>
#include <QContextMenuEvent>
#include "icon.h"


/*

  Идея: Реализовать выпадающее контекстное меню, вызывающееся для конкретного файла (или группы файлов).
  Аналогичное стандартному из windows.
  Элементы меню нужно брать из реестра

  ContexMenu вызывается для объекта типа Icon или IconGroup.

*/

namespace Gates{

class ContextMenu : public QMenu
{

    Q_OBJECT


public:
    ContextMenu() : QMenu(nullptr) {};
    ContextMenu(IconItem * parent) : QMenu(parent){};

};

}

#endif // CONTEXTMENU_H
