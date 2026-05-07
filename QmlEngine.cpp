#include "QmlEngine.h"
#include <QApplication>
#include <QQmlContext>
#include <QFontDatabase>
#include <QDebug>

#include "cpp/CustomQmlTypes/IconProvider.h"
#include "cpp/CustomQmlTypes/DesktopService.h"
#include "cpp/CustomQmlTypes/DragDropService.h"

namespace {

void setRootContextProperties(QQmlApplicationEngine * app)
{
    app->addImageProvider("gates_icon_provider", new IconProvider());
    app->rootContext()->setContextProperty("desktopService",    new DesktopService(app));
    app->rootContext()->setContextProperty("dragDropService",   &DragDropService::instance());
}

void addFonts()
{
    auto registerFont = [](const QString & fontFile) -> int {
        int id = QFontDatabase::addApplicationFont(fontFile);
        if (id == -1)
            qDebug() << "[QmlEngine] failed to add font" << fontFile;
        return id;
    };

    int regular    = registerFont(QStringLiteral(":/fonts/texgyreadventor-regular.otf"));
    int bold       = registerFont(QStringLiteral(":/fonts/texgyreadventor-bold.otf"));
    int italic     = registerFont(QStringLiteral(":/fonts/texgyreadventor-italic.otf"));
    int boldItalic = registerFont(QStringLiteral(":/fonts/texgyreadventor-bolditalic.otf"));

    const bool ok = !(regular == -1 && bold == -1 && italic == -1 && boldItalic == -1);
    if (!ok) {
        qDebug() << "[QmlEngine] no fonts added";
        return;
    }

    const int familyId = regular != -1 ? regular : bold != -1 ? bold
                       : italic  != -1 ? italic  : boldItalic;

    QStringList vars;
    if (regular    != -1) vars << QStringLiteral("Regular");
    if (bold       != -1) vars << QStringLiteral("Bold");
    if (italic     != -1) vars << QStringLiteral("Italic");
    if (boldItalic != -1) vars << QStringLiteral("Bold Italic");

    const QString family = QFontDatabase::applicationFontFamilies(familyId).first();
    qDebug().noquote() << QString("[QmlEngine] font \"%1\" added: %2")
                                 .arg(family, vars.join(", "));
}

} // anonymous namespace


namespace Gates {

QQmlApplicationEngine * QmlEngine::_instance { nullptr };

QmlEngine::QmlEngine() {}

void QmlEngine::init()
{
    if (_instance)
        return;

    _instance = new QQmlApplicationEngine();
    QObject::connect(qApp, &QApplication::aboutToQuit,
                     _instance, &QQmlApplicationEngine::deleteLater);

    _instance->addImportPath(QStringLiteral("qrc:/qt/qml"));

    setRootContextProperties(_instance);
    addFonts();
}

QQmlApplicationEngine & QmlEngine::instance()
{
    if (!_instance)
        init();
    return *_instance;
}

} // namespace Gates
