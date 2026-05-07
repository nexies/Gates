#include "shellqt.h"

#include <Shlobj.h>
#include <commoncontrols.h>
#include <dwmapi.h>

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
#include <QPixmap>
#include <QImage>

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

// ── Shell image list helper ───────────────────────────────────────────────────

static const IID kIID_IImageList =
    {0x46eb5926, 0x582e, 0x4017, {0x9f,0xdf,0xe8,0x99,0x8d,0xaa,0x09,0x50}};

static QPixmap pixmapFromImageList(int listId, int iIcon, int size)
{
    IImageList * imageList = nullptr;
    if (FAILED(SHGetImageList(listId, kIID_IImageList,
                              reinterpret_cast<void **>(&imageList))))
        return {};

    QPixmap result;
    HICON hIcon = nullptr;
    if (SUCCEEDED(imageList->GetIcon(iIcon, ILD_TRANSPARENT, &hIcon)) && hIcon) {
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

    // --- Step 1: get system icon index (needed for image list lookup) ---
    SHFILEINFOW infoIdx = {};
    const bool gotIdx = SHGetFileInfoW(path, attrs, &infoIdx, sizeof(infoIdx),
                                       SHGFI_SYSICONINDEX | useAttr) != 0;

    // --- Step 2: small (16) and large (32) via SHGetFileInfoW ---
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
    if (gotIdx) {
        // SHIL_EXTRALARGE = 2 → 48×48
        QPixmap pm48 = pixmapFromImageList(SHIL_EXTRALARGE, infoIdx.iIcon, 48);
        if (!pm48.isNull()) result.addPixmap(pm48);

        // SHIL_JUMBO = 4 → 256×256
        QPixmap pm256 = pixmapFromImageList(SHIL_JUMBO, infoIdx.iIcon, 256);
        if (!pm256.isNull()) result.addPixmap(pm256);
    }

    return result;
}
