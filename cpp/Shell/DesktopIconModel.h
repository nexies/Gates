#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QSet>
#include <QStringList>
#include "cpp/Config/ConfigManager.h"

class QScreen;
class QFileSystemWatcher;

namespace Gates {

class DesktopIconModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum Role {
        PathRole        = Qt::UserRole,
        DisplayNameRole,
        XRole,
        YRole,
    };
    Q_ENUM(Role)

    explicit DesktopIconModel(QScreen * screen, QObject * parent = nullptr);

    // Full rescan from filesystem + config reconciliation. Call on startup or
    // after the config was modified externally.
    void reload();

    Q_INVOKABLE void setPosition(int index, int x, int y);

    // QAbstractListModel
    int                    rowCount(const QModelIndex & parent = {}) const override;
    QVariant               data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void onDirectoryChanged(const QString & dir);

private:
    QSet<QString>  scanDir(const QString & dir) const;
    QPair<int,int> nextFreeCell() const;
    int            indexOfPath(const QString & path) const;

    QScreen *               _screen;
    QList<DesktopIconEntry> _icons;      // local (screen-relative) coordinates
    QFileSystemWatcher *    _watcher;
    QStringList             _sourceDirs;
};

} // namespace Gates
