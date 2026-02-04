#include "BlurBehindHelper.h"
#include <QDebug>
#include <QWindow>
#include <QQuickWindow>

#ifdef Q_OS_WIN32
#include <windows.h>
#include <WinUser.h>
#include "cpp/utilities/win/WinStructs.h"
#endif

bool BlurBehindHelper::applyBlur(QWindow *target)
{
    Q_UNUSED(target)
#ifndef Q_OS_WIN32
    qWarning() << "[BlurBehindHelper][applyBlur] : Blur effect is only available on Windows platform!";
    return false;
#endif

#ifdef Q_OS_WIN32

    QQuickWindow *window = qobject_cast<QQuickWindow *>(target);
    if (!window) {
        qFatal("Error: Your root item has to be a window.");
        return false;
    }
    HWND hwnd = (HWND)window->winId();
    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser)
    {
        pfnSetWindowCompositionAttribute setWindowCompositionAttribute = (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (setWindowCompositionAttribute)
        {
            // ACCENT_POLICY accent = { ACCENT_ENABLE_BLURBEHIND, 0, 0, 0 };
            ACCENT_POLICY accent = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, 0, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data;
            data.Attrib = WCA_ACCENT_POLICY;
            data.pvData = &accent;
            data.cbData = sizeof(accent);
            setWindowCompositionAttribute(hwnd, &data);
        }
    }
    return true;

#endif
    return false;
}

bool BlurBehindHelper::disableBlur(QWindow *target)
{
    Q_UNUSED(target)
#ifndef Q_OS_WIN32
    qWarning() << "[BlurBehindHelper][applyBlur] : Blur effect is only available on Windows platform!";
    return false;
#endif

#ifdef Q_OS_WIN32

#endif
    return false;
}
