#pragma once

#include <QAbstractNativeEventFilter>
#include <QSet>
#include <QWindow>

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace Gates::WindowHelper {

// Applies Win32 desktop-widget styles to a Qt window:
//   noActivate=true  (VirtualDesktop): WS_EX_TOOLWINDOW + WS_EX_NOACTIVATE, kept at
//                    HWND_BOTTOM by the filter. Covers the whole screen, never clicked.
//   noActivate=false (Frame): WS_EX_TOOLWINDOW only, placed just above the VD layer.
//                    Z-order is frozen by the filter; mouse clicks don't steal focus.
void makeDesktopWindow(QWindow * window, bool noActivate = true);
void unregisterDesktopWindow(QWindow * window);

// ── Internal filter (installed once on QApplication) ────────────────────────

class BottomLayerFilter : public QAbstractNativeEventFilter
{
public:
    static BottomLayerFilter & instance();

    // VirtualDesktop HWNDs — always kept at HWND_BOTTOM.
    void addHwnd(HWND hwnd);
    void removeHwnd(HWND hwnd);

    // Frame HWNDs — z-order frozen (just above VD layer), no mouse-activation.
    void addFrameHwnd(HWND hwnd);
    void removeFrameHwnd(HWND hwnd);

    // Explicitly re-send all VD HWNDs to HWND_BOTTOM (call after placing a new
    // frame at HWND_BOTTOM so the VD windows end up below it).
    void reanchorAllToBottom();

    bool nativeEventFilter(const QByteArray & eventType,
                           void * message, qintptr * result) override;
private:
    BottomLayerFilter() = default;

    QSet<HWND> _hwnds;       // VD HWNDs
    QSet<HWND> _frameHwnds;  // Frame HWNDs
};

} // namespace Gates::WindowHelper
