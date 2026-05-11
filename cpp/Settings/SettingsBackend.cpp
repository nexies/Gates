#include "SettingsBackend.h"
#include "cpp/Config/ConfigManager.h"

#include <QFileDialog>
#include <QGuiApplication>
#include <QScreen>
#include <QVariantMap>

namespace Gates {

SettingsBackend::SettingsBackend(QObject * parent)
    : QObject(parent)
{
    connect(&ConfigManager::instance(), &ConfigManager::configChanged,
            this, &SettingsBackend::onConfigChanged);
}

void SettingsBackend::onConfigChanged()
{
    emit framesChanged();
    emit desktopLayoutChanged();
}

// ── Frames ────────────────────────────────────────────────────────────────────

QVariantList SettingsBackend::frames() const
{
    QVariantList result;
    for (const auto & f : ConfigManager::instance().config().frames) {
        QVariantMap m;
        m[QStringLiteral("id")]   = f.id;
        m[QStringLiteral("name")] = f.name;
        m[QStringLiteral("dir")]  = f.dir;
        result.append(m);
    }
    return result;
}

void SettingsBackend::addFrame(const QString & name, const QString & dir)
{
    FrameConfig cfg;
    cfg.id   = ConfigManager::newFrameId();
    cfg.name = name.isEmpty() ? QStringLiteral("New Frame") : name;
    cfg.dir  = dir;
    cfg.x    = 200;
    cfg.y    = 200;
    ConfigManager::instance().addFrame(cfg);
}

void SettingsBackend::removeFrame(const QString & id)
{
    ConfigManager::instance().removeFrame(id);
}

void SettingsBackend::setFrameName(const QString & id, const QString & name)
{
    auto * cfg = ConfigManager::instance().frameById(id);
    if (!cfg || cfg->name == name) return;
    cfg->name = name;
    ConfigManager::instance().save();
    emit framesChanged();
}

void SettingsBackend::setFrameDir(const QString & id, const QString & dir)
{
    auto * cfg = ConfigManager::instance().frameById(id);
    if (!cfg || cfg->dir == dir) return;
    cfg->dir = dir;
    ConfigManager::instance().save();
    emit framesChanged();
}

QString SettingsBackend::pickDirectory()
{
    return QFileDialog::getExistingDirectory(
        nullptr,
        QStringLiteral("Выберите папку для фрейма"),
        QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
}

// ── Desktop layout ────────────────────────────────────────────────────────────

static QScreen * primaryScreen()
{
    QScreen * s = QGuiApplication::primaryScreen();
    return s ? s : (QGuiApplication::screens().isEmpty() ? nullptr : QGuiApplication::screens().first());
}

int  SettingsBackend::desktopCellW()  const { return ConfigManager::instance().config().desktopLayout.cellW;  }
int  SettingsBackend::desktopCellH()  const { return ConfigManager::instance().config().desktopLayout.cellH;  }
int  SettingsBackend::desktopGapX()   const { return ConfigManager::instance().config().desktopLayout.gapX;   }
int  SettingsBackend::desktopGapY()   const { return ConfigManager::instance().config().desktopLayout.gapY;   }
int  SettingsBackend::desktopMargin() const { return ConfigManager::instance().config().desktopLayout.margin; }
bool SettingsBackend::showDebugGrid()    const { return ConfigManager::instance().config().desktopLayout.showDebugGrid;    }
bool SettingsBackend::snapFramesToGrid() const { return ConfigManager::instance().config().desktopLayout.snapFramesToGrid; }

int SettingsBackend::desktopNumCols() const
{
    const auto & lay = ConfigManager::instance().config().desktopLayout;
    QScreen * s = primaryScreen();
    const int w = s ? s->availableSize().width() : 1920;
    return qMax(1, (w - 2 * lay.margin) / lay.stepX());
}

int SettingsBackend::desktopNumRows() const
{
    const auto & lay = ConfigManager::instance().config().desktopLayout;
    QScreen * s = primaryScreen();
    const int h = s ? s->availableSize().height() : 1080;
    return qMax(1, (h - 2 * lay.margin) / lay.stepY());
}

// Saves the layout and fires both the local signal and the live-update bridge.
static void saveLayout()
{
    ConfigManager::instance().save();
    ConfigManager::instance().notifyDesktopLayoutChanged();
}

void SettingsBackend::setDesktopNumCols(int cols)
{
    if (cols < 1) return;
    auto & lay = ConfigManager::instance().config().desktopLayout;
    QScreen * s = primaryScreen();
    const int w = s ? s->availableSize().width() : 1920;
    const int newCellW = qMax(20, (w - 2 * lay.margin - (cols - 1) * lay.gapX) / cols);
    if (lay.cellW == newCellW) return;
    lay.cellW = newCellW;
    saveLayout();
    emit desktopLayoutChanged();
}

void SettingsBackend::setDesktopNumRows(int rows)
{
    if (rows < 1) return;
    auto & lay = ConfigManager::instance().config().desktopLayout;
    QScreen * s = primaryScreen();
    const int h = s ? s->availableSize().height() : 1080;
    const int newCellH = qMax(20, (h - 2 * lay.margin - (rows - 1) * lay.gapY) / rows);
    if (lay.cellH == newCellH) return;
    lay.cellH = newCellH;
    saveLayout();
    emit desktopLayoutChanged();
}

void SettingsBackend::setDesktopGapX(int v)
{
    auto & lay = ConfigManager::instance().config().desktopLayout;
    if (lay.gapX == v) return;
    lay.gapX = v;
    saveLayout();
    emit desktopLayoutChanged();
}

void SettingsBackend::setDesktopGapY(int v)
{
    auto & lay = ConfigManager::instance().config().desktopLayout;
    if (lay.gapY == v) return;
    lay.gapY = v;
    saveLayout();
    emit desktopLayoutChanged();
}

void SettingsBackend::setDesktopMargin(int v)
{
    auto & lay = ConfigManager::instance().config().desktopLayout;
    if (lay.margin == v) return;
    lay.margin = v;
    saveLayout();
    emit desktopLayoutChanged();
}

void SettingsBackend::setShowDebugGrid(bool v)
{
    auto & lay = ConfigManager::instance().config().desktopLayout;
    if (lay.showDebugGrid == v) return;
    lay.showDebugGrid = v;
    saveLayout();
    emit desktopLayoutChanged();
}

void SettingsBackend::setSnapFramesToGrid(bool v)
{
    auto & lay = ConfigManager::instance().config().desktopLayout;
    if (lay.snapFramesToGrid == v) return;
    lay.snapFramesToGrid = v;
    saveLayout();
    emit desktopLayoutChanged();
}

} // namespace Gates
