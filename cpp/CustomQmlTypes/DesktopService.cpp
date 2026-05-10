#include "DesktopService.h"

#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QUrl>
#include <QDir>
#include <QDebug>
#include <QWindow>

#ifdef Q_OS_WIN
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#  include <shellapi.h>
#  include "cpp/Shell/ShellContextMenu.h"
#endif

DesktopService::DesktopService(QObject * parent) : QObject(parent) {}

void DesktopService::shellOpen(const QString & path)
{
#ifdef Q_OS_WIN
    // ShellExecuteW handles .lnk shortcuts, CLSID paths (::{...}),
    // regular files, and folders natively.
    const QString nativePath = path.startsWith(QLatin1String("::"))
                               ? path   // CLSID path — pass as-is
                               : QDir::toNativeSeparators(path);

    const HINSTANCE result = ShellExecuteW(
        nullptr, L"open",
        reinterpret_cast<const wchar_t *>(nativePath.utf16()),
        nullptr, nullptr, SW_SHOWNORMAL);

    if (reinterpret_cast<intptr_t>(result) <= 32)
        qWarning() << "[DesktopService] ShellExecuteW failed for" << path
                   << "error" << reinterpret_cast<intptr_t>(result);
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
#endif
}

void DesktopService::showContextMenu(const QString & path, int screenX, int screenY)
{
#ifdef Q_OS_WIN
    // Find a valid HWND to use as the menu owner window.
    // Prefer the currently focused Qt window; fall back to any visible one.
    HWND parentHwnd = nullptr;
    if (auto * win = QGuiApplication::focusWindow())
        parentHwnd = reinterpret_cast<HWND>(win->winId());
    if (!parentHwnd) {
        for (auto * win : QGuiApplication::allWindows()) {
            if (win->isVisible()) {
                parentHwnd = reinterpret_cast<HWND>(win->winId());
                break;
            }
        }
    }

    Gates::ShellContextMenu::instance().show(path, {screenX, screenY}, parentHwnd);
#endif
}

void DesktopService::showDesktopContextMenu(int screenX, int screenY)
{
#ifdef Q_OS_WIN
    HWND parentHwnd = nullptr;
    if (auto * win = QGuiApplication::focusWindow())
        parentHwnd = reinterpret_cast<HWND>(win->winId());
    if (!parentHwnd) {
        for (auto * win : QGuiApplication::allWindows()) {
            if (win->isVisible()) {
                parentHwnd = reinterpret_cast<HWND>(win->winId());
                break;
            }
        }
    }

    Gates::ShellContextMenu::instance().showBackground({screenX, screenY}, parentHwnd);
#endif
}

bool DesktopService::renameFile(const QString & oldPath, const QString & newName)
{
    const QString trimmed = newName.trimmed();
    if (trimmed.isEmpty()) return false;
    const QString newPath = QFileInfo(oldPath).dir().absoluteFilePath(trimmed);
    if (newPath == oldPath) return true;
    if (!QFile::rename(oldPath, newPath)) {
        qWarning() << "[DesktopService] rename failed:" << oldPath << "->" << newPath;
        return false;
    }
    return true;
}

void DesktopService::openLocalFile(const QString & path)
{
    shellOpen(path);
}

void DesktopService::openUrl(const QUrl & url)
{
    QDesktopServices::openUrl(url);
}
