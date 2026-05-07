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
    if (_hwnds.isEmpty() && _frameHwnds.isEmpty())
        qApp->installNativeEventFilter(this);
    _hwnds.insert(hwnd);
}

void BottomLayerFilter::removeHwnd(HWND hwnd)
{
    _hwnds.remove(hwnd);
    if (_hwnds.isEmpty() && _frameHwnds.isEmpty())
        qApp->removeNativeEventFilter(this);
}

void BottomLayerFilter::addFrameHwnd(HWND hwnd)
{
    if (_hwnds.isEmpty() && _frameHwnds.isEmpty())
        qApp->installNativeEventFilter(this);
    _frameHwnds.insert(hwnd);
}

void BottomLayerFilter::removeFrameHwnd(HWND hwnd)
{
    _frameHwnds.remove(hwnd);
    if (_hwnds.isEmpty() && _frameHwnds.isEmpty())
        qApp->removeNativeEventFilter(this);
}

void BottomLayerFilter::reanchorAllToBottom()
{
    for (HWND hwnd : _hwnds)
        SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

bool BottomLayerFilter::nativeEventFilter(const QByteArray & eventType,
                                          void * message, qintptr * result)
{
    if (eventType != "windows_generic_MSG")
        return false;

    const auto * msg = static_cast<MSG *>(message);

    // ── VirtualDesktop HWNDs: always HWND_BOTTOM ────────────────────────────
    if (_hwnds.contains(msg->hwnd)) {
        if (msg->message != WM_WINDOWPOSCHANGING)
            return false;
        auto * wp = reinterpret_cast<WINDOWPOS *>(msg->lParam);
        if (!(wp->flags & SWP_NOZORDER)) {
            wp->hwndInsertAfter = HWND_BOTTOM;
            const LONG_PTR exStyle = GetWindowLongPtr(msg->hwnd, GWL_EXSTYLE);
            if (exStyle & WS_EX_NOACTIVATE)
                wp->flags |= SWP_NOACTIVATE;
        }
        return false;
    }

    // ── Frame HWNDs: freeze z-order, prevent mouse-activation ───────────────
    if (_frameHwnds.contains(msg->hwnd)) {
        if (msg->message == WM_WINDOWPOSCHANGING) {
            auto * wp = reinterpret_cast<WINDOWPOS *>(msg->lParam);
            if (!(wp->flags & SWP_NOZORDER))
                wp->flags |= SWP_NOZORDER;
            return false;
        }
        if (msg->message == WM_MOUSEACTIVATE) {
            *result = MA_NOACTIVATE;
            return true;
        }
        return false;
    }

    return false;
}

// ── makeDesktopWindow ────────────────────────────────────────────────────────

void makeDesktopWindow(QWindow * window, bool noActivate)
{
    window->create();
    const HWND hwnd = reinterpret_cast<HWND>(window->winId());

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_TOOLWINDOW;
    exStyle &= ~WS_EX_APPWINDOW;
    if (noActivate)
        exStyle |= WS_EX_NOACTIVATE;
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

    // Every desktop-layer window starts at HWND_BOTTOM.
    SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    if (noActivate) {
        // VirtualDesktop: the filter keeps it at the absolute bottom.
        BottomLayerFilter::instance().addHwnd(hwnd);
    } else {
        // Frame: re-push all VD HWNDs to HWND_BOTTOM so they end up below this
        // frame (each SetWindowPos(HWND_BOTTOM) call goes to the very bottom of
        // the current stack). Then freeze the frame's z-order for all future
        // position changes and suppress mouse-driven activation.
        BottomLayerFilter::instance().reanchorAllToBottom();
        BottomLayerFilter::instance().addFrameHwnd(hwnd);
    }

    qDebug() << "[WindowHelper] desktop window applied to HWND" << hwnd
             << (noActivate ? "(VD/noActivate)" : "(frame)");
}

void unregisterDesktopWindow(QWindow * window)
{
    if (!window) return;
    const HWND hwnd = reinterpret_cast<HWND>(window->winId());
    BottomLayerFilter::instance().removeHwnd(hwnd);
    BottomLayerFilter::instance().removeFrameHwnd(hwnd);
}

} // namespace Gates::WindowHelper
