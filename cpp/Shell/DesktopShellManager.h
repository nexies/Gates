#pragma once

#include <QObject>
#include <QList>
#include <QString>

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace Gates {

class DesktopShellManager : public QObject
{
    Q_OBJECT

public:
    static DesktopShellManager & instance();

    // Task #4 — hide / restore native SysListView32 icons
    bool hideNativeIcons();
    bool restoreNativeIcons();
    bool nativeIconsHidden() const { return _iconsHidden; }

    // Task #3 — read ListView positions into ConfigManager on first launch
    bool captureIconPositionsToConfig();

signals:
    void nativeIconsHiddenChanged(bool hidden);

private:
    explicit DesktopShellManager(QObject * parent = nullptr);

    // Returns the HWND of the SysListView32 that holds desktop icons.
    // Handles both the classic Progman layout and the Win10/11 WorkerW layout.
    HWND findDesktopListView();

    struct ListViewItem {
        QString displayName;
        int     x = 0;
        int     y = 0;
    };

    // Reads item display names and positions from the ListView using
    // cross-process VirtualAllocEx / ReadProcessMemory.
    QList<ListViewItem> readListViewItems(HWND lv);

    // Tries to resolve a desktop display name to a full file path by
    // scanning the user and public desktop folders.
    QString resolveDesktopPath(const QString & displayName) const;

    HWND _listViewHwnd = nullptr;
    bool _iconsHidden  = false;

    static DesktopShellManager * _instance;
};

} // namespace Gates
