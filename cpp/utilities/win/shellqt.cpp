#include "shellqt.h"

#include <Shlobj.h>
#include <commoncontrols.h>
#include <dwmapi.h>
#include <objbase.h>   // CoInitializeEx / CoUninitialize / CoCreateInstance
#include <shobjidl.h>  // IShellLinkW, IPersistFile

// SHIL_ constants are defined in <commoncontrols.h> on recent SDK versions.
// Define fallbacks in case the SDK is older.
#ifndef SHIL_LARGE
#  define SHIL_LARGE       0x0
#endif
#ifndef SHIL_SMALL
#  define SHIL_SMALL       0x1
#endif
#ifndef SHIL_EXTRALARGE
#  define SHIL_EXTRALARGE  0x2
#endif
#ifndef SHIL_SYSSMALL
#  define SHIL_SYSSMALL    0x3
#endif
#ifndef SHIL_JUMBO
#  define SHIL_JUMBO       0x4
#endif

#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QPixmap>
#include <QImage>
#include <QScopeGuard>

void EnableBlurBehind() {}

void Execute(QString file, QStringList /*params*/)
{
    ShellExecuteW(nullptr, L"open", file.toStdWString().c_str(),
                  nullptr, nullptr, SW_RESTORE);
}

// ── Core: HICON → QPixmap via GDI DIB ────────────────────────────────────────
//
// QImage::fromHICON is unreliable across Qt6 builds on Windows (font hinting,
// premultiplication bugs). We draw directly onto a 32-bpp DIB section, which
// gives raw BGRA pixels that we copy into a QImage manually.

static QPixmap iconToPixmap(HICON hIcon, int size)
{
    if (!hIcon || size <= 0)
        return {};

    HDC screenDC = GetDC(nullptr);
    HDC memDC    = CreateCompatibleDC(screenDC);

    BITMAPINFOHEADER bih = {};
    bih.biSize        = sizeof(BITMAPINFOHEADER);
    bih.biWidth       = size;
    bih.biHeight      = -size;   // negative → top-down scan order
    bih.biPlanes      = 1;
    bih.biBitCount    = 32;
    bih.biCompression = BI_RGB;  // uncompressed, alpha in high byte

    BYTE    * bits = nullptr;
    HBITMAP   hBmp = CreateDIBSection(screenDC,
                                      reinterpret_cast<BITMAPINFO *>(&bih),
                                      DIB_RGB_COLORS,
                                      reinterpret_cast<void **>(&bits),
                                      nullptr, 0);
    if (!hBmp) {
        DeleteDC(memDC);
        ReleaseDC(nullptr, screenDC);
        return {};
    }

    auto * prevBmp = static_cast<HBITMAP>(SelectObject(memDC, hBmp));

    // Start with transparent black background
    memset(bits, 0, size * size * 4);

    // DI_NORMAL: color+mask compositing for classic icons,
    // alpha-blending for 32-bit icons.
    DrawIconEx(memDC, 0, 0, hIcon, size, size, 0, nullptr, DI_NORMAL);
    GdiFlush();

    // Modern 32-bit icons set the alpha channel; classic icons leave it 0.
    // If no pixel has alpha > 0, treat as fully opaque.
    const int totalPx = size * size;
    bool hasAlpha = false;
    for (int i = 0; i < totalPx && !hasAlpha; ++i)
        hasAlpha = bits[i * 4 + 3] != 0;

    if (!hasAlpha)
        for (int i = 0; i < totalPx; ++i)
            bits[i * 4 + 3] = 0xFF;

    // Windows DIB layout (BGRA, LE): byte0=B, byte1=G, byte2=R, byte3=A
    // Qt ARGB32 layout  (BGRA, LE): byte0=B, byte1=G, byte2=R, byte3=A  ← identical
    const QImage::Format fmt = hasAlpha
                               ? QImage::Format_ARGB32_Premultiplied
                               : QImage::Format_RGB32;

    // QImage with external pointer does NOT copy — call .copy() before freeing DIB.
    QPixmap result = QPixmap::fromImage(
        QImage(bits, size, size, size * 4, fmt).copy());

    SelectObject(memDC, prevBmp);
    DeleteObject(hBmp);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);

    return result;
}

// ── IShellItemImageFactory (primary path) ────────────────────────────────────
//
// Converts an HBITMAP returned by IShellItemImageFactory::GetImage to QPixmap.
// The bitmap is a 32-bpp top-down DIB with pre-multiplied ARGB.

// QImage::fromHBITMAP (Qt 6, Windows) handles all three edge cases we care about:
//  • Reads DIBSECTION bits directly → alpha channel preserved (GetDIBits with BI_RGB zeroes it)
//  • Correctly flips bottom-up bitmaps → no upside-down thumbnails
//  • Returns Format_ARGB32_Premultiplied, matching what IShellItemImageFactory produces
static QPixmap bitmapToPixmap(HBITMAP hbmp)
{
    if (!hbmp) return {};
    const QImage img = QImage::fromHBITMAP(hbmp);
    return img.isNull() ? QPixmap{} : QPixmap::fromImage(img);
}

QPixmap extractFilePixmap(const QString & path, int size)
{
    const HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    auto comGuard = qScopeGuard([comHr] { if (comHr == S_OK) CoUninitialize(); });

    const std::wstring wpath =
        QDir::toNativeSeparators(path).toStdWString();

    IShellItem * item = nullptr;
    if (FAILED(SHCreateItemFromParsingName(wpath.c_str(), nullptr,
                                           IID_IShellItem,
                                           reinterpret_cast<void **>(&item))))
        return {};
    auto itemGuard = qScopeGuard([item] { item->Release(); });

    IShellItemImageFactory * factory = nullptr;
    if (FAILED(item->QueryInterface(IID_IShellItemImageFactory,
                                    reinterpret_cast<void **>(&factory))))
        return {};
    auto factoryGuard = qScopeGuard([factory] { factory->Release(); });

    // Always fetch at 256 so we start from the highest-quality source frame.
    // Qt's SmoothTransformation scales down much better than the shell does.
    const int fetchSize = qMax(size, 256);
    HBITMAP hbmp = nullptr;
    const SIZE sz{ fetchSize, fetchSize };
    // SIIGBF_RESIZETOFIT: shell returns thumbnail if available, icon otherwise.
    // Correctly follows .lnk shortcuts and reads the assigned game icon for Steam.
    if (FAILED(factory->GetImage(sz, SIIGBF_RESIZETOFIT, &hbmp)) || !hbmp)
        return {};

    QPixmap result = bitmapToPixmap(hbmp);
    DeleteObject(hbmp);

    if (!result.isNull() && result.size() != QSize(size, size))
        result = result.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    return result;
}

// ── Shell image list helper ───────────────────────────────────────────────────

static const IID kIID_IImageList =
    {0x46eb5926, 0x582e, 0x4017, {0x9f,0xdf,0xe8,0x99,0x8d,0xaa,0x09,0x50}};

static QPixmap pixmapFromImageList(int listId, int iIcon, int size, int overlayIndex = 0)
{
    IImageList * imageList = nullptr;
    if (FAILED(SHGetImageList(listId, kIID_IImageList,
                              reinterpret_cast<void **>(&imageList))))
        return {};

    UINT flags = ILD_TRANSPARENT;
    if (overlayIndex > 0)
        flags |= INDEXTOOVERLAYMASK(overlayIndex); // composite shortcut arrow, cloud badge, etc.

    QPixmap result;
    HICON hIcon = nullptr;
    if (SUCCEEDED(imageList->GetIcon(iIcon, flags, &hIcon)) && hIcon) {
        result = iconToPixmap(hIcon, size);
        DestroyIcon(hIcon);
    }
    imageList->Release();
    return result;
}

// ── Public API ────────────────────────────────────────────────────────────────

QIcon extractIcons(const QString & sourceFile)
{
    // CLSID paths (::{...}) always "exist" from SHGetFileInfo's perspective;
    // real files may or may not exist (use SHGFI_USEFILEATTRIBUTES for missing ones).
    const bool isCLSID  = sourceFile.startsWith(QLatin1String("::"));
    const bool exists   = isCLSID || QFileInfo::exists(sourceFile);
    const DWORD useAttr = exists ? 0u : SHGFI_USEFILEATTRIBUTES;
    const DWORD attrs   = exists ? 0u : FILE_ATTRIBUTE_NORMAL;

    const QString   native = QDir::toNativeSeparators(sourceFile);
    const wchar_t * path   = reinterpret_cast<const wchar_t *>(native.utf16());

    QIcon result;

    // --- Step 1: system icon index + overlay index ---
    // SHGFI_OVERLAYINDEX stores the overlay in bits 24-31 of iIcon.
    // This covers shortcut arrows, OneDrive/cloud sync badges, UAC shields, etc.
#ifndef SHGFI_OVERLAYINDEX
#  define SHGFI_OVERLAYINDEX 0x00000040
#endif
    SHFILEINFOW infoIdx = {};
    const bool gotIdx = SHGetFileInfoW(path, attrs, &infoIdx, sizeof(infoIdx),
                                       SHGFI_SYSICONINDEX | SHGFI_OVERLAYINDEX | useAttr) != 0;

    const int baseIconIdx  = gotIdx ? (infoIdx.iIcon & 0x00FFFFFF) : 0;
    const int overlayIndex = gotIdx ? ((static_cast<unsigned>(infoIdx.iIcon) >> 24) & 0xFF) : 0;

    // --- Step 2: small (16) and large (32) via SHGetFileInfoW ---
    // SHGFI_ICON already composites the overlay into the returned HICON.
    for (DWORD sizeFlag : { (DWORD)SHGFI_SMALLICON, (DWORD)SHGFI_LARGEICON }) {
        SHFILEINFOW info = {};
        if (SHGetFileInfoW(path, attrs, &info, sizeof(info),
                           SHGFI_ICON | sizeFlag | useAttr) && info.hIcon) {
            const int sz = (sizeFlag == SHGFI_SMALLICON) ? 16 : 32;
            QPixmap pm = iconToPixmap(info.hIcon, sz);
            DestroyIcon(info.hIcon);
            if (!pm.isNull()) result.addPixmap(pm);
        }
    }

    // --- Step 3: extralarge (48) and jumbo (256) via system image lists ---
    // Image list lookup bypasses the shell's automatic overlay compositing,
    // so we pass overlayIndex explicitly.
    if (gotIdx) {
        QPixmap pm48 = pixmapFromImageList(SHIL_EXTRALARGE, baseIconIdx, 48, overlayIndex);
        if (!pm48.isNull()) result.addPixmap(pm48);

        QPixmap pm256 = pixmapFromImageList(SHIL_JUMBO, baseIconIdx, 256, overlayIndex);
        if (!pm256.isNull()) result.addPixmap(pm256);
    }

    return result;
}

// ── Shortcut icon resolution (IShellLink) ─────────────────────────────────────

QIcon extractShortcutIcon(const QString & lnkPath)
{
    // COM may or may not be initialised on this thread already.
    // S_OK   → we initialised it, so uninitialise on exit.
    // S_FALSE → already initialised, leave it alone.
    const HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    auto comGuard = qScopeGuard([comHr] {
        if (comHr == S_OK) CoUninitialize();
    });

    IShellLinkW * sl = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_IShellLinkW, reinterpret_cast<void **>(&sl))))
        return {};

    IPersistFile * pf = nullptr;
    sl->QueryInterface(IID_IPersistFile, reinterpret_cast<void **>(&pf));
    if (!pf) { sl->Release(); return {}; }

    const bool loaded = SUCCEEDED(pf->Load(lnkPath.toStdWString().c_str(), STGM_READ));
    pf->Release();
    if (!loaded) { sl->Release(); return {}; }

    wchar_t iconPathW[MAX_PATH] = {};
    int     iconIdx = 0;
    sl->GetIconLocation(iconPathW, MAX_PATH, &iconIdx);
    sl->Release();

    if (iconPathW[0] == L'\0') return {};

    // Expand environment variables — Steam shortcuts commonly use %ProgramFiles(x86)% etc.
    wchar_t expandedW[MAX_PATH] = {};
    ExpandEnvironmentStringsW(iconPathW, expandedW, MAX_PATH);

    // SHDefExtractIconW is the authoritative Windows API for icon extraction:
    // handles .ico, .exe, .dll, PNG-compressed frames, iconIdx, and masked icons correctly.
    // Using QImageReader for .ico bypassed the AND mask, producing white pixels on
    // icons that use traditional (non-PNG) bitmask encoding.
    HICON hLarge = nullptr, hSmall = nullptr;
    const HRESULT hr = SHDefExtractIconW(expandedW, iconIdx, 0,
                                         &hLarge, &hSmall,
                                         MAKELONG(256, 48));
    if (FAILED(hr)) return {};

    QIcon result;
    if (hLarge) {
        const QPixmap pm = iconToPixmap(hLarge, 256);
        DestroyIcon(hLarge);
        if (!pm.isNull()) {
            result.addPixmap(pm);
            for (int sz : { 128, 48, 32, 16 })
                result.addPixmap(pm.scaled(sz, sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    if (hSmall) {
        const QPixmap pm = iconToPixmap(hSmall, 48);
        DestroyIcon(hSmall);
        if (!pm.isNull() && result.isNull())
            result.addPixmap(pm);
    }
    return result;
}
