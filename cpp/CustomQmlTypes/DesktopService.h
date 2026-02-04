#pragma once

#include <QObject>

class DesktopService : public QObject
{
    Q_OBJECT

public:
    DesktopService(QObject * parent = nullptr);

public slots:
    void openLocalFile(const QString &path);
    void openUrl(const QUrl & url);
};
