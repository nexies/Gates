#pragma once

#include <QAbstractNativeEventFilter>
#include <QSet>
#include <QWindow>

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace Gates::WindowHelper {

// Applies Win32 desktop-widget styles to a Qt window and registers it with
// the app-wide HWND_BOTTOM native event filter:
//   • WS_EX_TOOLWINDOW  — hides from taskbar and Alt+Tab
//   • WS_EX_NOACTIVATE  — (optional) prevents focus steal
//   • SetWindowPos(HWND_BOTTOM) — places below all regular windows
void makeDesktopWindow(QWindow * window, bool noActivate = true);

// Removes the window from HWND_BOTTOM maintenance (call before destroying).
void unregisterDesktopWindow(QWindow * window);

// ── Internal filter (installed once on QApplication) ────────────────────────

class BottomLayerFilter : public QAbstractNativeEventFilter
{
public:
    static BottomLayerFilter & instance();

    void addHwnd(HWND hwnd);
    void removeHwnd(HWND hwnd);

    bool nativeEventFilter(const QByteArray & eventType,
                           void * message, qintptr * result) override;
private:
    BottomLayerFilter() = default;
    QSet<HWND> _hwnds;
};

} // namespace Gates::WindowHelper
