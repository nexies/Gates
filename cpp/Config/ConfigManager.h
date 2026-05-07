#pragma once

#include <QObject>
#include <QString>
#include <QList>

class QFileSystemWatcher;

namespace Gates {

// ── Data structures ──────────────────────────────────────────────────────────

struct IconEntry {
    QString path;
    int     row = 0;
    int     col = 0;
};

struct FrameStyle {
    QString blur    = QStringLiteral("auto"); // "auto"|"mica"|"acrylic"|"none"
    QString color   = QStringLiteral("#1a1a2e");
    float   opacity = 0.72f;
    int     radius  = 12;
};

struct FrameConfig {
    QString          id;
    QString          name;
    QString          monitor;
    int              x           = 100;
    int              y           = 100;
    int              width       = 380;
    int              height      = 280;
    bool             collapsed   = false;
    QString          dockedEdge  = QStringLiteral("none"); // "none"|"left"|"right"|"top"|"bottom"
    FrameStyle       style;
    QList<IconEntry> icons;
};

struct DesktopIconEntry {
    QString path;
    int     x = 0;
    int     y = 0;
};

struct AppConfig {
    bool                    autostart      = false;
    QString                 blurPreference = QStringLiteral("auto");
    QList<DesktopIconEntry> desktopIcons;
    QList<FrameConfig>      frames;
};

// ── ConfigManager ────────────────────────────────────────────────────────────

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    static ConfigManager & instance();

    bool load();
    bool save();

    bool isFirstLaunch() const { return _firstLaunch; }

    const AppConfig & config() const { return _config; }
    AppConfig       & config()       { return _config; }

    void addFrame(const FrameConfig & frame);
    void removeFrame(const QString & id);
    void updateFrame(const FrameConfig & frame);

    FrameConfig * frameById(const QString & id);

    static QString newFrameId();

signals:
    void configChanged();
    void frameAdded(Gates::FrameConfig frame);
    void frameRemoved(QString id);

private:
    explicit ConfigManager(QObject * parent = nullptr);

    QString configPath() const;
    void    watchPath(const QString & path);

    AppConfig           _config;
    QFileSystemWatcher *_watcher          = nullptr;
    bool                _ignoreNextChange = false;
    bool                _firstLaunch      = false;

    static ConfigManager * _instance;
};

} // namespace Gates
