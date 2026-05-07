#include "GatesFrameForeign.h"

#include <QQmlComponent>
#include <QQmlContext>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

#include "QmlEngine.h"
#include "cpp/Shell/WindowHelper.h"

namespace Gates {

FrameForeign::FrameForeign(const FrameConfig & cfg, QObject * parent)
    : QObject(parent)
    , _id(cfg.id)
{
    QQmlComponent frameComponent(
        &QmlEngine::instance(),
        QUrl(QStringLiteral("qrc:/qt/qml/Gates/qml/GatesFrame.qml")));

    if (frameComponent.status() == QQmlComponent::Error) {
        qCritical() << "[FrameForeign] component load error:";
        for (const auto & err : frameComponent.errors())
            qCritical() << " " << err.toString();
        return;
    }

    if (frameComponent.status() != QQmlComponent::Ready) {
        qCritical() << "[FrameForeign] component not ready, status ="
                    << frameComponent.status();
        return;
    }

    _context = new QQmlContext(&QmlEngine::instance(), this);

    _dirModel = new DirEntryModel(this);
    _context->setContextProperty("my_fileModel", _dirModel);
    _context->setContextProperty("frameId", _id);

    _window.reset(qobject_cast<QWindow *>(frameComponent.create(_context)));

    if (!_window) {
        qCritical() << "[FrameForeign] create() returned null:";
        for (const auto & err : frameComponent.errors())
            qCritical() << " " << err.toString();
        return;
    }

    qDebug() << "[FrameForeign] created frame" << _id;

    // Frames stay below regular app windows but ARE activatable (user interacts with them)
    WindowHelper::makeDesktopWindow(_window.get(), /*noActivate=*/false);

    applyConfig(cfg);
    connectWindowSignals();
}

void FrameForeign::applyConfig(const FrameConfig & cfg)
{
    if (!_window)
        return;

    _window->setPosition(cfg.x, cfg.y);
    _window->resize(cfg.width, cfg.height);
    _window->setTitle(cfg.name);
    _window->setProperty("frameColor",   QColor(cfg.style.color));
    _window->setProperty("frameOpacity", cfg.style.opacity);
    _window->setVisible(!cfg.collapsed);

    if (!cfg.icons.isEmpty())
        setDirectory(QFileInfo(cfg.icons.first().path).absolutePath());
    else if (_dirModel->currentDirPath().isEmpty())
        setDirectory(QDir::homePath());
}

void FrameForeign::connectWindowSignals()
{
    auto emitGeometry = [this]() {
        if (_window)
            emit geometryChanged(_id, _window->x(), _window->y(),
                                 _window->width(), _window->height());
    };

    connect(_window.get(), &QWindow::xChanged,      this, emitGeometry);
    connect(_window.get(), &QWindow::yChanged,      this, emitGeometry);
    connect(_window.get(), &QWindow::widthChanged,  this, emitGeometry);
    connect(_window.get(), &QWindow::heightChanged, this, emitGeometry);

    connect(_window.get(), &QWindow::visibleChanged, this, [this](bool v) {
        emit visibilityChanged(_id, v);
    });
}

void FrameForeign::show()    { setVisible(true);  }
void FrameForeign::hide()    { setVisible(false); }

void FrameForeign::setVisible(bool visible)
{
    if (_window)
        _window->setVisible(visible);
}

void FrameForeign::setDirectory(const QString & directory)
{
    if (!_dirModel->setCurDirPath(directory))
        return;
    if (_window)
        _window->setTitle(getNameForDirectory(directory));
}

void FrameForeign::setColor(const QColor & color)
{
    if (_window)
        _window->setProperty("frameColor", color);
}

void FrameForeign::setOpacity(float opacity)
{
    if (_window)
        _window->setProperty("frameOpacity", opacity);
}

QString FrameForeign::getNameForDirectory(const QString & path)
{
    const QFileInfo pathInfo(path);
    for (int i = 0; i < QStandardPaths::AppConfigLocation + 1; ++i) {
        const auto list = QStandardPaths::standardLocations(
            QStandardPaths::StandardLocation(i));
        for (const auto & fname : list)
            if (pathInfo.absoluteFilePath() == QFileInfo(fname).absoluteFilePath())
                return QStandardPaths::displayName(QStandardPaths::StandardLocation(i));
    }
    return QFileInfo(path).fileName();
}

} // namespace Gates
