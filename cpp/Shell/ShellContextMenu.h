#pragma once

#include <QObject>
#include <QString>
#include <QPoint>

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace Gates {

// Shows the native Windows Shell context menu for any path:
// real files, .lnk shortcuts, or virtual CLSID paths (::{...}).
// Handles IContextMenu / IContextMenu2 / IContextMenu3 with proper
// window subclassing so submenus, icons and owner-draw items work correctly.
class ShellContextMenu : public QObject
{
    Q_OBJECT

public:
    static ShellContextMenu & instance();

    // path      — absolute file path, or "::virtual::<DisplayName>"
    // screenPos — global screen coordinates for the menu
    // parentHwnd — owner window (used for menu message routing)
    void show(const QString & path, QPoint screenPos, HWND parentHwnd);

    // Show the Windows desktop background context menu (right-click on empty desktop).
    void showBackground(QPoint screenPos, HWND parentHwnd);

private:
    explicit ShellContextMenu(QObject * parent = nullptr);

    // Resolves "::virtual::<Name>" to a CLSID path "::{GUID}" when known,
    // or returns the path unchanged.
    static QString resolvePath(const QString & path);

    static ShellContextMenu * _instance;
};

} // namespace Gates
