#pragma once
#include <QQmlEngine>
#include <QFileSystemModel>


class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath)
public:
    explicit FileSystemModel(QObject * parent = nullptr);
};
