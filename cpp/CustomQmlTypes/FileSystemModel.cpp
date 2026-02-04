#include "FileSystemModel.h"

FileSystemModel::FileSystemModel(QObject *parent) :
    QFileSystemModel(parent)
{
    setResolveSymlinks(true);
}
