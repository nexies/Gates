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
    QString          dir;     // explicit directory path; empty = derive from icons
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

// ── Desktop icon grid layout ─────────────────────────────────────────────────
// TODO: expose to QML (Q_GADGET or context property) and add a settings UI so
//       the user can adjust cell size, gaps and margins per monitor.
//       Serialised under [desktop.layout] in config.toml.
struct DesktopLayoutConfig {
    int cellW  = 90;   // icon delegate width  (px)
    int cellH  = 100;  // icon delegate height (px)
    int gapX   = 6;    // horizontal gap between cells (px)
    int gapY   = 6;    // vertical   gap between cells (px)
    int margin = 10;   // padding from all screen edges (px)

    // Derived helpers — keep QML stepX/stepY in sync when changing these.
    int stepX() const { return cellW + gapX; }
    int stepY() const { return cellH + gapY; }
};

struct AppConfig {
    bool                    autostart      = false;
    QString                 blurPreference = QStringLiteral("auto");
    DesktopLayoutConfig     desktopLayout;
    QList<DesktopIconEntry> desktopIcons;   // global screen coordinates
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

    void addDesktopIcon(const DesktopIconEntry & entry);
    void removeDesktopIcon(const QString & path);

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
    int                 _ignoreChanges    = 0;    // suppress N file-watcher events after save
    bool                _firstLaunch      = false;

    static ConfigManager * _instance;
};

} // namespace Gates
