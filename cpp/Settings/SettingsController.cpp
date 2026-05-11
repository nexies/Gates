#include "SettingsController.h"
#include "SettingsBackend.h"
#include "QmlEngine.h"

#include <QDebug>
#include <QQmlComponent>
#include <QQmlContext>
#include <QWindow>

namespace Gates {

SettingsController::SettingsController(QObject * parent)
    : QObject(parent)
{
    _backend = new SettingsBackend(this);
}

SettingsController::~SettingsController() = default;

void SettingsController::show()
{
    if (!_window)
        createWindow();

    if (_window) {
        _window->show();
        _window->raise();
        _window->requestActivate();
    }
}

void SettingsController::hide()
{
    if (_window)
        _window->hide();
}

bool SettingsController::isVisible() const
{
    return _window && _window->isVisible();
}

void SettingsController::createWindow()
{
    QQmlComponent comp(
        &QmlEngine::instance(),
        QUrl(QStringLiteral("qrc:/qt/qml/Gates/qml/SettingsWindow.qml")));

    if (comp.status() != QQmlComponent::Ready) {
        for (const auto & err : comp.errors())
            qCritical() << "[SettingsController]" << err.toString();
        return;
    }

    _context = new QQmlContext(&QmlEngine::instance(), this);
    _context->setContextProperty(QStringLiteral("settingsBackend"), _backend);

    auto * obj = comp.create(_context);
    _window.reset(qobject_cast<QWindow *>(obj));
    if (!_window) {
        qCritical() << "[SettingsController] SettingsWindow.qml did not create a QWindow";
        delete obj;
    }
}

} // namespace Gates
