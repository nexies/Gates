#include "DesktopLayer.h"
#include "DesktopIconModel.h"
#include "WindowHelper.h"

#include "QmlEngine.h"
#include "cpp/Config/ConfigManager.h"

#include <QGuiApplication>
#include <QScreen>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickWindow>
#include <QDebug>

namespace Gates {

DesktopLayer * DesktopLayer::_instance = nullptr;

DesktopLayer & DesktopLayer::instance()
{
    if (!_instance)
        _instance = new DesktopLayer();
    return *_instance;
}

DesktopLayer::DesktopLayer(QObject * parent) : QObject(parent) {}

void DesktopLayer::init()
{
    _iconModel = new DesktopIconModel(this);

    // Connect to ConfigManager so we reload when icons change
    connect(&ConfigManager::instance(), &ConfigManager::configChanged,
            this, &DesktopLayer::reloadIcons);

    // Create a window for each screen
    for (QScreen * screen : QGuiApplication::screens())
        createWindowForScreen(screen);

    // React to monitor changes (Phase 5 will extend this further)
    connect(qApp, &QGuiApplication::screenAdded, this, &DesktopLayer::createWindowForScreen);
    connect(qApp, &QGuiApplication::screenRemoved, this, &DesktopLayer::destroyWindowForScreen);
}

void DesktopLayer::createWindowForScreen(QScreen * screen)
{
    if (_windows.contains(screen))
        return;

    QQmlComponent component(
        &QmlEngine::instance(),
        QUrl(QStringLiteral("qrc:/qt/qml/Gates/qml/VirtualDesktop.qml")));

    if (component.status() != QQmlComponent::Ready) {
        qCritical() << "[DesktopLayer] VirtualDesktop.qml load error:";
        for (const auto & err : component.errors())
            qCritical() << " " << err.toString();
        return;
    }

    auto * ctx = new QQmlContext(&QmlEngine::instance(), this);
    ctx->setContextProperty("desktopIconModel", _iconModel);
    ctx->setContextProperty("targetScreen", screen);

    auto * window = qobject_cast<QQuickWindow *>(component.create(ctx));
    if (!window) {
        qCritical() << "[DesktopLayer] failed to create VirtualDesktop window";
        return;
    }

    // Position on the correct screen before applying Win32 styles
    window->setScreen(screen);
    window->setGeometry(screen->geometry());

    // Keep Qt's focus system away from this window (Win32 WS_EX_NOACTIVATE alone
    // is not enough — Qt routes QFocusEvent internally without checking it)
    window->setFlag(Qt::WindowDoesNotAcceptFocus, true);

    // Apply HWND_BOTTOM + WS_EX_TOOLWINDOW + WS_EX_NOACTIVATE
    WindowHelper::makeDesktopWindow(window, /*noActivate=*/true);

    // Clean up when screen is removed
    connect(window, &QQuickWindow::destroyed, this, [this, screen]() {
        _windows.remove(screen);
    });

    window->show();
    _windows.insert(screen, window);
    qDebug() << "[DesktopLayer] created window for screen" << screen->name();
}

void DesktopLayer::destroyWindowForScreen(QScreen * screen)
{
    auto * window = _windows.take(screen);
    if (window) {
        WindowHelper::unregisterDesktopWindow(window);
        window->deleteLater();
        qDebug() << "[DesktopLayer] destroyed window for screen" << screen->name();
    }
}

void DesktopLayer::reloadIcons()
{
    if (_iconModel)
        _iconModel->reload();
}

} // namespace Gates
