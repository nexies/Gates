#include "GatesFrameDispatcher.h"
#include "cpp/Foreign/GatesFrameForeign.h"
#include "cpp/CustomQmlTypes/DragDropService.h"
#include "QmlEngine.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QQmlComponent>
#include <QTimer>
#include <QWindow>

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

    // Debounce geometry saves: many events fire per drag pixel; we only write
    // the final position once the user stops moving/resizing for 400 ms.
    _saveDebounce = new QTimer(this);
    _saveDebounce->setSingleShot(true);
    _saveDebounce->setInterval(400);
    connect(_saveDebounce, &QTimer::timeout, this, []() {
        ConfigManager::instance().save();
    });

    // ── DragDropService wiring ────────────────────────────────────────────────
    auto & dds = DragDropService::instance();
    dds.setFrameLocator([this](QPoint p) { return frameIdAtPos(p); });
    connect(&dds, &DragDropService::dropOnFrame,   this, &FrameDispatcher::onDropOnFrame);
    connect(&dds, &DragDropService::dropOnDesktop, this, &FrameDispatcher::onDropOnDesktop);

    // ── Ghost drag window ─────────────────────────────────────────────────────
    QQmlComponent ghostComp(&QmlEngine::instance(),
                            QUrl(QStringLiteral("qrc:/qt/qml/Gates/qml/DragGhost.qml")));
    if (ghostComp.status() == QQmlComponent::Ready) {
        _ghostWindow = qobject_cast<QWindow *>(ghostComp.create());
        if (_ghostWindow)
            static_cast<QObject *>(_ghostWindow)->setParent(this);
        else
            qWarning() << "[FrameDispatcher] DragGhost is not a Window";
    } else {
        for (const auto & err : ghostComp.errors())
            qWarning() << "[FrameDispatcher] DragGhost error:" << err.toString();
    }

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
    _saveDebounce->start(); // coalesce rapid resize/move events into one save
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

QString FrameDispatcher::frameIdAtPos(const QPoint & globalPos) const
{
    for (auto it = _frames.constBegin(); it != _frames.constEnd(); ++it) {
        const auto * frame = it.value();
        const auto * w = frame->window();
        if (!w || !w->isVisible()) continue;
        const QRect geom(w->x(), w->y(), w->width(), w->height());
        if (geom.contains(globalPos))
            return it.key();
    }
    return {};
}

void FrameDispatcher::onDropOnFrame(QString path, QString /*sourceFrameId*/, QString targetFrameId)
{
    const auto * target = _frames.value(targetFrameId);
    if (!target) return;

    const QString targetDir = target->dirPath();
    if (targetDir.isEmpty()) return;

    const QFileInfo fi(path);
    const QString targetPath = QDir(targetDir).filePath(fi.fileName());

    if (QFileInfo(targetPath).exists()) {
        qWarning() << "[FrameDispatcher] drop target already exists:" << targetPath;
        return;
    }

    if (!QFile::rename(path, targetPath))
        qWarning() << "[FrameDispatcher] failed to move" << path << "->" << targetPath;
    // QFileSystemWatcher in DirEntryModel refreshes both source and target automatically
}

void FrameDispatcher::onDropOnDesktop(QString path, QString sourceFrameId, qreal /*x*/, qreal /*y*/)
{
    if (sourceFrameId.isEmpty()) return; // desktop→desktop reposition: handled by VirtualDesktop

    const QString desktopDir =
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    const QFileInfo fi(path);
    const QString targetPath = QDir(desktopDir).filePath(fi.fileName());

    if (path == targetPath) return;

    if (!QFile::rename(path, targetPath))
        qWarning() << "[FrameDispatcher] failed to move" << path << "to desktop";
}

} // namespace Gates
