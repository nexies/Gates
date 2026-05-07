#pragma once

#include <QObject>
#include <QHash>

class QScreen;
class QQuickWindow;

namespace Gates {

class DesktopIconModel;

// Creates and manages one full-screen transparent QQuickWindow per monitor.
// Each window sits at HWND_BOTTOM (above wallpaper, below all apps) and
// hosts the VirtualDesktop QML component showing icons not assigned to frames.
class DesktopLayer : public QObject
{
    Q_OBJECT

public:
    static DesktopLayer & instance();

    // Call once after QApplication and ConfigManager are ready.
    void init();

    // Reload icon model (e.g. after config changes).
    void reloadIcons();

    DesktopIconModel * iconModel() const { return _iconModel; }

private:
    explicit DesktopLayer(QObject * parent = nullptr);

    void createWindowForScreen(QScreen * screen);
    void destroyWindowForScreen(QScreen * screen);

    QHash<QScreen *, QQuickWindow *> _windows;
    DesktopIconModel               * _iconModel = nullptr;

    static DesktopLayer * _instance;
};

} // namespace Gates
