#include <iostream>

#include <QApplication>

#include "_basic_frame.h"
#include "frameshandler.h"
#include "_icon.h"

#include <Shlobj.h>


HRESULT GetSHContextMenu(LPSHELLFOLDER psfFolder, LPCITEMIDLIST localPidl,
                         void** ppCM, int* pcmType)
{
   *ppCM = NULL;
   LPCONTEXTMENU pICv1 = NULL; // plain version
   // try to obtain the lowest possible IContextMenu
   HRESULT hr = psfFolder->GetUIObjectOf(NULL, 1, &localPidl,
                   IID_IContextMenu, NULL, (void**)&pICv1);
   if(pICv1) { // try to obtain a higher level pointer, first 3 then 2
      hr = pICv1->QueryInterface(IID_IContextMenu3, ppCM);
      if(NOERROR == hr) *pcmType = 3;
      else {
         hr = pICv1->QueryInterface(IID_IContextMenu2, ppCM);
         if(NOERROR == hr) *pcmType = 2;
      }

      if(*ppCM) pICv1->Release(); // free initial "v1.0" interface
      else { // no higher version supported
         *pcmType = 1;
         *ppCM = pICv1;
         hr = NOERROR; // never mind the query failures, this'll do
      }
   }

   return hr;
}

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    return a.exec();
}


/*
 * Размышления:
 *      1) Стандартная библеотека для отрисовки графического интерфейса: MFC. Отображение иконок, меню.
 *          Использовать бессмысленно, разве что для доступа к пунктам меню и иконкам приложений и ярлыков. (медленне, чем Qt, несовместима с Qt)
 *      2) Программа пишется для Windows, с использованием таких понятий как WinsowsPowerShell, реестр. Кросс-платформинг не подрузамевается, поэтому грешим, джентельмены
 *
 * План:
 *      1) Класс ContexMenu. Доступ к элементам стандартного контекстного меню (реестр), вызов нужных приложений.
 *          Пункты copy, paste, detele и тд - воплощать вручную
 *      2) Класс Icon. Отображение иконки на экране. Взаимодействие с пользователем:
 *              - Запуск приложения (открытие файла в нужной программе). Можно использовать ShellExecute.
 *              - Вызов контекстного меню.
 *              - Выделение, перетаскивание, удаление
 *      3) Класс GatesFrame. Взаимодействие с пользователем:
 *              - Изменение геометрии окна
 *              - Привязка элементов Icon к окну
 *              - Анимации окна
 *      4) Класс GatesMainFrame. Может быть предком GatesFrame. Прозрачное окно, расстянутое на весь экран. Выполняет функцию основного рабочего стола.
 *
 *
 * Идеи:
 *      - Нужен способ контролировать все отображаемые GatesFrame. То есть нужен класс GatesFramesHandler, который бы контролировал все окна (изменял настройки).
 *      - Для возможности выделения нескольких иконок нужно реализовать IconGroup - объект, содержащий указатели на несколько объектов Icon и действия с ними.
 *
*/
