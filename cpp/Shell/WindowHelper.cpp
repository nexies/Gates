#include "WindowHelper.h"

#include <QCoreApplication>
#include <QDebug>

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace Gates::WindowHelper {

// ── BottomLayerFilter ────────────────────────────────────────────────────────

BottomLayerFilter & BottomLayerFilter::instance()
{
    static BottomLayerFilter filter;
    return filter;
}

void BottomLayerFilter::addHwnd(HWND hwnd)
{
    if (_hwnds.isEmpty())
        qApp->installNativeEventFilter(this);
    _hwnds.insert(hwnd);
}

void BottomLayerFilter::removeHwnd(HWND hwnd)
{
    _hwnds.remove(hwnd);
    if (_hwnds.isEmpty())
        qApp->removeNativeEventFilter(this);
}

bool BottomLayerFilter::nativeEventFilter(const QByteArray & eventType,
                                          void * message, qintptr * /*result*/)
{
    if (eventType != "windows_generic_MSG")
        return false;

    const auto * msg = static_cast<MSG *>(message);

    if (msg->message != WM_WINDOWPOSCHANGING)
        return false;

    if (!_hwnds.contains(msg->hwnd))
        return false;

    // Someone is trying to change z-order — redirect to HWND_BOTTOM.
    // Position/size changes are left untouched.
    auto * wp = reinterpret_cast<WINDOWPOS *>(msg->lParam);
    if (!(wp->flags & SWP_NOZORDER)) {
        wp->hwndInsertAfter = HWND_BOTTOM;
    }

    return false; // let Qt continue normal processing
}

// ── makeDesktopWindow ────────────────────────────────────────────────────────

void makeDesktopWindow(QWindow * window, bool noActivate)
{
    // Ensure the native handle exists
    window->create();
    const HWND hwnd = reinterpret_cast<HWND>(window->winId());

    // Remove from taskbar and Alt+Tab list
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_TOOLWINDOW;
    exStyle &= ~WS_EX_APPWINDOW;
    if (noActivate)
        exStyle |= WS_EX_NOACTIVATE;
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

    // Place below all normal windows
    SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    // Register for ongoing HWND_BOTTOM maintenance
    BottomLayerFilter::instance().addHwnd(hwnd);
    qDebug() << "[WindowHelper] desktop window applied to HWND" << hwnd
             << (noActivate ? "(noActivate)" : "");
}

void unregisterDesktopWindow(QWindow * window)
{
    if (!window) return;
    const HWND hwnd = reinterpret_cast<HWND>(window->winId());
    BottomLayerFilter::instance().removeHwnd(hwnd);
}

} // namespace Gates::WindowHelper
