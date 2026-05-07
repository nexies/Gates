#include "ShellContextMenu.h"

#include <QDebug>
#include <QDir>

#include <Shlobj.h>
#include <Shlwapi.h>
#include <shellapi.h>
#include <commctrl.h>   // SetWindowSubclass / DefSubclassProc

// ── IContextMenu2/3 subclass hook ────────────────────────────────────────────
//
// TrackPopupMenu runs a modal loop. During that loop Windows sends
// WM_INITMENUPOPUP / WM_DRAWITEM / WM_MEASUREITEM / WM_MENUCHAR to the
// owner window. We must forward these to IContextMenu2/3 so submenu icons
// and owner-drawn items render correctly.
//
// We use SetWindowSubclass (comctl32) to temporarily intercept those messages
// for the duration of the TrackPopupMenu call.

namespace {

struct MenuState {
    IContextMenu2 * cm2 = nullptr;
    IContextMenu3 * cm3 = nullptr;
};

LRESULT CALLBACK menuSubclassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
                                   UINT_PTR /*id*/, DWORD_PTR data)
{
    auto * state = reinterpret_cast<MenuState *>(data);

    if (state->cm3) {
        LRESULT res = 0;
        if (SUCCEEDED(state->cm3->HandleMenuMsg2(msg, wp, lp, &res))
            && (msg == WM_MENUCHAR))   // only swallow WM_MENUCHAR returns
            return res;
    } else if (state->cm2) {
        if (msg == WM_INITMENUPOPUP ||
            msg == WM_DRAWITEM      ||
            msg == WM_MEASUREITEM) {
            state->cm2->HandleMenuMsg(msg, wp, lp);
            return 0;
        }
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

constexpr UINT_PTR kSubclassId = 0xCA75;

} // anonymous namespace

// ── Helpers ───────────────────────────────────────────────────────────────────

namespace Gates {

ShellContextMenu * ShellContextMenu::_instance = nullptr;

ShellContextMenu & ShellContextMenu::instance()
{
    if (!_instance)
        _instance = new ShellContextMenu();
    return *_instance;
}

ShellContextMenu::ShellContextMenu(QObject * parent) : QObject(parent) {}

QString ShellContextMenu::resolvePath(const QString & path)
{
    if (!path.startsWith(QLatin1String("::virtual::")))
        return path;

    const QString name = path.mid(11).toLower();

    static const QHash<QString, QLatin1String> kCLSID = {
        { "recycle bin",    QLatin1String("::{645FF040-5081-101B-9F08-00AA002F954E}") },
        { "корзина",        QLatin1String("::{645FF040-5081-101B-9F08-00AA002F954E}") },
        { "this pc",        QLatin1String("::{20D04FE0-3AEA-1069-A2D8-08002B30309D}") },
        { "этот компьютер", QLatin1String("::{20D04FE0-3AEA-1069-A2D8-08002B30309D}") },
        { "мой компьютер",  QLatin1String("::{20D04FE0-3AEA-1069-A2D8-08002B30309D}") },
        { "network",        QLatin1String("::{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}") },
        { "сеть",           QLatin1String("::{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}") },
    };

    return kCLSID.value(name, QLatin1String(""));
}

// ── Main implementation ───────────────────────────────────────────────────────

void ShellContextMenu::show(const QString & path, QPoint screenPos, HWND parentHwnd)
{
    const QString resolved = resolvePath(path);
    if (resolved.isEmpty()) {
        qWarning() << "[ShellContextMenu] cannot resolve path for context menu:" << path;
        return;
    }

    // ── 1. Parse path into an absolute PIDL ──────────────────────────────────
    PIDLIST_ABSOLUTE pidlFull = nullptr;
    {
        const std::wstring wpath = QDir::toNativeSeparators(resolved).toStdWString();
        SFGAOF attrs = 0;
        const HRESULT hr = SHParseDisplayName(wpath.c_str(), nullptr,
                                              &pidlFull, 0, &attrs);
        if (FAILED(hr)) {
            qWarning() << "[ShellContextMenu] SHParseDisplayName failed"
                       << Qt::hex << (uint)hr << "for" << resolved;
            return;
        }
    }

    // ── 2. Split into parent folder pidl + child item pidl ───────────────────
    // ILClone the full pidl so we can ILRemoveLastID on the clone for parent.
    PIDLIST_ABSOLUTE pidlParent = ILClone(pidlFull);
    ILRemoveLastID(pidlParent);
    PCUITEMID_CHILD pidlChild = ILFindLastID(pidlFull);

    // ── 3. Get IShellFolder for the parent ───────────────────────────────────
    IShellFolder * desktop = nullptr;
    SHGetDesktopFolder(&desktop);

    IShellFolder * parentFolder = nullptr;
    if (ILIsEmpty(pidlParent)) {
        parentFolder = desktop;
        desktop->AddRef();
    } else {
        desktop->BindToObject(pidlParent, nullptr,
                              IID_IShellFolder,
                              reinterpret_cast<void **>(&parentFolder));
    }

    if (!parentFolder) {
        qWarning() << "[ShellContextMenu] BindToObject failed for parent folder";
        desktop->Release();
        CoTaskMemFree(pidlParent);
        CoTaskMemFree(pidlFull);
        return;
    }

    // ── 4. Get IContextMenu from the shell folder ─────────────────────────────
    IContextMenu * cm = nullptr;
    LPCITEMIDLIST items[] = { pidlChild };
    parentFolder->GetUIObjectOf(parentHwnd, 1, items,
                                IID_IContextMenu, nullptr,
                                reinterpret_cast<void **>(&cm));

    if (!cm) {
        qWarning() << "[ShellContextMenu] GetUIObjectOf(IContextMenu) failed";
        parentFolder->Release();
        desktop->Release();
        CoTaskMemFree(pidlParent);
        CoTaskMemFree(pidlFull);
        return;
    }

    // ── 5. Try to promote to IContextMenu3 or IContextMenu2 ──────────────────
    MenuState state;
    cm->QueryInterface(IID_IContextMenu3, reinterpret_cast<void **>(&state.cm3));
    if (!state.cm3)
        cm->QueryInterface(IID_IContextMenu2, reinterpret_cast<void **>(&state.cm2));

    IContextMenu * activeCm = state.cm3
                              ? static_cast<IContextMenu *>(state.cm3)
                              : state.cm2
                                ? static_cast<IContextMenu *>(state.cm2)
                                : cm;

    // ── 6. Build the popup menu ───────────────────────────────────────────────
    HMENU hMenu = CreatePopupMenu();
    const HRESULT qcmHr = activeCm->QueryContextMenu(
        hMenu, 0,
        /*idCmdFirst=*/1, /*idCmdLast=*/0x7FFF,
        CMF_NORMAL | CMF_EXPLORE);

    if (FAILED(qcmHr)) {
        qWarning() << "[ShellContextMenu] QueryContextMenu failed" << Qt::hex << (uint)qcmHr;
        DestroyMenu(hMenu);
        goto cleanup;
    }

    // ── 7. Install subclass to handle menu messages during modal loop ─────────
    if (parentHwnd && (state.cm2 || state.cm3))
        SetWindowSubclass(parentHwnd, menuSubclassProc, kSubclassId,
                          reinterpret_cast<DWORD_PTR>(&state));

    {
        // ── 8. Show the menu ──────────────────────────────────────────────────
        SetForegroundWindow(parentHwnd); // required for proper menu dismissal
        const int cmd = TrackPopupMenu(
            hMenu,
            TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON,
            screenPos.x(), screenPos.y(),
            0, parentHwnd, nullptr);

        // ── 9. Remove subclass ────────────────────────────────────────────────
        if (parentHwnd && (state.cm2 || state.cm3))
            RemoveWindowSubclass(parentHwnd, menuSubclassProc, kSubclassId);

        // ── 10. Execute the selected command ──────────────────────────────────
        if (cmd > 0) {
            CMINVOKECOMMANDINFOEX info = {};
            info.cbSize  = sizeof(info);
            info.fMask   = CMIC_MASK_UNICODE | CMIC_MASK_ASYNCOK;
            info.hwnd    = parentHwnd;
            info.lpVerb  = MAKEINTRESOURCEA(cmd - 1);
            info.lpVerbW = MAKEINTRESOURCEW(cmd - 1);
            info.nShow   = SW_SHOWNORMAL;

            const HRESULT hr = activeCm->InvokeCommand(
                reinterpret_cast<CMINVOKECOMMANDINFO *>(&info));
            if (FAILED(hr))
                qWarning() << "[ShellContextMenu] InvokeCommand failed"
                           << Qt::hex << (uint)hr;
        }
    }

    DestroyMenu(hMenu);

cleanup:
    if (state.cm3) state.cm3->Release();
    if (state.cm2) state.cm2->Release();
    cm->Release();
    parentFolder->Release();
    desktop->Release();
    CoTaskMemFree(pidlParent);
    CoTaskMemFree(pidlFull);
}

} // namespace Gates
