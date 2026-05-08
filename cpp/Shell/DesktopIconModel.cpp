#include "DesktopIconModel.h"
#include <QFileInfo>
#include <QGuiApplication>
#include <QPair>
#include <QScreen>
#include <QtMath>

namespace {
// Layout helpers — read live from config so future settings UI takes effect immediately.
const Gates::DesktopLayoutConfig & layout() {
    return Gates::ConfigManager::instance().config().desktopLayout;
}

// Returns the screen whose availableGeometry contains (x, y), or nullptr.
QScreen * screenForGlobalPos(int x, int y)
{
    for (QScreen * s : QGuiApplication::screens())
        if (s->availableGeometry().contains(x, y))
            return s;
    return nullptr;
}

// Returns the screen nearest to (x, y) by closest-edge distance.
QScreen * nearestScreen(int x, int y)
{
    const auto screens = QGuiApplication::screens();
    if (screens.isEmpty())
        return nullptr;

    QScreen * best     = screens.first();
    int       bestDist = INT_MAX;
    for (QScreen * s : screens) {
        const QRect g = s->availableGeometry();
        const int cx  = qBound(g.left(), x, g.right());
        const int cy  = qBound(g.top(),  y, g.bottom());
        const int d   = (x - cx) * (x - cx) + (y - cy) * (y - cy);
        if (d < bestDist) { bestDist = d; best = s; }
    }
    return best;
}
} // anonymous namespace

namespace Gates {

DesktopIconModel::DesktopIconModel(QScreen * screen, QObject * parent)
    : QAbstractListModel(parent), _screen(screen)
{
    reload();
}

void DesktopIconModel::reload()
{
    beginResetModel();
    _icons.clear();
    _configIndex.clear();

    const auto & allIcons = ConfigManager::instance().config().desktopIcons;
    const QRect  myGeom   = _screen->availableGeometry();

    const auto & lay = layout();

    // Track occupied grid cells as (col, row) packed integer.
    QSet<qint64> occupied;
    auto cellKey = [](int col, int row) -> qint64 {
        return (qint64)col * 100000 + row;
    };
    auto localToCell = [&lay](int lx, int ly) -> QPair<int,int> {
        return { qMax(0, (lx - lay.margin) / lay.stepX()),
                 qMax(0, (ly - lay.margin) / lay.stepY()) };
    };

    // First pass: icons that belong to this screen.
    for (int i = 0; i < allIcons.size(); ++i) {
        const auto & ic = allIcons.at(i);
        if (screenForGlobalPos(ic.x, ic.y) != _screen)
            continue;

        DesktopIconEntry local = ic;
        local.x -= myGeom.x();
        local.y -= myGeom.y();
        _icons.append(local);
        _configIndex.append(i);
        const auto [c, r] = localToCell(local.x, local.y);
        occupied.insert(cellKey(c, r));
    }

    // Second pass: icons whose global position doesn't match any screen.
    // Assign them to the nearest screen; place in first free grid cell.
    for (int i = 0; i < allIcons.size(); ++i) {
        const auto & ic = allIcons.at(i);
        if (screenForGlobalPos(ic.x, ic.y) != nullptr)
            continue;
        if (nearestScreen(ic.x, ic.y) != _screen)
            continue;

        const int cols = qMax(1, (myGeom.width() - 2 * lay.margin) / lay.stepX());
        int cell = 0;
        while (occupied.contains(cellKey(cell % cols, cell / cols)))
            ++cell;
        const int col = cell % cols;
        const int row = cell / cols;

        DesktopIconEntry local = ic;
        local.x = lay.margin + col * lay.stepX();
        local.y = lay.margin + row * lay.stepY();
        _icons.append(local);
        _configIndex.append(i);
        occupied.insert(cellKey(col, row));
    }

    endResetModel();
}

void DesktopIconModel::setPosition(int index, int localX, int localY)
{
    if (index < 0 || index >= _icons.size())
        return;

    _icons[index].x = localX;
    _icons[index].y = localY;

    // Convert local → global and update the shared config entry.
    const QRect myGeom = _screen->availableGeometry();
    auto & cfg = ConfigManager::instance().config();
    const int ci = _configIndex.at(index);
    cfg.desktopIcons[ci].x = localX + myGeom.x();
    cfg.desktopIcons[ci].y = localY + myGeom.y();
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
            return entry.path.mid(11);
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
