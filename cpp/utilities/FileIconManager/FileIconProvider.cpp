#include "FileIconProvider.h"

#include <QDebug>
#include <QImage>
#include <QImageReader>
#include <QPainter>
#include <QPixmap>

#ifdef Q_OS_WIN
#  include "cpp/utilities/win/shellqt.h"
#endif

FileIconProvider::FileIconProvider() {}

QIcon FileIconProvider::icon(const QFileInfo & info) const
{
#ifdef Q_OS_WIN
    // For image files, compose shell icon + scaled thumbnail
    if (!QImageReader::imageFormat(info.filePath()).isEmpty())
        return extractImageIcon(info);

    // For .lnk shortcuts, resolve the icon location via IShellLink first.
    // SHGetFileInfoW doesn't expand environment variables in shortcut icon paths,
    // so Steam-style shortcuts with %ProgramFiles(x86)% in the icon path fail silently.
    if (info.suffix().compare(QLatin1String("lnk"), Qt::CaseInsensitive) == 0) {
        const QIcon lnkIcon = extractShortcutIcon(info.filePath());
        if (!lnkIcon.isNull()) return lnkIcon;
    }

    // For all other files, use the shell icon
    QIcon shellIcon = extractIcons(info.filePath());
    if (!shellIcon.isNull())
        return shellIcon;
#endif

    return QFileIconProvider::icon(info);
}

QIcon FileIconProvider::extractImageIcon(const QFileInfo & info) const
{
    QImageReader reader(info.filePath());
    QImage       image = reader.read();

    QIcon out;
    if (!image.isNull()) {
        // PDF thumbnails are rendered without a background (transparent alpha).
        // Composite over white so the page content is visible against any backdrop.
        if (info.suffix().compare(QLatin1String("pdf"), Qt::CaseInsensitive) == 0
                && image.hasAlphaChannel()) {
            QImage bg(image.size(), QImage::Format_RGB32);
            bg.fill(Qt::white);
            QPainter p(&bg);
            p.drawImage(0, 0, image);
            image = bg;
        }

        const QPixmap pm = QPixmap::fromImage(image);
        out.addPixmap(pm.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        out.addPixmap(pm.scaled(512, 512, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // Fallback: shell icon at small size
    QIcon shellIcon = QFileIconProvider::icon(info);
    out.addPixmap(shellIcon.pixmap(32, 32));

    return out;
}
