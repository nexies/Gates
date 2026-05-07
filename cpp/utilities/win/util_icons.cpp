//
// Created by green on 15.01.2026.
//

#include "util.hpp"

#if defined(Q_OS_WIN)
#include <windows.h>
#include <shobjidl.h>   // IShellItemImageFactory
#include <shlobj.h>     // SHCreateItemFromParsingName
#include <wrl/client.h>
#endif

#include <QtGui/qtgui-config.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")

namespace util {
    using Microsoft::WRL::ComPtr;

    static SIIGBF build_SIIGBF(bool includeOverlays, bool allowThumbnails) {
        SIIGBF flags = SIIGBF_RESIZETOFIT;

        if (includeOverlays) flags = (SIIGBF)(flags | SIIGBF_ICONONLY); // оверлеи обычно идут с icon-only
        // На практике: оверлеи корректнее получать через SHGetFileInfo (см. ниже),
        // но IShellItemImageFactory тоже может их учитывать в ряде случаев.

        if (!allowThumbnails) {
            flags = (SIIGBF)(flags | SIIGBF_ICONONLY);
        } else {
            // если allowThumbnails=true, Shell может вернуть thumbnail (где применимо)
            flags = (SIIGBF)(flags | SIIGBF_THUMBNAILONLY); // <-- это строго только превью
            // Чтобы "иконка или превью", используем 0 (по умолчанию) + RESIZETOFIT:
            flags = SIIGBF_RESIZETOFIT;
            // (Shell сам решит: thumbnail если доступно, иначе иконку типа)
        }
        return flags;
    }

    HICON get_HICON(const std::wstring & path,
        IconSize size,
        bool overlay,
        bool thumbnail) {

        // Важно: COM должен быть инициализирован в потоке.
        // Лучше чтобы это делал ваш апп (CoInitializeEx).
        // Тут НЕ делаем CoInitializeEx, чтобы не ломать модель приложения.

        ComPtr<IShellItem> item;
        HRESULT hr = SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&item));
        if (FAILED(hr)) return nullptr;

        ComPtr<IShellItemImageFactory> factory;
        hr = item.As(&factory);
        if (FAILED(hr)) return nullptr;

        SIZE sz{ (int)size, (int)size };

        HBITMAP hbmp = nullptr;
        SIIGBF flags = build_SIIGBF(overlay, thumbnail);

        hr = factory->GetImage(sz, flags, &hbmp);
        if (FAILED(hr) || !hbmp) return nullptr;

        // Конвертируем HBITMAP -> HICON
        ICONINFO ii{};
        ii.fIcon = TRUE;
        ii.hbmColor = hbmp;
        ii.hbmMask  = CreateBitmap((int)size, (int)size, 1, 1, nullptr);  // 1bpp маска

        HICON hIcon = CreateIconIndirect(&ii);

        if(!hIcon) {
            qWarning("CreateIconIndirect() failed.");
        }

        DeleteObject(hbmp); // bitmap больше не нужен
        return hIcon;
    }

    HBITMAP get_HBITMAP(const std::wstring& path,
                                    IconSize size,
                                    bool overlay,
                                    bool thumbnail)
    {
        ComPtr<IShellItem> item;
        HRESULT hr = SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&item));
        if (FAILED(hr)) return nullptr;

        ComPtr<IShellItemImageFactory> factory;
        hr = item.As(&factory);
        if (FAILED(hr)) return nullptr;

        SIZE sz{ (int)size, (int)size };
        HBITMAP hbmp = nullptr;

        SIIGBF flags = build_SIIGBF(overlay, thumbnail);
        hr = factory->GetImage(sz, flags, &hbmp);
        if (FAILED(hr)) return nullptr;

        return hbmp; // вызывающий DeleteObject
    }

    QIcon get_file_icon(const QString &path, IconSize size, bool include_overlay, bool allow_thumbnail)
    {
        ComPtr<IShellItem> item;
        std::wstring wpath = path.toStdWString();

        auto win_icon = get_HICON(wpath, size, include_overlay, allow_thumbnail);

        if(!win_icon)
            return {};

        auto img = QImage::fromHICON(win_icon);
        auto pix = QPixmap::fromImage(img);
        return QIcon(pix);
    }


    QPixmap get_file_pixmap (const QString & path, IconSize size, bool includeOverlays, bool allowThumbnails)
    {
        ComPtr<IShellItem> item;
        std::wstring wpath = path.toStdWString();

        auto win_icon = get_HBITMAP(wpath, size, includeOverlays, allowThumbnails);

        if(!win_icon)
            return {};

        auto img = QImage::fromHBITMAP(win_icon);
        auto pix = QPixmap::fromImage(img);
        return pix;
    }

}