#pragma once
#include <QFileSystemModel>


class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT

    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath)
public:
    explicit FileSystemModel(QObject * parent = nullptr);
};
