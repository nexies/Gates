#include "IconProvider.h"

#include <QIcon>
#include <QPixmap>
#include <QDebug>

#ifdef Q_OS_WIN
#  include "cpp/utilities/win/shellqt.h"

// Known virtual shell item CLSIDs → special folder paths that SHGetFileInfo understands
static QString virtualItemPath(const QString & displayName)
{
    static const QHash<QString, QString> knownVirtual = {
        { "recycle bin",   "::{645FF040-5081-101B-9F08-00AA002F954E}" },
        { "корзина",       "::{645FF040-5081-101B-9F08-00AA002F954E}" },
        { "this pc",       "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}" },
        { "этот компьютер","::{20D04FE0-3AEA-1069-A2D8-08002B30309D}" },
        { "мой компьютер", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}" },
        { "network",       "::{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}" },
        { "сеть",          "::{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}" },
        { "user's files",  "::{59031A47-3F72-44A7-89C5-5595FE6B30EE}" },
    };
    return knownVirtual.value(displayName.toLower());
}
#endif

IconProvider::IconProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    _iconProvider = new FileIconProvider;
}

IconProvider::~IconProvider()
{
    delete _iconProvider;
}

QPixmap IconProvider::requestPixmap(const QString & id, QSize * size,
                                    const QSize & requestedSize)
{
    const int targetSz = (requestedSize.isValid() && requestedSize != QSize(-1, -1))
                         ? qMax(requestedSize.width(), requestedSize.height()) : 48;

    QPixmap out;

#ifdef Q_OS_WIN
    if (id.startsWith(QLatin1String("::virtual::"))) {
        const QString clsidPath = virtualItemPath(id.mid(11));

        if (!clsidPath.isEmpty())
            out = extractFilePixmap(clsidPath, targetSz);

        if (out.isNull()) {
            // try the display name directly (SHCreateItemFromParsingName may resolve it)
            out = extractFilePixmap(id.mid(11), targetSz);
        }

        if (out.isNull()) {
            const QIcon fallback = QIcon::fromTheme(
                QStringLiteral("system-file-manager"),
                QIcon(QStringLiteral(":/icons/home.png")));
            out = fallback.pixmap(targetSz, targetSz);
        }
    } else {
        // Primary: IShellItemImageFactory — handles .lnk shortcuts, all file types
        out = extractFilePixmap(id, targetSz);

        // Fallback: legacy shell extraction (SHGetFileInfo + image lists)
        if (out.isNull())
            out = _iconProvider->icon(QFileInfo(id)).pixmap(targetSz, targetSz);
    }
#else
    out = _iconProvider->icon(QFileInfo(id)).pixmap(targetSz, targetSz);
#endif

    if (size) {
        size->setWidth(out.width());
        size->setHeight(out.height());
    }
    return out;
}
