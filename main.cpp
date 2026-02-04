#include <QApplication>
#include <QDebug>
#include <QDir>

#include <QStandardPaths>
#include "cpp/Foreign/GatesFrameForeign.h"
#include <QQuickWindow>

#include <QDebug>
#include "cpp/utilities/win/Functions.h"

#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    GatesFrameForeign fr;
    GatesFrameForeign fr2;
    GatesFrameForeign fr3;
    qDebug() << QDir::homePath();
    // fr.setDirectory(QDir::homePath() + QDir::separator() + "OneDrive" + QDir::separator() + "Рабочий стол");
    fr.setDirectory("C:\\Users\\nexie\\Downloads");
    fr2.setDirectory("C:\\Users\\nexie\\Downloads");
    fr3.setDirectory("C:\\Users\\nexie\\Downloads");
    //fr.setDirectory(QDir::homePath());
    //fr.setDirectory(QDir::homePath());
    //"C:\Users\green\OneDrive\Рабочий стол"
    fr.setColor(Qt::red);
    fr.setOpacity(0.6);
    fr.show();
    fr2.show();
    fr3.show();

    //qDebug() << Q_FUNC_INFO;


    return a.exec();

    //return 0;
}

// #endif
