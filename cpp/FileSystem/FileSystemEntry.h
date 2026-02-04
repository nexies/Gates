#ifndef FILESYSTEMENTRY_H
#define FILESYSTEMENTRY_H

#include <QHash>
#include <QFileInfo>

class FileSystemEntry
{
public:
    FileSystemEntry();


private:
    QHash<QString, QFileInfo> _infos;
    QFileInfo _root;

};

#endif // FILESYSTEMENTRY_H
