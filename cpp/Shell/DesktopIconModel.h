#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QVector>
#include "cpp/Config/ConfigManager.h"

class QScreen;

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

    void reload();

    Q_INVOKABLE void setPosition(int index, int x, int y);

    // QAbstractListModel
    int           rowCount(const QModelIndex & parent = {}) const override;
    QVariant      data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    QScreen *               _screen;
    QList<DesktopIconEntry> _icons;        // local (screen-relative) coordinates
    QVector<int>            _configIndex;  // maps model row → config desktopIcons index
};

} // namespace Gates
