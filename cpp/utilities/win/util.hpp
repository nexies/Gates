//
// Created by green on 15.01.2026.
//

#ifndef GATES_WIN_WIN_UTIL_HPP
#define GATES_WIN_WIN_UTIL_HPP

#include <QIcon>
#include <QPixmap>
#include <QString>

class QWindow;

namespace util {

    // Blur behind windows
    bool enable_blur_behind(QWindow * window);
    bool disable_blur_behind(QWindow * window);

    enum class IconSize {
        Small16 = 16,
        Small20 = 20,
        Small24 = 24,
        Small32 = 32,
        Large48 = 48,
        Large64 = 64,
        Jumbo256 = 256,
        Jumbo512 = 512
    };

    QIcon get_file_icon(const QString & path, IconSize size = IconSize::Small32, bool include_overlay = true, bool allow_thumbnail = true);
    QPixmap get_file_pixmap(const QString & path, IconSize size = IconSize::Small32, bool include_overlay = true, bool allow_thumbnail = true);

}


#endif //GATES_WIN_WIN_UTIL_HPP