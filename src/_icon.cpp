#include "_icon.h"
#include <windows.h>
#include <iostream>


using namespace std;

Icon::Icon()
{
    this->setGeometry(800, 800, 100, 70);
    this->setIcon(QIcon("app.ico"));
    this->setIconSize(QSize(50, 50));
    //    this->show();
    this->setWindowOpacity(1);
}

void Icon::mousePressEvent(QMouseEvent *e)
{

//    ShellExecute(NULL, L"open", L"D:\\Games\\osulazer\\osu!.exe", NULL, NULL, SW_SHOWDEFAULT);

    ShellExecute(NULL, L"open", L"C:\\Users\\~\\Desktop\\osu! lazer.lnk", NULL, NULL, SW_SHOWDEFAULT);
//    ShellExecute(NULL, L"open", L"D:\\Games\\osulazer\\packages", NULL, NULL, SW_SHOWDEFAULT);

//    cout << "Button pressed" << endl;
}
