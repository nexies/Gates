#ifndef CONTEXMENU_H
#define CONTEXMENU_H

#include <QWidget>
#include "icon.h"


/*

  Идея: Реализовать выпадающее контекстное меню, вызывающееся для конкретного файла (или группы файлов).
  Аналогичное стандартному из windows.
  Элементы меню нужно брать из реестра

  ContexMenu вызывается для объекта типа Icon или IconGroup.

*/

class ContexMenu : public QWidget
{

    Q_OBJECT


public:
    ContexMenu() : QWidget(nullptr) {};

};

#endif // CONTEXMENU_H
