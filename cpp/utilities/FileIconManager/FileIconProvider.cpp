#include "FileIconProvider.h"

#include <QDebug>
#include <QImageReader>

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

    // For all other files (including .lnk shortcuts), use the shell icon
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
        const QPixmap pm = QPixmap::fromImage(image);
        out.addPixmap(pm.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        out.addPixmap(pm.scaled(512, 512, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // Fallback: shell icon at small size
    QIcon shellIcon = QFileIconProvider::icon(info);
    out.addPixmap(shellIcon.pixmap(32, 32));

    return out;
}
