#include "DesktopIconModel.h"
#include <QFileInfo>

namespace Gates {

DesktopIconModel::DesktopIconModel(QObject * parent)
    : QAbstractListModel(parent)
{
    reload();
}

void DesktopIconModel::reload()
{
    beginResetModel();
    _icons = ConfigManager::instance().config().desktopIcons;
    endResetModel();
}

void DesktopIconModel::setPosition(int index, int x, int y)
{
    if (index < 0 || index >= _icons.size())
        return;

    _icons[index].x = x;
    _icons[index].y = y;

    // Persist to config
    ConfigManager::instance().config().desktopIcons = _icons;
    ConfigManager::instance().save();

    emit dataChanged(this->index(index), this->index(index), { XRole, YRole });
}

int DesktopIconModel::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : _icons.size();
}

QVariant DesktopIconModel::data(const QModelIndex & idx, int role) const
{
    if (!idx.isValid() || idx.row() >= _icons.size())
        return {};

    const auto & entry = _icons.at(idx.row());

    switch (role) {
    case PathRole:
        return entry.path;
    case DisplayNameRole:
        if (entry.path.startsWith(QStringLiteral("::virtual::")))
            return entry.path.mid(11); // strip "::virtual::" prefix
        return QFileInfo(entry.path).completeBaseName();
    case XRole:
        return entry.x;
    case YRole:
        return entry.y;
    default:
        return {};
    }
}

QHash<int, QByteArray> DesktopIconModel::roleNames() const
{
    return {
        { PathRole,        "path"        },
        { DisplayNameRole, "displayName" },
        { XRole,           "iconX"       },
        { YRole,           "iconY"       },
    };
}

} // namespace Gates
