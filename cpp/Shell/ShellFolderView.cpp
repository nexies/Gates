#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <shobjidl.h>   // IExplorerBrowser, IExplorerBrowserEvents, SHCreateItemFromParsingName
#include <shlobj.h>
#include <commctrl.h>   // ListView_SetBkColor / ListView_SetTextColor

#include "ShellFolderView.h"

#include <QDir>
#include <QQuickWindow>
#include <QDebug>

// ── ShellViewEventSink ────────────────────────────────────────────────────────
// COM event sink for IExplorerBrowser. Defined here to keep Windows headers
// out of ShellFolderView.h.

namespace Gates { class ShellFolderView; }

class ShellViewEventSink final : public IExplorerBrowserEvents
{
public:
    explicit ShellViewEventSink(Gates::ShellFolderView *owner) : _owner(owner) {}

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv) override
    {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IExplorerBrowserEvents) {
            *ppv = static_cast<IExplorerBrowserEvents *>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef()  override { return ++_ref; }
    ULONG STDMETHODCALLTYPE Release() override
    {
        const ULONG r = --_ref;
        if (!r) delete this;
        return r;
    }

    // IExplorerBrowserEvents
    HRESULT STDMETHODCALLTYPE OnNavigationPending(PCIDLIST_ABSOLUTE) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnNavigationComplete(PCIDLIST_ABSOLUTE) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnNavigationFailed(PCIDLIST_ABSOLUTE) override { return S_OK; }

    HRESULT STDMETHODCALLTYPE OnViewCreated(IShellView *psv) override;

private:
    Gates::ShellFolderView *_owner;
    ULONG                   _ref = 1;
};

// ── Helpers ──────────────────────────────────────────────────────────────────

namespace Gates {

// Converts a QQuickItem's bounding rect to a Win32 RECT in physical pixels,
// relative to the parent QQuickWindow's client area.
static RECT itemToNativeRect(QQuickItem *item)
{
    QQuickWindow *win = item->window();
    if (!win) return {0, 0, 0, 0};

    const qreal   dpr = win->devicePixelRatio();
    const QPointF pos = item->mapToScene(QPointF(0, 0));
    const int x = qRound(pos.x() * dpr);
    const int y = qRound(pos.y() * dpr);
    const int w = qRound(item->width()  * dpr);
    const int h = qRound(item->height() * dpr);
    return {x, y, x + w, y + h};
}

// ── ShellFolderView ───────────────────────────────────────────────────────────

ShellFolderView::ShellFolderView(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, false);
}

ShellFolderView::~ShellFolderView()
{
    destroyBrowser();
}

QString ShellFolderView::folderPath() const { return _path; }

void ShellFolderView::setFolderPath(const QString &path)
{
    if (_path == path) return;
    _path = path;
    emit folderPathChanged();
    if (_ready) navigateTo(path);
}

QColor ShellFolderView::backgroundColor() const { return _bgColor; }

void ShellFolderView::setBackgroundColor(const QColor &color)
{
    if (_bgColor == color) return;
    _bgColor = color;
    emit backgroundColorChanged();
}

void ShellFolderView::componentComplete()
{
    QQuickItem::componentComplete();
    tryInit();
}

void ShellFolderView::itemChange(ItemChange change, const ItemChangeData &data)
{
    QQuickItem::itemChange(change, data);
    if (change == ItemSceneChange) {
        if (data.window && !_ready)
            tryInit();
        else if (!data.window)
            destroyBrowser();
    }
}

void ShellFolderView::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
    if (!_ready) {
        tryInit();
    } else {
        // Defer to next event loop tick: QML layout may not be fully resolved yet,
        // so mapToScene() could return stale scene coordinates at this instant.
        QMetaObject::invokeMethod(this, &ShellFolderView::updateBrowserRect,
                                  Qt::QueuedConnection);
    }
}

void ShellFolderView::tryInit()
{
    if (_ready) return;
    if (!window()) return;
    if (width() <= 0 || height() <= 0) return;
    initBrowser();
}

void ShellFolderView::initBrowser()
{
    if (_ready || !window()) return;

    auto parentHwnd = reinterpret_cast<HWND>(window()->winId());
    if (!parentHwnd) return;

    HRESULT hr = CoCreateInstance(CLSID_ExplorerBrowser, nullptr,
                                   CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_browser));
    if (FAILED(hr)) {
        qWarning() << "[ShellFolderView] CoCreateInstance failed:" << Qt::hex << hr;
        return;
    }

    // Subscribe to browser events so we can style the view after navigation
    _events = new ShellViewEventSink(this);
    _browser->Advise(_events, reinterpret_cast<DWORD *>(&_eventsCookie));

    FOLDERSETTINGS fs{};
    fs.ViewMode = FVM_ICON;
    fs.fFlags   = FWF_NOHEADERINALLVIEWS | FWF_AUTOARRANGE;

    RECT rc = itemToNativeRect(this);
    hr = _browser->Initialize(parentHwnd, &rc, &fs);
    if (FAILED(hr)) {
        qWarning() << "[ShellFolderView] Initialize failed:" << Qt::hex << hr;
        _browser->Unadvise(_eventsCookie);
        _events->Release();
        _events = nullptr;
        _browser->Release();
        _browser = nullptr;
        return;
    }

    EXPLORER_BROWSER_OPTIONS opts{};
    _browser->GetOptions(&opts);
    opts = static_cast<EXPLORER_BROWSER_OPTIONS>(
        opts | EBO_NOBORDER | EBO_NOPERSISTVIEWSTATE | EBO_NOTRAVELLOG);
    _browser->SetOptions(opts);

    _ready = true;
    qDebug() << "[ShellFolderView] initialized, parentHwnd=" << parentHwnd;

    if (!_path.isEmpty())
        navigateTo(_path);
}

void ShellFolderView::navigateTo(const QString &path)
{
    if (!_browser || path.isEmpty()) return;

    const QString nativePath = QDir::toNativeSeparators(path);

    IShellItem *shellItem = nullptr;
    HRESULT hr = SHCreateItemFromParsingName(
        reinterpret_cast<LPCWSTR>(nativePath.utf16()),
        nullptr, IID_PPV_ARGS(&shellItem));

    if (SUCCEEDED(hr) && shellItem) {
        hr = _browser->BrowseToObject(shellItem, SBSP_ABSOLUTE);
        shellItem->Release();
        if (FAILED(hr))
            qWarning() << "[ShellFolderView] BrowseToObject failed:" << Qt::hex << hr;
        else
            qDebug() << "[ShellFolderView] navigated to" << nativePath;
    } else {
        qWarning() << "[ShellFolderView] SHCreateItemFromParsingName failed for"
                   << nativePath << Qt::hex << hr;
    }
}

void ShellFolderView::updateBrowserRect()
{
    if (!_browser || !window()) return;
    RECT rc = itemToNativeRect(this);
    _browser->SetRect(nullptr, rc);
}

void ShellFolderView::destroyBrowser()
{
    if (_browser) {
        if (_eventsCookie) {
            _browser->Unadvise(_eventsCookie);
            _eventsCookie = 0;
        }
        if (_events) {
            _events->Release();
            _events = nullptr;
        }
        _browser->Destroy();
        _browser->Release();
        _browser = nullptr;
        _ready   = false;
        qDebug() << "[ShellFolderView] destroyed";
    }
}

} // namespace Gates

// ── ShellViewEventSink::OnViewCreated ─────────────────────────────────────────
// Called by IExplorerBrowser after the shell view HWND is created and before
// the first navigation result is shown. Apply background/text color here so
// the ListView never shows with a white background.

HRESULT STDMETHODCALLTYPE ShellViewEventSink::OnViewCreated(IShellView *psv)
{
    if (!psv) return E_INVALIDARG;

    const QColor c = _owner->backgroundColor();
    const COLORREF bg = RGB(c.red(), c.green(), c.blue());

    IOleWindow *oleWin = nullptr;
    if (SUCCEEDED(psv->QueryInterface(IID_PPV_ARGS(&oleWin)))) {
        HWND viewHwnd = nullptr;
        if (SUCCEEDED(oleWin->GetWindow(&viewHwnd)) && viewHwnd) {
            // viewHwnd is SHELLDLL_DefView; SysListView32 is its direct child
            HWND listView = FindWindowEx(viewHwnd, nullptr, L"SysListView32", nullptr);
            if (listView) {
                ListView_SetBkColor(listView, bg);
                ListView_SetTextBkColor(listView, bg);
                ListView_SetTextColor(listView, RGB(255, 255, 255));
            }
        }
        oleWin->Release();
    }
    return S_OK;
}
