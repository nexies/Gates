#include "DirEntryModel.h"
#include <QDir>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include "cpp/utilities/FileIconManager/FileIconProvider.h"
#include <QDebug>
#include <QDateTime>

DirEntryModel::DirEntryModel(QObject *parent)
    : QStandardItemModel(parent)
{
    _iconProvider = new FileIconProvider();

    _watcher = new QFileSystemWatcher(this);
    connect(_watcher, &QFileSystemWatcher::directoryChanged,
            this, &DirEntryModel::reload);
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
    if (newCurrentDirPath == m_currentDirPath)
        return true;

    if (!QFileInfo(newCurrentDirPath).isDir()) {
        qWarning() << "[DirEntryModel] not a directory:" << newCurrentDirPath;
        return false;
    }

    // Unwatch the old directory
    if (!m_currentDirPath.isEmpty())
        _watcher->removePath(m_currentDirPath);

    m_currentDirPath = newCurrentDirPath;
    emit currentDirPathChanged();

    _watcher->addPath(m_currentDirPath);
    populate();
    return true;
}

void DirEntryModel::reload()
{
    // Re-add the watch: some OS implementations remove it after a change event
    if (!_watcher->directories().contains(m_currentDirPath))
        _watcher->addPath(m_currentDirPath);

    populate();
}

void DirEntryModel::populate()
{
    clear();

    QDir dir(m_currentDirPath);
    const QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for (const QFileInfo &info : entries) {
        QStandardItem *item = new QStandardItem();
        item->setData(info.fileName(),         Qt::DisplayRole);
        item->setData(info.fileName(),         FileNameRole);
        item->setData(info.lastModified(),     FileTimeRole);
        item->setData(_iconProvider->icon(info), Qt::DecorationRole);
        item->setData(info.absoluteFilePath(), FilePathRole);
        appendRow(item);
    }
}
