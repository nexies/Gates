#include "GatesFrameForeign.h"
#include <QQmlComponent>
#include <QQmlContext>
#include <QDir>


#include "QmlEngine.h"
#include <QDebug>
#include <QStandardPaths>

GatesFrameForeign::GatesFrameForeign(QObject *parent) :
    QObject(parent)
{
    // QQmlComponent frameComponent(&QmlEngine::instance(), QUrl("qrc:/Gates/qml/Frame.qml"));
    QQmlComponent frameComponent(&QmlEngine::instance(), QUrl("qrc:/qml/GatesFrame.qml"));

    _context = new QQmlContext(&QmlEngine::instance(), this);

    _dirModel = new DirEntryModel(this);
    _dirModel->setCurDirPath(QDir::homePath());

    _context->setContextProperty("my_fileModel", _dirModel);

    _window.reset(qobject_cast<QWindow *> (frameComponent.create(_context)));

    if(_window)
    {
        qDebug() << "[GatesFrameForeign] : new instance!";
        _window->setVisible(false);
    }
}

void GatesFrameForeign::show()
{
    setVisible(true);
}

void GatesFrameForeign::hide()
{
    setVisible(false);
}

void GatesFrameForeign::setVisible(bool visible)
{
    if(!_window)
        return;
    _window->setVisible(visible);
}

void GatesFrameForeign::setDirectory(const QString &directory)
{
    if(!_dirModel->setCurDirPath(directory))
        return;
    if(_window)
        _window->setTitle(getNameForDirectory(directory));
}

void GatesFrameForeign::setColor(const QColor &color)
{
    if(_window)
        _window->setProperty("frameColor", color);
}

void GatesFrameForeign::setOpacity(float opacity)
{
    if(_window)
        _window->setProperty("frameOpacity", opacity);
}

QString GatesFrameForeign::getNameForDirectory(const QString & path)
{
    if(false) // TODO: Frame has a config file
    {

    }

    QFileInfo pathInfo(path);

    // search if directory has a standard system name
    for(int i = 0; i < QStandardPaths::AppConfigLocation + 1; i++)
    {
        auto list = QStandardPaths::standardLocations(QStandardPaths::StandardLocation(i));
        for(auto fname: list)
            if(pathInfo.absoluteFilePath() == QFileInfo(fname).absoluteFilePath())
                return QStandardPaths::displayName(QStandardPaths::StandardLocation(i));
    }

    return QFileInfo(path).fileName();
}
