#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>
#include "cpp/Config/ConfigManager.h"

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

    explicit DesktopIconModel(QObject * parent = nullptr);

    void reload();

    Q_INVOKABLE void setPosition(int index, int x, int y);

    // QAbstractListModel
    int           rowCount(const QModelIndex & parent = {}) const override;
    QVariant      data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    QList<DesktopIconEntry> _icons;
};

} // namespace Gates
