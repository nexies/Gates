#include "shellqt.h"

void Execute(QString file, QStringList params)
{
    ShellExecuteW(NULL, L"open", file.toStdWString().c_str(), NULL, NULL, SW_RESTORE);
//    ShellExecuteW(NULL, L"open", icon->file.fileName().toStdWString().c_str(), NULL,NULL, SW_RESTORE);

}
