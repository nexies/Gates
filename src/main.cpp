#include <iostream>


#include <QApplication>


#include "mainwindow.h"
#include "dragabble_widget.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    DragabbleWidget wid;

//    w.show();
    wid.show();
    return a.exec();
}
