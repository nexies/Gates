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

    void openLocalFile(const QString & path);
    void openUrl(const QUrl & url);
};
