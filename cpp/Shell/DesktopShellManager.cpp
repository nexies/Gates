#include "DesktopShellManager.h"

#include <QDir>
#include <QStandardPaths>
#include <QDebug>

#include <commctrl.h>
#include <Shlobj.h>

#include "cpp/Config/ConfigManager.h"

namespace Gates {

// ── Singleton ────────────────────────────────────────────────────────────────

DesktopShellManager * DesktopShellManager::_instance = nullptr;

DesktopShellManager & DesktopShellManager::instance()
{
    if (!_instance)
        _instance = new DesktopShellManager();
    return *_instance;
}

DesktopShellManager::DesktopShellManager(QObject * parent) : QObject(parent) {}

// ── Finding the ListView ─────────────────────────────────────────────────────

// Progman always exists. On older Windows it directly owns SHELLDLL_DefView.
// On Win10/11 (when a wallpaper is active), Explorer creates a separate WorkerW
// that owns SHELLDLL_DefView instead. We check both layouts.
HWND DesktopShellManager::findDesktopListView()
{
    if (_listViewHwnd && IsWindow(_listViewHwnd))
        return _listViewHwnd;

    auto findListViewIn = [](HWND parent) -> HWND {
        HWND shelldll = FindWindowEx(parent, nullptr, L"SHELLDLL_DefView", nullptr);
        if (!shelldll)
            return nullptr;
        return FindWindowEx(shelldll, nullptr, L"SysListView32", nullptr);
    };

    // Classic layout: Progman → SHELLDLL_DefView → SysListView32
    HWND progman = FindWindow(L"Progman", nullptr);
    HWND lv = findListViewIn(progman);

    if (!lv) {
        // Win10/11 layout: enumerate top-level windows looking for WorkerW
        struct FindCtx { HWND result; decltype(findListViewIn) & finder; };
        FindCtx ctx { nullptr, findListViewIn };

        EnumWindows([](HWND hwnd, LPARAM lp) -> BOOL {
            auto * ctx = reinterpret_cast<FindCtx *>(lp);
            ctx->result = ctx->finder(hwnd);
            return ctx->result ? FALSE : TRUE; // stop when found
        }, reinterpret_cast<LPARAM>(&ctx));

        lv = ctx.result;
    }

    if (!lv)
        qWarning() << "[DesktopShellManager] SysListView32 not found";

    _listViewHwnd = lv;
    return lv;
}

// ── Cross-process ListView reading ───────────────────────────────────────────

// LVM_GETITEM / LVM_GETITEMPOSITION work by passing a pointer that the ListView
// control dereferences. When sending to another process the pointer must live
// in that process's address space → VirtualAllocEx + Read/WriteProcessMemory.

namespace {

struct RemoteMem
{
    HANDLE  hProcess = nullptr;
    LPVOID  ptr      = nullptr;
    SIZE_T  size     = 0;

    RemoteMem(HANDLE proc, SIZE_T sz)
        : hProcess(proc), size(sz)
    {
        ptr = VirtualAllocEx(proc, nullptr, sz, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    }
    ~RemoteMem()
    {
        if (ptr) VirtualFreeEx(hProcess, ptr, 0, MEM_RELEASE);
    }

    bool valid() const { return ptr != nullptr; }

    bool write(const void * src, SIZE_T sz) const {
        SIZE_T written = 0;
        return WriteProcessMemory(hProcess, ptr, src, sz, &written) && written == sz;
    }
    bool read(void * dst, SIZE_T sz) const {
        SIZE_T nread = 0;
        return ReadProcessMemory(hProcess, ptr, dst, sz, &nread) && nread == sz;
    }
};

} // anonymous namespace

QList<DesktopShellManager::ListViewItem> DesktopShellManager::readListViewItems(HWND lv)
{
    QList<ListViewItem> result;
    if (!lv)
        return result;

    DWORD pid = 0;
    GetWindowThreadProcessId(lv, &pid);

    HANDLE hProcess = OpenProcess(
        PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION,
        FALSE, pid);
    if (!hProcess) {
        qWarning() << "[DesktopShellManager] OpenProcess failed, error" << GetLastError();
        return result;
    }

    const int count = (int)SendMessage(lv, LVM_GETITEMCOUNT, 0, 0);
    qDebug() << "[DesktopShellManager] ListView has" << count << "items";

    constexpr SIZE_T kTextChars = 512;
    constexpr SIZE_T kTextBytes = kTextChars * sizeof(wchar_t);

    RemoteMem remoteText(hProcess, kTextBytes);
    RemoteMem remoteLvItem(hProcess, sizeof(LVITEMW));
    RemoteMem remotePoint(hProcess, sizeof(POINT));

    if (!remoteText.valid() || !remoteLvItem.valid() || !remotePoint.valid()) {
        qWarning() << "[DesktopShellManager] VirtualAllocEx failed";
        CloseHandle(hProcess);
        return result;
    }

    for (int i = 0; i < count; ++i) {
        // --- display name ---
        LVITEMW lvItem  = {};
        lvItem.mask     = LVIF_TEXT;
        lvItem.iItem    = i;
        lvItem.iSubItem = 0;
        lvItem.pszText  = static_cast<LPWSTR>(remoteText.ptr); // remote address
        lvItem.cchTextMax = kTextChars - 1;

        remoteLvItem.write(&lvItem, sizeof(LVITEMW));
        SendMessage(lv, LVM_GETITEMW, i, reinterpret_cast<LPARAM>(remoteLvItem.ptr));

        wchar_t textBuf[kTextChars] = {};
        remoteText.read(textBuf, kTextBytes);
        const QString displayName = QString::fromWCharArray(textBuf);

        // --- position ---
        POINT pos = { 0, 0 };
        SendMessage(lv, LVM_GETITEMPOSITION, i, reinterpret_cast<LPARAM>(remotePoint.ptr));
        remotePoint.read(&pos, sizeof(POINT));

        result.append({ displayName, pos.x, pos.y });
    }

    CloseHandle(hProcess);
    return result;
}

// ── Path resolution ──────────────────────────────────────────────────────────

// Maps a ListView display name to the full file path by scanning the desktop
// folders. Handles both user desktop and public desktop.
QString DesktopShellManager::resolveDesktopPath(const QString & displayName) const
{
    // Collect all desktop folders: user desktop + public desktop
    QStringList desktopDirs =
        QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);

    // Add %PUBLIC%\Desktop which Qt doesn't include in standardLocations
    wchar_t publicDesktop[MAX_PATH] = {};
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_COMMON_DESKTOPDIRECTORY,
                                   nullptr, SHGFP_TYPE_CURRENT, publicDesktop))) {
        const QString pub = QString::fromWCharArray(publicDesktop);
        if (!desktopDirs.contains(pub))
            desktopDirs.append(pub);
    }

    for (const QString & dir : desktopDirs) {
        const QDir d(dir);
        const QFileInfoList entries =
            d.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

        for (const QFileInfo & fi : entries) {
            // ListView display names omit .lnk extension for shortcuts
            const QString baseName = fi.completeBaseName();
            if (baseName.compare(displayName, Qt::CaseInsensitive) == 0 ||
                fi.fileName().compare(displayName, Qt::CaseInsensitive) == 0) {
                return fi.absoluteFilePath();
            }
        }
    }
    return {}; // true virtual item (Recycle Bin, This PC, Network, …)
}

// ── Task #3: capture → config ────────────────────────────────────────────────

bool DesktopShellManager::captureIconPositionsToConfig()
{
    HWND lv = findDesktopListView();
    if (!lv) {
        qWarning() << "[DesktopShellManager] captureIconPositionsToConfig: ListView not found";
        return false;
    }

    const QList<ListViewItem> items = readListViewItems(lv);
    if (items.isEmpty()) {
        qWarning() << "[DesktopShellManager] no items read from ListView";
        return false;
    }

    auto & cfg = ConfigManager::instance().config();
    cfg.desktopIcons.clear();

    for (const auto & item : items) {
        const QString path = resolveDesktopPath(item.displayName);
        DesktopIconEntry entry;
        entry.path = path.isEmpty() ? (QStringLiteral("::virtual::") + item.displayName) : path;
        entry.x    = item.x;
        entry.y    = item.y;
        cfg.desktopIcons.append(entry);

        qDebug() << "[DesktopShellManager] captured icon:"
                 << item.displayName << "@" << item.x << item.y
                 << (path.isEmpty() ? "(virtual)" : path);
    }

    ConfigManager::instance().save();
    qDebug() << "[DesktopShellManager] captured" << items.size() << "desktop icons";
    return true;
}

// ── Task #4: hide / restore ──────────────────────────────────────────────────

bool DesktopShellManager::hideNativeIcons()
{
    HWND lv = findDesktopListView();
    if (!lv) {
        qWarning() << "[DesktopShellManager] hideNativeIcons: ListView not found";
        return false;
    }

    ShowWindow(lv, SW_HIDE);
    _iconsHidden = true;
    emit nativeIconsHiddenChanged(true);
    qDebug() << "[DesktopShellManager] native icons hidden";
    return true;
}

bool DesktopShellManager::restoreNativeIcons()
{
    HWND lv = findDesktopListView();
    if (!lv) {
        qWarning() << "[DesktopShellManager] restoreNativeIcons: ListView not found";
        return false;
    }

    ShowWindow(lv, SW_SHOW);
    _iconsHidden = false;
    emit nativeIconsHiddenChanged(false);
    qDebug() << "[DesktopShellManager] native icons restored";
    return true;
}

} // namespace Gates
