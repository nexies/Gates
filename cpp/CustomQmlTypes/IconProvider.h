#pragma once

#include <QObject>
#include <QQuickImageProvider>
#include "cpp/utilities/FileIconManager/FileIconProvider.h"

class IconProvider : public QQuickImageProvider
{
    Q_OBJECT

    FileIconProvider * _iconProvider;

public:
    IconProvider();
    ~IconProvider();

    virtual QPixmap requestPixmap (const QString &id, QSize * size, const QSize & requestedSize);
};
