#include <iostream>

#include <QApplication>
#include <QIODevice>
#include <QTextStream>

#include "abstractframe.h"
#include "frameshandler.h"
#include "gatesframe.h"
#include "icon.h"

#include <Shlobj.h>

//HRESULT GetSHContextMenu(LPSHELLFOLDER psfFolder, LPCITEMIDLIST localPidl,
//                         void** ppCM, int* pcmType)
//{
//   *ppCM = NULL;
//   LPCONTEXTMENU pICv1 = NULL; // plain version
//   // try to obtain the lowest possible IContextMenu
//   HRESULT hr = psfFolder->GetUIObjectOf(NULL, 1, &localPidl,
//                   IID_IContextMenu, NULL, (void**)&pICv1);
//   if(pICv1) { // try to obtain a higher level pointer, first 3 then 2
//      hr = pICv1->QueryInterface(IID_IContextMenu3, ppCM);
//      if(NOERROR == hr) *pcmType = 3;
//      else {
//         hr = pICv1->QueryInterface(IID_IContextMenu2, ppCM);
//         if(NOERROR == hr) *pcmType = 2;
//      }

//      if(*ppCM) pICv1->Release(); // free initial "v1.0" interface
//      else { // no higher version supported
//         *pcmType = 1;
//         *ppCM = pICv1;
//         hr = NOERROR; // never mind the query failures, this'll do
//      }
//   }

//   return hr;
//}

//void test(){
//#define MIN_SHELL_ID 1
//#define MAX_SHELL_ID 30000
//CMenu menu;
//menu.CreatePopupMenu();
//int cmType; // we don't need this here
//LPCONTEXTMENU pCM;
//LPCITEMIDLIST pidl;
//LPSHELLFOLDER psfFolder;
//// assume that psfFolder and pidl are valid
//HRESULT hr = GetSHContextMenu(psfFolder, pidl, (void**)&pCM, &cmType);
//// fill the menu with the standard shell items
//hr = pCM->QueryContextMenu(menu, 0, MIN_SHELL_ID, MAX_SHELL_ID, CMF_EXPLORE);
//// show the menu and retrieve the selected command ID
//int cmdID = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, cx, cy, this);
//}




#include <QProcess>
#include "contextmenu.h"

using namespace std;
//using namespace Gates;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextStream qout(stdout);

//    BasicFrame test;

//    test.add_icon("C:\\Users\\Nexie\\Desktop\\Discord.lnk");
//    test.add_icon("C:\\Users\\Nexie\\Desktop\\libs");
//    test.add_icon("C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe");
//    test.show();

//    QDir dir("C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe");
//    QStringList filenames = dir.entryList();

//    for(int i = 0; i < filenames.size(); i++)
//        qout << filenames[i] << endl;

//    Gates::AbstractFrame frame(QString("C:\\Users\\Public\\Desktop\\"));
//    frame.show();

//    IconItem ic(QString("C:\\Users\\Nexie\\Desktop\\"));

//    LaunchWithShell(&ic);

//    IconItem t("C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe");
//    qout << t.fileFullPath();


//    QProcess pr;
//    pr.setProgram("C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe");
//    pr.start();

    Gates::Frame frame("C:\\Users\\Public\\Desktop", "DeskTop");
    frame.show();


    return a.exec();
//    return 0;
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
