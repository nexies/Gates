#include "shellqt.h"
#include <Shlobj.h>
#include <iostream>

#include <dwmapi.h>
#include <QDir>
#include <commoncontrols.h>

using namespace std;
DWM_BLURBEHIND blurSettings = {DWM_BB_ENABLE, true, NULL, FALSE};

void EnableBlurBehind() {
//    HRESULT result = DwmEnableBlurBehindWindow(HWND(1), &blurSettings);
}

void Execute(QString file, QStringList params)
{
    ShellExecuteW(NULL, L"open", file.toStdWString().c_str(), NULL, NULL, SW_RESTORE);
}


QPixmap pixmapFromShellImageList(int iImageList, const SHFILEINFO &info)
{
    QPixmap result;
    // For MinGW:
    static const IID iID_IImageList = {0x46eb5926, 0x582e, 0x4017, {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x9, 0x50}};

    IImageList *imageList = nullptr;
    if (FAILED(SHGetImageList(iImageList, iID_IImageList, reinterpret_cast<void **>(&imageList))))
        return result;

    HICON hIcon = nullptr;
    if (SUCCEEDED(imageList->GetIcon(info.iIcon, ILD_TRANSPARENT, &hIcon))) {
        DestroyIcon(hIcon);
    }
    return result;
}

QIcon extractIcons(QString sourceFile)
{

    enum { // Shell image list ids
        sHIL_EXTRALARGE = 0x2,  // 48x48 or user-defined
        sHIL_JUMBO = 0x4        // 256x256 (Vista or later)
    };

    unsigned stdSizeEntries [3] = {SHGFI_SMALLICON,
                                   SHGFI_LARGEICON,
                                   SHGFI_SHELLICONSIZE};

    const QString nativeName = QDir::toNativeSeparators(sourceFile);
    const auto *sourceFileC = reinterpret_cast<const wchar_t *>(nativeName.utf16());

    SHFILEINFO info;
    unsigned int baseFlags = SHGFI_ICON | SHGFI_SYSICONINDEX | SHGFI_ICONLOCATION;

    if (!QFileInfo(sourceFile).isDir())
        baseFlags |= SHGFI_USEFILEATTRIBUTES;

    QIcon result;


    for (unsigned sizeEntryFlag : stdSizeEntries) {
        const unsigned flags = baseFlags | sizeEntryFlag;

        ZeroMemory(&info, sizeof(SHFILEINFO));
        const HRESULT hr = SHGetFileInfo(sourceFileC, 0, &info, sizeof(SHFILEINFO), flags);
        if (FAILED(hr)) continue;

        if (info.hIcon) {
            // const QPixmap standart = QtWin::fromHICON(info.hIcon);
            DestroyIcon(info.hIcon);
            // if(!standart.isNull()) result.addPixmap(standart);
            const QPixmap extraLarge = pixmapFromShellImageList(sHIL_EXTRALARGE, info);
            if (!extraLarge.isNull()) result.addPixmap(extraLarge);

            const QPixmap jumbo = pixmapFromShellImageList(sHIL_JUMBO, info);
            if (!jumbo.isNull()) result.addPixmap(jumbo);
        }

    }
    return result;
}

