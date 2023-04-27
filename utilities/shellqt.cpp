#include "shellqt.h"
#include <iostream>

using namespace std;

void Execute(QString file, QStringList params)
{
    ShellExecuteW(NULL, L"open", file.toStdWString().c_str(), NULL, NULL, SW_RESTORE);
//    ShellExecuteW(NULL, L"open", icon->file.fileName().toStdWString().c_str(), NULL,NULL, SW_RESTORE);

}

QIcon ExtractIcons(QString filepath)
{
    HICON smallIcon, bigIcon;
    int numChars = filepath.size();

    WCHAR * path = new WCHAR[numChars + 1];
    filepath.toWCharArray(path);

    // ^^^^^ there is probably some actual size limit you can use
    // or you can dynamically allocate the right number based on the filePath string length
    path[numChars] = '\0';  // make sure of Nul termination
    ExtractIconEx(path,0, &smallIcon, &bigIcon, 2);

    delete [] path;

    return QtWin::fromHICON(bigIcon);
}
