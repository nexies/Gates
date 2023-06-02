#include <iostream>

#include <QApplication>
#include <QIODevice>
#include <QTextStream>

#include "iconview.h"
#include "frameshandler.h"
#include "frame.h"
#include "icon.h"
#include <QStyle>

#include <QProcess>
#include "contextmenu.h"
#include <QStyleOptionViewItem>
#include <QPainter>
#include <QPaintEngine>
#include <QPushButton>
#include <QLabel>

using namespace std;
//using namespace Gates;



//"C:\\Users\\Public\\Desktop\\TeamViewer.lnk"
//"C:\\Program Files\\TeamViewer\\TeamViewer.exe"


int main(int argc, char *argv[])
{

    QApplication a( argc, argv );

//    Gates::IconItem icon("C:\\Users\\Public\\Desktop\\TeamViewer.lnk");
//    icon.show();

//    Gates::Frame frame("C:\\Users\\Public\\Desktop", "new frame");
//    frame.show();
//    frame.animateCycle();
//    Gates::FramesHandler handler;

//    handler.collectAllDesktopFiles();

//    Gates::IconItem discord("C:\\Users\\green\\Desktop\\Discord.lnk");
//    discord.show();

    Gates::NameBar test("test name bar");
    test.show();

    // test comment to check if commit is working

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
