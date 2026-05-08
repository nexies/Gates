#include "ConfigManager.h"

#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QFileSystemWatcher>
#include <QUuid>
#include <QDebug>
#include <sstream>

#include "third_party/tomlplusplus/toml.hpp"

// ── Parsing helpers ──────────────────────────────────────────────────────────

static Gates::IconEntry parseIconEntry(const toml::table & t)
{
    return {
        QString::fromStdString(t["path"].value_or(std::string{})),
        (int)t["row"].value_or(0),
        (int)t["col"].value_or(0)
    };
}

static Gates::DesktopIconEntry parseDesktopIconEntry(const toml::table & t)
{
    return {
        QString::fromStdString(t["path"].value_or(std::string{})),
        (int)t["x"].value_or(0),
        (int)t["y"].value_or(0)
    };
}

static Gates::FrameStyle parseFrameStyle(const toml::table & t)
{
    Gates::FrameStyle s;
    s.blur    = QString::fromStdString(t["blur"].value_or(std::string{"auto"}));
    s.color   = QString::fromStdString(t["color"].value_or(std::string{"#1a1a2e"}));
    s.opacity = (float)t["opacity"].value_or(0.72);
    s.radius  = (int)t["radius"].value_or(12);
    return s;
}

static Gates::FrameConfig parseFrameConfig(const toml::table & t)
{
    Gates::FrameConfig f;
    f.id         = QString::fromStdString(t["id"].value_or(std::string{}));
    f.name       = QString::fromStdString(t["name"].value_or(std::string{"Frame"}));
    f.dir        = QString::fromStdString(t["dir"].value_or(std::string{}));
    f.monitor    = QString::fromStdString(t["monitor"].value_or(std::string{}));
    f.x          = (int)t["x"].value_or(100);
    f.y          = (int)t["y"].value_or(100);
    f.width      = (int)t["width"].value_or(380);
    f.height     = (int)t["height"].value_or(280);
    f.collapsed  = t["collapsed"].value_or(false);
    f.dockedEdge = QString::fromStdString(t["docked_edge"].value_or(std::string{"none"}));

    if (const auto * style = t["style"].as_table())
        f.style = parseFrameStyle(*style);

    if (const auto * icons = t["icons"].as_array()) {
        for (const auto & item : *icons)
            if (const auto * iconTable = item.as_table())
                f.icons.append(parseIconEntry(*iconTable));
    }
    return f;
}

// ── Serialization helpers ────────────────────────────────────────────────────

static toml::table serializeStyle(const Gates::FrameStyle & s)
{
    return toml::table{
        { "blur",    s.blur.toStdString()  },
        { "color",   s.color.toStdString() },
        { "opacity", (double)s.opacity     },
        { "radius",  s.radius              }
    };
}

static toml::table serializeIconEntry(const Gates::IconEntry & e)
{
    return toml::table{
        { "path", e.path.toStdString() },
        { "row",  e.row                },
        { "col",  e.col                }
    };
}

static toml::table serializeDesktopIcon(const Gates::DesktopIconEntry & e)
{
    return toml::table{
        { "path", e.path.toStdString() },
        { "x",    e.x                  },
        { "y",    e.y                  }
    };
}

static toml::table serializeFrame(const Gates::FrameConfig & f)
{
    toml::array icons;
    for (const auto & icon : f.icons)
        icons.push_back(serializeIconEntry(icon));

    return toml::table{
        { "id",          f.id.toStdString()         },
        { "name",        f.name.toStdString()        },
        { "dir",         f.dir.toStdString()         },
        { "monitor",     f.monitor.toStdString()     },
        { "x",           f.x                         },
        { "y",           f.y                         },
        { "width",       f.width                     },
        { "height",      f.height                    },
        { "collapsed",   f.collapsed                 },
        { "docked_edge", f.dockedEdge.toStdString()  },
        { "style",       serializeStyle(f.style)     },
        { "icons",       std::move(icons)            }
    };
}

// ── ConfigManager ────────────────────────────────────────────────────────────

namespace Gates {

ConfigManager * ConfigManager::_instance = nullptr;

ConfigManager & ConfigManager::instance()
{
    if (!_instance)
        _instance = new ConfigManager();
    return *_instance;
}

ConfigManager::ConfigManager(QObject * parent) : QObject(parent)
{
    _watcher = new QFileSystemWatcher(this);

    connect(_watcher, &QFileSystemWatcher::fileChanged, this,
            [this](const QString & path)
    {
        // Some OS remove the watch after a file write — re-add it
        if (!_watcher->files().contains(path))
            _watcher->addPath(path);

        if (_ignoreChanges > 0) {
            --_ignoreChanges;
            return;
        }

        qDebug() << "[ConfigManager] config file changed externally, reloading";
        if (load())
            emit configChanged();
    });
}

QString ConfigManager::configPath() const
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + QStringLiteral("/config.toml");
}

void ConfigManager::watchPath(const QString & path)
{
    if (!_watcher->files().contains(path))
        _watcher->addPath(path);
}

bool ConfigManager::load()
{
    const QString path = configPath();

    if (!QFile::exists(path)) {
        _firstLaunch = true;
        qDebug() << "[ConfigManager] no config file, first launch";
        return false;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "[ConfigManager] cannot open config file:" << path;
        return false;
    }
    const QByteArray content = file.readAll();

    try {
        const auto root = toml::parse(std::string_view(content.constData(), content.size()));

        _config.autostart      = root["global"]["autostart"].value_or(false);
        _config.blurPreference = QString::fromStdString(
            root["global"]["blur_preference"].value_or(std::string{"auto"}));

        _config.desktopIcons.clear();
        if (const auto * arr = root["desktop"]["icons"].as_array()) {
            for (const auto & item : *arr)
                if (const auto * t = item.as_table())
                    _config.desktopIcons.append(parseDesktopIconEntry(*t));
        }

        _config.frames.clear();
        if (const auto * arr = root["frames"].as_array()) {
            for (const auto & item : *arr)
                if (const auto * t = item.as_table())
                    _config.frames.append(parseFrameConfig(*t));
        }

        watchPath(path);
        _firstLaunch = false;
        qDebug() << "[ConfigManager] loaded:" << _config.frames.size() << "frames,"
                 << _config.desktopIcons.size() << "desktop icons";
        return true;

    } catch (const std::exception & e) {
        qCritical() << "[ConfigManager] parse error:" << e.what();
        return false;
    }
}

bool ConfigManager::save()
{
    toml::array desktopIconsArr;
    for (const auto & di : _config.desktopIcons)
        desktopIconsArr.push_back(serializeDesktopIcon(di));

    toml::array framesArr;
    for (const auto & f : _config.frames)
        framesArr.push_back(serializeFrame(f));

    const toml::table root{
        { "global", toml::table{
            { "autostart",       _config.autostart                       },
            { "blur_preference", _config.blurPreference.toStdString()    }
        }},
        { "desktop", toml::table{
            { "icons", std::move(desktopIconsArr) }
        }},
        { "frames", std::move(framesArr) }
    };

    const QString path = configPath();

    std::ostringstream oss;
    oss << root;
    const std::string serialized = oss.str();

    _ignoreChanges = 3;  // some OS fire 2–3 events per write; suppress all of them

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        _ignoreChanges = 0;
        qCritical() << "[ConfigManager] cannot write config file:" << path;
        return false;
    }
    file.write(serialized.c_str(), (qint64)serialized.size());
    watchPath(path);

    qDebug() << "[ConfigManager] saved to" << path;
    return true;
}

void ConfigManager::addFrame(const FrameConfig & frame)
{
    _config.frames.append(frame);
    save();
    emit frameAdded(frame);
    emit configChanged();
}

void ConfigManager::removeFrame(const QString & id)
{
    const auto it = std::find_if(_config.frames.begin(), _config.frames.end(),
                                 [&id](const FrameConfig & f) { return f.id == id; });
    if (it == _config.frames.end())
        return;

    _config.frames.erase(it);
    save();
    emit frameRemoved(id);
    emit configChanged();
}

void ConfigManager::updateFrame(const FrameConfig & frame)
{
    for (auto & f : _config.frames) {
        if (f.id == frame.id) {
            f = frame;
            save();
            emit configChanged();
            return;
        }
    }
}

FrameConfig * ConfigManager::frameById(const QString & id)
{
    for (auto & f : _config.frames)
        if (f.id == id)
            return &f;
    return nullptr;
}

void ConfigManager::addDesktopIcon(const DesktopIconEntry & entry)
{
    _config.desktopIcons.append(entry);
    save();
    emit configChanged();
}

void ConfigManager::removeDesktopIcon(const QString & path)
{
    const int before = _config.desktopIcons.size();
    _config.desktopIcons.removeIf([&path](const DesktopIconEntry & e) {
        return e.path == path;
    });
    if (_config.desktopIcons.size() != before) {
        save();
        emit configChanged();
    }
}

QString ConfigManager::newFrameId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

} // namespace Gates
