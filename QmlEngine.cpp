#include "QmlEngine.h"
#include <QApplication>
#include <QQmlContext>
#include <QFontDatabase>

#include "cpp/CustomQmlTypes/RoundedRect.h"
#include "cpp/CustomQmlTypes/GatesFrameState.h"
#include "cpp/CustomQmlTypes/IconProvider.h"
#include "cpp/CustomQmlTypes/DesktopService.h"
#include "cpp/CustomQmlTypes/ResizableFramelessWindow.h"
#include "cpp/CustomQmlTypes/FileSystemModel.h"


namespace
{
void registerQmlTypes ()
{
    qmlRegisterType<RoundedRect>("CustomQmlTypes", 1, 0, "RoundedRect");
    qmlRegisterType<GatesFrameState>("CustomQmlTypes", 1, 0, "GatesFrameState");
    qmlRegisterType<ResizableFramelessWindow>("CustomQmlTypes", 1, 0, "ResizableFramelessWindow");
    qmlRegisterType<FileSystemModel>("CustomQmlTypes", 1, 0, "FileSystemModel");

}

void setRootContextProperties(QQmlApplicationEngine * app)
{
    app->addImageProvider("gates_icon_provider", new IconProvider());
    app->rootContext()->setContextProperty("desktopService", new DesktopService(app));
}

void addFonts ()
{
    auto registerFont = [] (const QString & fontFile) -> int
    {
        int id = QFontDatabase::addApplicationFont(fontFile);
        if(id == -1)
            qDebug() << "[QmlEngine.cpp][addFonts] : failed to add font" << fontFile;
        return id;
    };

    const QString texGyreReadAdventorRegular    = ":/fonts/texgyreadventor-regular.otf";
    const QString texGyreReadAdventorBold       = ":/fonts/texgyreadventor-bold.otf";
    const QString texGyreReadAdventorItalic     = ":/fonts/texgyreadventor-italic.otf";
    const QString texGyreReadAdventorBoldItalic = ":/fonts/texgyreadventor-bolditalic.otf";

    int regular     = registerFont(texGyreReadAdventorRegular);
    int bold        = registerFont(texGyreReadAdventorBold);
    int italic      = registerFont(texGyreReadAdventorItalic);
    int boldItalic  = registerFont(texGyreReadAdventorBoldItalic);

    bool ok = !(regular == -1 && bold == -1 && italic == -1 && boldItalic == -1);

    int familyId = regular != -1 ? regular :
                   bold != -1 ? bold :
                   italic != -1 ? italic :
                   boldItalic;
    if(!ok)
    {
        qDebug() << "[QmlEngine.cpp][addFonts] : no fonts added";
        return;
    }

    QStringList vars;
    if(regular    != -1) vars << "Regular";
    if(bold       != -1) vars << "Bold";
    if(italic     != -1) vars << "Italic";
    if(boldItalic != -1) vars << "Bold Italic";
    QString family = QFontDatabase::applicationFontFamilies(familyId)[0];

    qDebug().noquote() << QString("[QmlEngine.cpp][addFonts] : Font family \"%1\" added. Available configurations: %2")
                                 .arg(family, vars.join(", "));
}

}


QQmlApplicationEngine * QmlEngine::_instance { nullptr };

QmlEngine::QmlEngine()
{

}

void QmlEngine::init()
{
    if(!_instance)
    {
        _instance = new QQmlApplicationEngine();
        QObject::connect(qApp, &QApplication::aboutToQuit,
                         _instance, &QQmlApplicationEngine::deleteLater);

        registerQmlTypes();
        setRootContextProperties(_instance);
        addFonts();
    }
}

QQmlApplicationEngine &QmlEngine::instance()
{
    if(!_instance)
        init();

    return *_instance;
}
