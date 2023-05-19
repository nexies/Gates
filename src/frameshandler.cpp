#include "frameshandler.h"

#include <QTextStream>

void Gates::FramesHandler::collectAllDesktopFiles()
{
    QTextStream qout(stdout);

    QString usersPath = "C:/Users";
    QDir usersFolder(usersPath);

    QFileInfoList fullList;

    for(QFileInfo file: usersFolder.entryInfoList()){
        QString userDesktopPath = file.filePath() + "/Desktop";
        QDir userDesktop(userDesktopPath);
        fullList.append(userDesktop.entryInfoList().mid(2));
    }

    for(QFileInfo file: fullList){
        qout << file.absoluteFilePath() << endl;
    }

}
