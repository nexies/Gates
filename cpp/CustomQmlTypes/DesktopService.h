#pragma once

#include <QObject>

class DesktopService : public QObject
{
    Q_OBJECT

public:
    explicit DesktopService(QObject * parent = nullptr);

public slots:
    // Open any file/shortcut/CLSID path via Windows Shell
    void shellOpen(const QString & path);

    // Show the native Shell context menu at global screen coordinates
    void showContextMenu(const QString & path, int screenX, int screenY);

    // Show the Windows desktop background context menu (empty-area right-click)
    void showDesktopContextMenu(int screenX, int screenY);

    // Rename a file; newName is the new filename (with extension, no directory).
    // Returns true on success. QFileSystemWatcher in DirEntryModel refreshes automatically.
    Q_INVOKABLE bool renameFile(const QString & oldPath, const QString & newName);

    void openLocalFile(const QString & path);
    void openUrl(const QUrl & url);
};
