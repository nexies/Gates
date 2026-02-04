#include "DirEntryModel.h"
#include <QDir>
#include <QFileInfo>
#include "cpp/utilities/FileIconManager/FileIconProvider.h"
#include <QDebug>
#include <QDateTime>

DirEntryModel::DirEntryModel(QObject *parent) :
    QStandardItemModel(parent)
{
    _iconProvider = new FileIconProvider();
    setCurDirPath(QDir::rootPath());
}

QVariant DirEntryModel::data(const QModelIndex &index, int role) const
{
    return QStandardItemModel::data(index, role);
}

QHash<int, QByteArray> DirEntryModel::roleNames() const
{
    QHash<int, QByteArray> result = QStandardItemModel::roleNames();
    result.insert(FileNameRole, "fileName");
    result.insert(FilePathRole, "filePath");
    result.insert(FilePermissionsRole, "filePermissions");
    result.insert(FileTimeRole, "fileCreationTime");
    return result;
}

QString DirEntryModel::currentDirPath() const
{
    return m_currentDirPath;
}

bool DirEntryModel::setCurDirPath(const QString &newCurrentDirPath)
{
    if(!QFileInfo(newCurrentDirPath).isDir())
    {
        qWarning() << "[DirEntryModel][setCurDirPath] :" << newCurrentDirPath << "is not a directory!";
        return false;
    }
    m_currentDirPath = newCurrentDirPath;

    clear();

    QDir dir(newCurrentDirPath);
    QFileInfoList entryList = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for(auto info: entryList)
    {
        QStandardItem * item = new QStandardItem();
        item->setData(info.fileName(), Qt::DisplayRole);
        item->setData(info.fileName(), FileNameRole);
        item->setData(info.lastModified(), FileTimeRole);
        item->setData(_iconProvider->icon(info), Qt::DecorationRole);
        item->setData(info.absoluteFilePath(), FilePathRole);
        // item->setData(info.permissions(), FilePermissionsRole);
        appendRow(item);
    }
    return true;
}
