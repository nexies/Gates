#include "DesktopService.h"
#include <QDesktopServices>
#include <QUrl>

DesktopService::DesktopService(QObject *parent) {}

void DesktopService::openLocalFile(const QString &path)
{
    openUrl(QUrl::fromLocalFile(path));
}

void DesktopService::openUrl(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}
