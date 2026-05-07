#include "GatesFrameDispatcher.h"
#include "cpp/Foreign/GatesFrameForeign.h"

#include <QDebug>

namespace Gates {

FrameDispatcher::FrameDispatcher(QObject * parent)
    : QObject(parent)
{
}

void FrameDispatcher::init()
{
    auto & cm = ConfigManager::instance();

    connect(&cm, &ConfigManager::configChanged,  this, &FrameDispatcher::onConfigChanged);
    connect(&cm, &ConfigManager::frameAdded,     this, &FrameDispatcher::onFrameAdded);
    connect(&cm, &ConfigManager::frameRemoved,   this, &FrameDispatcher::onFrameRemoved);

    syncFromConfig();
}

// ── Sync ─────────────────────────────────────────────────────────────────────

void FrameDispatcher::syncFromConfig()
{
    const auto & frames = ConfigManager::instance().config().frames;

    for (const auto & cfg : frames)
        if (!_frames.contains(cfg.id))
            spawnFrame(cfg);

    const QList<QString> existingIds = _frames.keys();
    for (const QString & id : existingIds) {
        const bool inConfig = std::any_of(frames.begin(), frames.end(),
                                          [&id](const FrameConfig & f) { return f.id == id; });
        if (!inConfig)
            despawnFrame(id);
    }
}

void FrameDispatcher::spawnFrame(const FrameConfig & cfg)
{
    auto * frame = new FrameForeign(cfg, this);

    connect(frame, &FrameForeign::geometryChanged,
            this,  &FrameDispatcher::onFrameGeometryChanged);

    _frames.insert(cfg.id, frame);
    frame->show();
    qDebug() << "[FrameDispatcher] spawned frame" << cfg.id << cfg.name;
}

void FrameDispatcher::despawnFrame(const QString & id)
{
    auto * frame = _frames.take(id);
    if (frame) {
        frame->hide();
        frame->deleteLater();
        qDebug() << "[FrameDispatcher] despawned frame" << id;
    }
}

// ── Slots ─────────────────────────────────────────────────────────────────────

void FrameDispatcher::onConfigChanged()
{
    const auto & frames = ConfigManager::instance().config().frames;

    for (const auto & cfg : frames)
        if (auto * frame = _frames.value(cfg.id, nullptr))
            frame->applyConfig(cfg);

    syncFromConfig();
}

void FrameDispatcher::onFrameAdded(Gates::FrameConfig cfg)
{
    if (!_frames.contains(cfg.id))
        spawnFrame(cfg);
}

void FrameDispatcher::onFrameRemoved(QString id)
{
    despawnFrame(id);
}

void FrameDispatcher::onFrameGeometryChanged(QString id, int x, int y, int w, int h)
{
    auto * cfg = ConfigManager::instance().frameById(id);
    if (!cfg || (cfg->x == x && cfg->y == y && cfg->width == w && cfg->height == h))
        return;

    cfg->x      = x;
    cfg->y      = y;
    cfg->width  = w;
    cfg->height = h;
    ConfigManager::instance().save();
}

// ── Public API ────────────────────────────────────────────────────────────────

FrameForeign * FrameDispatcher::frameById(const QString & id) const
{
    return _frames.value(id, nullptr);
}

FrameConfig * FrameDispatcher::createFrame(int x, int y, int w, int h)
{
    FrameConfig cfg;
    cfg.id     = ConfigManager::newFrameId();
    cfg.name   = QStringLiteral("New Frame");
    cfg.x      = x;
    cfg.y      = y;
    cfg.width  = w;
    cfg.height = h;

    ConfigManager::instance().addFrame(cfg);
    return ConfigManager::instance().frameById(cfg.id);
}

void FrameDispatcher::destroyFrame(const QString & id)
{
    ConfigManager::instance().removeFrame(id);
}

} // namespace Gates
