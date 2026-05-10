#include "DesktopIconModel.h"
#include "DesktopFolderLocator.h"

#include <QDir>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QGuiApplication>
#include <QScreen>

namespace {

const Gates::DesktopLayoutConfig & layout()
{
    return Gates::ConfigManager::instance().config().desktopLayout;
}

QScreen * screenForGlobalPos(int x, int y)
{
    for (QScreen * s : QGuiApplication::screens())
        if (s->availableGeometry().contains(x, y))
            return s;
    return nullptr;
}

auto cellKey(int col, int row) -> qint64
{
    return (qint64)col * 100000 + row;
}

} // anonymous namespace

namespace Gates {

DesktopIconModel::DesktopIconModel(QScreen * screen, QObject * parent)
    : QAbstractListModel(parent)
    , _screen(screen)
    , _watcher(new QFileSystemWatcher(this))
{
    _sourceDirs = DesktopFolderLocator::sourceDirs();
    _watcher->addPaths(_sourceDirs);

    connect(_watcher, &QFileSystemWatcher::directoryChanged,
            this, &DesktopIconModel::onDirectoryChanged);

    reload();
}

// ── Full rescan ───────────────────────────────────────────────────────────────

void DesktopIconModel::reload()
{
    beginResetModel();
    _icons.clear();

    const QRect myGeom = _screen->availableGeometry();
    const auto &lay    = layout();
    const bool primary = (_screen == QGuiApplication::primaryScreen());

    // Scan all source directories
    QStringList actualFiles;
    for (const QString &dir : std::as_const(_sourceDirs))
        for (const QString &f : scanDir(dir))
            actualFiles.append(f);

    const QSet<QString> actualSet(actualFiles.begin(), actualFiles.end());

    // Position lookup from config: path → global QPoint
    QHash<QString, QPoint> configPos;
    for (const auto &ic : ConfigManager::instance().config().desktopIcons)
        configPos.insert(ic.path, QPoint(ic.x, ic.y));

    QSet<qint64> occupied;
    auto markOccupied = [&](int localX, int localY) {
        occupied.insert(cellKey(qMax(0, (localX - lay.margin) / lay.stepX()),
                                qMax(0, (localY - lay.margin) / lay.stepY())));
    };

    QStringList needsAutoPlace;

    // First pass: files that have a stored position on this screen
    for (const QString &path : std::as_const(actualFiles)) {
        if (!configPos.contains(path)) {
            if (primary) needsAutoPlace.append(path);
            continue;
        }
        const QPoint gp = configPos.value(path);
        if (screenForGlobalPos(gp.x(), gp.y()) != _screen)
            continue;
        DesktopIconEntry entry;
        entry.path = path;
        entry.x    = gp.x() - myGeom.x();
        entry.y    = gp.y() - myGeom.y();
        _icons.append(entry);
        markOccupied(entry.x, entry.y);
    }

    // Second pass: auto-place new files on the primary screen
    if (primary) {
        const int cols = qMax(1, (myGeom.width() - 2 * lay.margin) / lay.stepX());
        for (const QString &path : std::as_const(needsAutoPlace)) {
            int cell = 0;
            while (occupied.contains(cellKey(cell % cols, cell / cols)))
                ++cell;
            const int col = cell % cols;
            const int row = cell / cols;

            DesktopIconEntry entry;
            entry.path = path;
            entry.x    = lay.margin + col * lay.stepX();
            entry.y    = lay.margin + row * lay.stepY();
            _icons.append(entry);
            occupied.insert(cellKey(col, row));

            DesktopIconEntry global = entry;
            global.x += myGeom.x();
            global.y += myGeom.y();
            ConfigManager::instance().config().desktopIcons.append(global);
        }
    }

    // Reconcile: remove config entries for files that no longer exist
    auto &allIcons   = ConfigManager::instance().config().desktopIcons;
    const int before = allIcons.size();
    allIcons.removeIf([&](const DesktopIconEntry &e) {
        return !actualSet.contains(e.path);
    });

    if (allIcons.size() != before || !needsAutoPlace.isEmpty())
        ConfigManager::instance().save();

    endResetModel();
}

// ── Real-time directory change ────────────────────────────────────────────────

void DesktopIconModel::onDirectoryChanged(const QString & dir)
{
    // Re-add watch — some OS implementations remove it after a change event
    if (!_watcher->directories().contains(dir))
        _watcher->addPath(dir);

    const QSet<QString> newFiles = scanDir(dir);
    const QString cleanDir = QDir::cleanPath(dir);

    // Files from this directory currently in the model
    QSet<QString> oldFiles;
    for (const auto &icon : std::as_const(_icons))
        if (QDir::cleanPath(QFileInfo(icon.path).absolutePath()) == cleanDir)
            oldFiles.insert(icon.path);

    const QSet<QString> added   = newFiles - oldFiles;
    const QSet<QString> removed = oldFiles - newFiles;

    bool dirty = false;

    // Remove entries for deleted files
    for (const QString &path : removed) {
        const int idx = indexOfPath(path);
        if (idx < 0) continue;
        beginRemoveRows({}, idx, idx);
        _icons.removeAt(idx);
        endRemoveRows();
        auto &all        = ConfigManager::instance().config().desktopIcons;
        const int before = all.size();
        all.removeIf([&path](const DesktopIconEntry &e) { return e.path == path; });
        if (all.size() != before) dirty = true;
    }

    // Add entries for new files (primary screen only).
    // A caller (e.g. GatesFrameDispatcher) may have already written the desired
    // position to config before the file appeared on disk, so check config first.
    if (_screen == QGuiApplication::primaryScreen()) {
        const QRect myGeom = _screen->availableGeometry();
        const auto &lay    = layout();

        QHash<QString, QPoint> configPos;
        for (const auto &ic : ConfigManager::instance().config().desktopIcons)
            configPos.insert(ic.path, QPoint(ic.x, ic.y));

        for (const QString &path : added) {
            DesktopIconEntry entry;
            entry.path = path;

            if (configPos.contains(path)) {
                const QPoint gp = configPos.value(path);
                entry.x = gp.x() - myGeom.x();
                entry.y = gp.y() - myGeom.y();
                // Position already in config — nothing to write
            } else {
                auto [col, row] = nextFreeCell();
                entry.x = lay.margin + col * lay.stepX();
                entry.y = lay.margin + row * lay.stepY();

                DesktopIconEntry global = entry;
                global.x += myGeom.x();
                global.y += myGeom.y();
                ConfigManager::instance().config().desktopIcons.append(global);
                dirty = true;
            }

            beginInsertRows({}, _icons.size(), _icons.size());
            _icons.append(entry);
            endInsertRows();
        }
    }

    if (dirty)
        ConfigManager::instance().save();
}

// ── Position update ───────────────────────────────────────────────────────────

void DesktopIconModel::setPosition(int index, int localX, int localY)
{
    if (index < 0 || index >= _icons.size())
        return;

    _icons[index].x = localX;
    _icons[index].y = localY;

    const QString path = _icons.at(index).path;
    const QRect myGeom = _screen->availableGeometry();

    for (auto &ic : ConfigManager::instance().config().desktopIcons) {
        if (ic.path == path) {
            ic.x = localX + myGeom.x();
            ic.y = localY + myGeom.y();
            break;
        }
    }
    ConfigManager::instance().save();

    emit dataChanged(this->index(index), this->index(index), { XRole, YRole });
}

// ── Helpers ───────────────────────────────────────────────────────────────────

QSet<QString> DesktopIconModel::scanDir(const QString & dir) const
{
    QSet<QString> result;
    const QFileInfoList entries = QDir(dir).entryInfoList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &fi : entries)
        result.insert(fi.absoluteFilePath());
    return result;
}

QPair<int,int> DesktopIconModel::nextFreeCell() const
{
    const auto &lay = layout();
    const QRect geom = _screen->availableGeometry();
    const int cols   = qMax(1, (geom.width() - 2 * lay.margin) / lay.stepX());

    QSet<qint64> occupied;
    for (const auto &icon : std::as_const(_icons))
        occupied.insert(cellKey(qMax(0, (icon.x - lay.margin) / lay.stepX()),
                                qMax(0, (icon.y - lay.margin) / lay.stepY())));

    int cell = 0;
    while (occupied.contains(cellKey(cell % cols, cell / cols)))
        ++cell;
    return { cell % cols, cell / cols };
}

int DesktopIconModel::indexOfPath(const QString & path) const
{
    for (int i = 0; i < _icons.size(); ++i)
        if (_icons.at(i).path == path)
            return i;
    return -1;
}

// ── QAbstractListModel interface ──────────────────────────────────────────────

int DesktopIconModel::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : _icons.size();
}

QVariant DesktopIconModel::data(const QModelIndex & idx, int role) const
{
    if (!idx.isValid() || idx.row() >= _icons.size())
        return {};

    const auto &entry = _icons.at(idx.row());

    switch (role) {
    case PathRole:
        return entry.path;
    case DisplayNameRole:
        if (entry.path.startsWith(QStringLiteral("::virtual::")))
            return entry.path.mid(11);
        return QFileInfo(entry.path).completeBaseName();
    case XRole: return entry.x;
    case YRole: return entry.y;
    default:    return {};
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
