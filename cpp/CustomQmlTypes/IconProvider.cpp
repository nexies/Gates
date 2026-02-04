#include "IconProvider.h"

#include <QIcon>
#include <QPixmap>
#include <QDebug>

IconProvider::IconProvider() :
    QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    _iconProvider = new FileIconProvider;
}

IconProvider::~IconProvider()
{
    delete _iconProvider;
}

QPixmap IconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QIcon icon = _iconProvider->icon(QFileInfo(id));

    QPixmap out;
    if(requestedSize == QSize(-1, -1)){
        out = icon.pixmap(70, 70);
    }
    else
        out = icon.pixmap(requestedSize);

    size->setHeight(out.height());
    size->setWidth(out.width());

    return out;
}


